#include "StdAfx.h"
#include "New_ModelBMD.h"
#include <filesystem> // C++17
#include <iostream>
#include <sstream>
#ifdef jdk_shader_local330
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "New_RenderBMD.h"

#include "Utilities/Log/muConsoleDebug.h"
#include <MU_OpenGL.h>

struct ShaderUniformCache
{
	GLint uProj;
	GLint uView;
	GLint u_bodyLight;
	GLint u_lightPosition;
	GLint u_meshUV;
	GLint u_setting1;
	GLint u_setting2;
	GLint u_enableLight;
	GLint uTexture;
	GLint u_shadowMode;
	GLint u_shadowProj;
	GLint u_bodyOrigin;
};

static std::unordered_map<GLuint, ShaderUniformCache> g_UniformCache;

using namespace OGL330;
extern int WaterTextureNumber;
static float g_ProjMatrix[16];
static float g_ViewMatrix[16];
namespace OGL330MODEL
{
	std::unordered_map<int, GLuint> shaderProgramMap;
	GLuint g_CurrentShaderID = 0;
}

GLuint LoadShaderProgramFromFiles(const char* vertexPath, const char* fragmentPath)
{
	auto LoadShaderSource = [](const char* path) -> std::string
		{
			std::ifstream file(path);
			if (!file.is_open())
			{
				std::cerr << "Failed to open shader: " << path << std::endl;
				return "";
			}
			std::stringstream buffer;
			buffer << file.rdbuf();
			return buffer.str();
		};

	std::string vertCode = LoadShaderSource(vertexPath);
	const char* vertSrc = vertCode.c_str();
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertSrc, nullptr);
	glCompileShader(vertShader);

	GLint success;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(vertShader, 512, nullptr, log);
		std::cerr << "Vertex shader error [" << vertexPath << "]: " << log << std::endl;
	}

	std::string fragCode = LoadShaderSource(fragmentPath);
	const char* fragSrc = fragCode.c_str();
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragSrc, nullptr);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(fragShader, 512, nullptr, log);
		std::cerr << "Fragment shader error [" << fragmentPath << "]: " << log << std::endl;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char log[512];
		glGetProgramInfoLog(program, 512, nullptr, log);
		std::cerr << "Program link error: " << log << std::endl;
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}

void OGL330MODEL::Init()
{
	const char* shaderNames[SHADER_330_ALL] = {
	"Model",
	"BlendMesh",
	"Chrome1",
	"Chrome2",
	"Chrome3",
	"Chrome4",
	"Chrome5",
	"Chrome6",
	"Chrome7",
	"Oil",
	"Metal"
	};

	char pathBuffer[128];
	char pathBuffer2[128];

	for (int i = 0; i < SHADER_330_ALL; ++i)
	{
		snprintf(pathBuffer, sizeof(pathBuffer), "Data\\Effect\\VBO\\%s.vs", shaderNames[i]);
		snprintf(pathBuffer2, sizeof(pathBuffer2), "Data\\Effect\\VBO\\%s.fs", shaderNames[i]);
		shaderProgramMap[i] = LoadShaderProgramFromFiles(pathBuffer, pathBuffer2);
	}
}

void OGL330MODEL::UpdateCameraMatrix()
{
	glGetFloatv(GL_PROJECTION_MATRIX, g_ProjMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, g_ViewMatrix);
}

void OGL330MODEL::ConvertOldMeshToVaoMesh(GLuint iModel, bool DelMesh)
{
	if (!OGL330::IsShader()) return;

	BMD* pModel = gmClientModels->GetModel(iModel);

	if (pModel)
	{
		pModel->LoadMeshToVAO();

		pModel->UploadAllToGPU();
	}
}

void OGL330MODEL::UseShader(GLuint shaderID)
{
	if (g_CurrentShaderID != shaderID)
	{
		glUseProgram(shaderID);
		g_CurrentShaderID = shaderID;
	}
}

void OGL330MODEL::UnUseShader()
{
	if (g_CurrentShaderID != 0)
	{
		glUseProgram(0);
		g_CurrentShaderID = 0;
	}
}

void OGL330MODEL::SetTargetRender(OBJECT* pObj)
{
	if (!OGL330::IsShader()) return;

	if (pObj != NULL)
	{
		GMMeshShader->Lock(true);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		GMMeshShader->FlushAllMesh();
		GMMeshShader->Lock(false);
	}
}

void OGL330MODEL::SendUniform(GLuint shaderID,
	const mvec4& bodyLight,
	const mvec4& lightPosition,
	const mvec4& meshUV,
	const mvec4& setting1,
	const mvec4& setting2,
	const bool enableLight,
	const bool shadow,
	vec3_t vBodyOrigin)
{
	ShaderUniformCache* cache = nullptr;
	
	auto it = g_UniformCache.find(shaderID);
	if (it == g_UniformCache.end())
	{
		ShaderUniformCache newCache;

		newCache.uProj = glGetUniformLocation(shaderID, "uProj");
		newCache.uView = glGetUniformLocation(shaderID, "uView");
		newCache.u_bodyLight = glGetUniformLocation(shaderID, "u_bodyLight");
		newCache.u_lightPosition = glGetUniformLocation(shaderID, "u_lightPosition");
		newCache.u_meshUV = glGetUniformLocation(shaderID, "u_meshUV");
		newCache.u_setting1 = glGetUniformLocation(shaderID, "u_setting1");
		newCache.u_setting2 = glGetUniformLocation(shaderID, "u_setting2");
		newCache.u_enableLight = glGetUniformLocation(shaderID, "u_enableLight");
		newCache.uTexture = glGetUniformLocation(shaderID, "uTexture");
		newCache.u_shadowMode = glGetUniformLocation(shaderID, "u_shadowMode");
		newCache.u_shadowProj = glGetUniformLocation(shaderID, "u_shadowProj");
		newCache.u_bodyOrigin = glGetUniformLocation(shaderID, "u_bodyOrigin");

		g_UniformCache[shaderID] = newCache;
		cache = &g_UniformCache[shaderID];
	}
	else
	{
		cache = &it->second;
	}

	if (cache->uProj >= 0)
		glUniformMatrix4fv(cache->uProj, 1, GL_FALSE, g_ProjMatrix);

	if (cache->uView >= 0)
		glUniformMatrix4fv(cache->uView, 1, GL_FALSE, g_ViewMatrix);

	if (cache->u_bodyLight >= 0)
		glUniform4f(cache->u_bodyLight, bodyLight.x, bodyLight.y, bodyLight.z, bodyLight.w);

	if (cache->u_lightPosition >= 0)
		glUniform4f(cache->u_lightPosition, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);

	if (cache->u_meshUV >= 0)
		glUniform4f(cache->u_meshUV, meshUV.x, meshUV.y, meshUV.z, meshUV.w);

	if (cache->u_setting1 >= 0)
		glUniform4f(cache->u_setting1, setting1.x, setting1.y, setting1.z, setting1.w);

	if (cache->u_setting2 >= 0)
		glUniform4f(cache->u_setting2, setting2.x, setting2.y, setting2.z, setting2.w);

	if (cache->u_enableLight >= 0)
		glUniform1i(cache->u_enableLight, enableLight ? 1 : 0);

	if (cache->uTexture >= 0)
		glUniform1i(cache->uTexture, 0);

	if (cache->u_shadowMode >= 0)
		glUniform1i(cache->u_shadowMode, shadow ? 1 : 0);

	if (cache->u_shadowProj >= 0)
		glUniform2f(cache->u_shadowProj, 2000.0f, 4000.0f);

	if (cache->u_bodyOrigin >= 0)
		glUniform3f(cache->u_bodyOrigin, vBodyOrigin[0], vBodyOrigin[1], vBodyOrigin[2]);
}

using namespace OGL330MODEL;

CGMMeshShader::CGMMeshShader()
{
	m_Transfrom = false;
	m_Data.reserve(10);
	m_Lock = false;
	m_Enabled = true;
	memset(m_vLightPosOrg, 0, sizeof(vec3_t));
	memset(m_vLightDirOrg, 0, sizeof(vec3_t));
	memset(m_vLightPos, 0, sizeof(vec3_t));
	memset(m_vLightDir, 0, sizeof(vec3_t));
	m_finalBone = NULL;
}

CGMMeshShader::~CGMMeshShader()
{
	Release();
}

void CGMMeshShader::Release()
{
	m_Data.clear();
}

bool CGMMeshShader::IsAlpha(int iType)
{
	if (iType & RENDER_BRIGHT) return true;

	if (iType & RENDER_CHROME3) return true;
	else if (iType & RENDER_CHROME4) return true;
	else if (iType & RENDER_CHROME5) return true;
	else if (iType & RENDER_CHROME7) return true;

	return false;
}

void CGMMeshShader::MakeShaderType(int iShaderType, bool enableLight, bool bAlphaNoUse, float BlendU, float BlendV, RenderMeshVAO& r)
{
	mvec4 bodyLight;

	if (bAlphaNoUse)
	{
		bodyLight.x = r.m_isColor.x * r.m_isAlpha;
		bodyLight.y = r.m_isColor.y * r.m_isAlpha;
		bodyLight.z = r.m_isColor.z * r.m_isAlpha;
		bodyLight.w = 1.f;
	}
	else
	{
		bodyLight.x = r.m_isColor.x;
		bodyLight.y = r.m_isColor.y;
		bodyLight.z = r.m_isColor.z;
		bodyLight.w = r.m_isAlpha;
	}

	mvec4 lightPosition;

	lightPosition.w = r.m_isAlpha;

	if (enableLight)
	{
		lightPosition.x = m_vLightDir[0];
		lightPosition.y = m_vLightDir[1];
		lightPosition.z = m_vLightDir[2];
	}
	else
	{
		lightPosition.x = 0.f;
		lightPosition.y = 0.f;
		lightPosition.z = 0.f;
	}

	mvec4 meshUV;
	mvec4 setting1;
	mvec4 setting2;

	switch (iShaderType)
	{
	case SHADER_330_BLENDMESH:
	{
		meshUV.x = BlendU;
		meshUV.y = BlendV;
		meshUV.z = 1.f;
		meshUV.w = 0.f;
	}
	break;
	case SHADER_330_CHROME1:
	{
		setting1.x = 1.f;
		setting1.y = 0.f;
		setting1.z = (int)WorldTime % 10000 * 0.0001f;
		setting1.w = 0.f;

		setting2.x = 0.5f;
		setting2.y = 0.5f;
		setting2.z = 2.0f;
		setting2.w = 1.f;
	}
	break;
	case SHADER_330_CHROME2:
	{
		setting1.x = 1.f;
		setting1.y = 0.f;
		setting1.z = 0.f;
		setting1.w = (int)WorldTime % 5000 * 0.00024f - 0.4f;

		setting2.x = 0.8f;
		setting2.y = 2.f;
		setting2.z = 1.f;
		setting2.w = 3.f;
	}
	break;
	case SHADER_330_CHROME3:
	{
		setting2.x = 0.0;
		setting2.y = -0.1f;
		setting2.z = -0.8f;
		setting2.w = 1.f;
	}
	break;
	case SHADER_330_CHROME4:
	{
		setting1.x = cosf(WorldTime * 0.001f);
		setting1.y = sinf(WorldTime * 0.002f);
		setting1.z = 1.f;
		setting1.w = (int)WorldTime % 10000 * 0.0001f;

		meshUV.x = BlendU;
		meshUV.y = BlendV;
		meshUV.z = 1.f;
		meshUV.w = 0.f;

		setting2.x = 0.5f;
		setting2.y = 3.f;
		setting2.z = 0.5f;
		setting2.w = 3.f;
	}
	break;
	case SHADER_330_CHROME5:
	{
		setting1.x = cosf(WorldTime * 0.001f);
		setting1.y = sinf(WorldTime * 0.002f);
		setting1.z = 1.f;
		setting1.w = (int)WorldTime % 10000 * 0.0001f;

		setting2.x = 2.5f;
		setting2.y = 1.f;
		setting2.z = 3.f;
		setting2.w = 5.f;
	}
	break;
	case SHADER_330_CHROME6:
	{
		setting1.x = 0.8f;
		setting1.y = 2.f;
		setting1.z = (int)WorldTime % 5000 * 0.00024f - 0.4f;
		setting1.w = 0.f;
	}
	break;
	case SHADER_330_CHROME7:
	{
		setting1.x = 0.8f;
		setting1.y = 0.8f;
		setting1.z = WorldTime;
		setting1.w = 0.00006f;
	}
	break;
	case SHADER_330_METAL:
	{
		setting2.x = 0.5f;
		setting2.y = 0.2f;
		setting2.z = 0.5f;
		setting2.w = 0.5f;
	}
	break;
	case SHADER_330_OIL:
	{
		meshUV.x = BlendU;
		meshUV.y = BlendV;
		meshUV.z = 1.f;
		meshUV.w = 0.f;
	}
	break;
	}

	r.m_bodyLight = bodyLight;
	r.m_lightPosition = lightPosition;
	r.m_meshUV = meshUV;
	r.m_setting1 = setting1;
	r.m_setting2 = setting2;

	r.m_Shader = shaderProgramMap[iShaderType];
}

void CGMMeshShader::SetHighLight(bool bHighLight, bool bBattleCastle)
{
	if (bHighLight)
	{
		Vector(1.3f, 0.f, 2.f, m_vLightPosOrg);
	}
	else if (bBattleCastle)
	{
		Vector(0.5f, -1.f, 1.f, m_vLightPosOrg);
	}
	else
	{
		Vector(0.f, -1.5f, 0.f, m_vLightPosOrg);
	}

	VectorCopy(m_vLightPosOrg, m_vLightDirOrg);
}

void CGMMeshShader::AddMeshCommand(BMD* pSrc, int idx, int RFlag, float Alpha, int BlendMesh, float Light, float BlendU, float BlendV, int Texture)
{
	VAOMesh& rMesh = pSrc->New_Meshs[idx];

	int iSourceTex = pSrc->IndexTexture[rMesh.Texture];

	if (iSourceTex == BITMAP_HIDE || (RFlag != RENDER_TEXTURE && (iSourceTex == BITMAP_SKIN
		|| iSourceTex == BITMAP_HQSKIN
		|| iSourceTex == BITMAP_HQSKIN2
		|| iSourceTex == BITMAP_HQSKIN3
		|| iSourceTex == BITMAP_HAIR
		|| iSourceTex == BITMAP_HQHAIR))
		)
	{
		return;
	}

	switch (iSourceTex)
	{
	case BITMAP_SKIN:
		if (pSrc->HideSkin == true)
			return;
		iSourceTex = pSrc->Skin + BITMAP_SKIN;
		break;
	case BITMAP_HQSKIN:
		if (pSrc->HideSkin == true)
			return;
		iSourceTex = pSrc->Skin + BITMAP_HQSKIN;
		break;
	case BITMAP_HQSKIN2:
		if (pSrc->HideSkin == true)
			return;
		iSourceTex = pSrc->Skin + BITMAP_HQSKIN2;
		break;
	case BITMAP_HQSKIN3:
		if (pSrc->HideSkin == true)
			return;
		iSourceTex = pSrc->Skin + BITMAP_HQSKIN3;
		break;
	case BITMAP_WATER:
		iSourceTex = (int)WaterTextureNumber + BITMAP_WATER;
		break;
	case BITMAP_HAIR:
		if (pSrc->HideSkin == true)
			return;
		iSourceTex = pSrc->Skin + BITMAP_HAIR;
		break;
	case BITMAP_HQHAIR:
		if (pSrc->HideSkin == true)
			return;
		iSourceTex = pSrc->Skin + BITMAP_HQHAIR;
		break;
	}

	if (Texture != -1)
		iSourceTex = Texture;

	if (iSourceTex != -1)
	{
		if (pSrc->LightTexture[rMesh.Texture] != 0 && (pSrc->HideSkin || RFlag != RENDER_TEXTURE))
		{
			return;
		}

		Mesh_t& rOldMesh = pSrc->Meshs[idx];
		m_Data.push_back(RenderMeshVAO());
		RenderMeshVAO& rNew = m_Data[m_Data.size() - 1];
		rNew.m_OldBMD = pSrc;
		rNew.m_IndexMesh = idx;
		rNew.m_FlagRender = RFlag;
		rNew.m_isAlpha = Alpha > 0.99 ? 1.f : Alpha;
		rNew.m_isLight = !pSrc->bOffLight;
		rNew.m_isLight = pSrc->LightEnable;

		memcpy(&rNew.m_isColor.x, pSrc->BodyLight, sizeof(float) * 3);

		int iShaderType = SHADER_330_NONE;

		if (BITMAP_HIDE == iSourceTex && RFlag != (RENDER_SHADOWMAP | RENDER_TEXTURE))
		{
			return;
		}

		OGL330::GetTexturShader(rNew.m_FlagRender, rNew.m_TextureID, iShaderType);

		if (rNew.m_TextureID == -1)
			rNew.m_TextureID = iSourceTex;

		if (Texture != -1)
			rNew.m_TextureID = Texture;

		if (iShaderType == SHADER_330_NONE)
		{
			if (rMesh.m_csTScript)
			{
				if (rMesh.m_csTScript->getStreamMesh())
				{
					iShaderType = SHADER_330_BLENDMESH;
				}
			}
			else if (idx == BlendMesh || idx == pSrc->StreamMesh)
			{
				iShaderType = SHADER_330_BLENDMESH;
				rNew.m_isLight = false;
			}

			if (BlendMesh <= -2 || rMesh.Texture == BlendMesh)
			{
				if (!(rNew.m_FlagRender & RENDER_BRIGHT))
					rNew.m_FlagRender |= RENDER_BRIGHT;

				if (BlendU != 0.f || BlendV != 0.f)
				{
					iShaderType = SHADER_330_BLENDMESH;
				}

				rNew.m_isColor.x *= Light;
				rNew.m_isColor.y *= Light;
				rNew.m_isColor.z *= Light;
				rNew.m_isAlpha = 1.f;
			}
		}
		else
		{
			if (rOldMesh.NoneBlendMesh)
			{
				m_Data.pop_back();
				return;
			}

			if (rMesh.m_csTScript)
			{
				if (rMesh.m_csTScript->getNoneBlendMesh())
				{
					m_Data.pop_back();
					return;
				}
			}

			rNew.m_isLight = false;
		}

		if (rNew.m_isLight)
		{
			vec34_t mat;
			Vector(0.f, 0.f, -45.f, pSrc->ShadowAngle);
			AngleMatrix(pSrc->ShadowAngle, mat);
			VectorIRotate(m_vLightDirOrg, mat, m_vLightDir);
		}

		MakeShaderType(iShaderType, rNew.m_isLight, IsAlpha(RFlag), BlendU, BlendV, rNew);
	}
}

void CGMMeshShader::AddBoneTransform(BMD* model, float(*BoneMatrix)[3][4], bool trans)
{
	m_Transfrom = trans;
	m_finalBone = BoneMatrix[0][0];
}

void CGMMeshShader::FlushAllMesh()
{
	m_Lock = false;
	for (MeshVAO::iterator iter = m_Data.begin(); iter != m_Data.end(); ++iter)
	{
		g_NewRenderBMD->Render(*iter);
	}

	m_Data.resize(0);
}

#endif