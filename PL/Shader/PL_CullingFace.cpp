#include "stdafx.h"
#include "MapManager.h"
#include "CameraManager.h"
#include "CameraMove.h"
#include "PL/Shader/PL_CullingFace.h"

MUCullingFace gCullingFace;

MUCullingFace::MUCullingFace()
{
	iFrustrumBoundMinX = 0;
	iFrustrumBoundMinY = 0;
	iFrustrumBoundMaxX = TERRAIN_SIZE_MASK;
	iFrustrumBoundMaxY = TERRAIN_SIZE_MASK;
}

void MUCullingFace::Frustum3DPlanes(vec3_t position)
{
	float proj[16];
	float clip[16];
	float modelview[16];

	glGetFloatv(GL_PROJECTION_MATRIX, proj);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	// Multiplicacion OpenGL: clip = proj * modelview
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			clip[i + j * 4] =
				modelview[0 + j * 4] * proj[i + 0] +
				modelview[1 + j * 4] * proj[i + 4] +
				modelview[2 + j * 4] * proj[i + 8] +
				modelview[3 + j * 4] * proj[i + 12];
		}
	}

	// Extract and normalize planes
	auto normalize = [](float* p) {
		float length = sqrtf(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
		if (length > 0.0f) {
			p[0] /= length; p[1] /= length; p[2] /= length; p[3] /= length;
		}
		};

	// Left
	fFrustumPlane[0][0] = clip[3] + clip[0];
	fFrustumPlane[0][1] = clip[7] + clip[4];
	fFrustumPlane[0][2] = clip[11] + clip[8];
	fFrustumPlane[0][3] = clip[15] + clip[12];
	normalize(fFrustumPlane[0]);

	// Right
	fFrustumPlane[1][0] = clip[3] - clip[0];
	fFrustumPlane[1][1] = clip[7] - clip[4];
	fFrustumPlane[1][2] = clip[11] - clip[8];
	fFrustumPlane[1][3] = clip[15] - clip[12];
	normalize(fFrustumPlane[1]);

	// Bottom
	fFrustumPlane[2][0] = clip[3] + clip[1];
	fFrustumPlane[2][1] = clip[7] + clip[5];
	fFrustumPlane[2][2] = clip[11] + clip[9];
	fFrustumPlane[2][3] = clip[15] + clip[13];
	normalize(fFrustumPlane[2]);

	// Top
	fFrustumPlane[3][0] = clip[3] - clip[1];
	fFrustumPlane[3][1] = clip[7] - clip[5];
	fFrustumPlane[3][2] = clip[11] - clip[9];
	fFrustumPlane[3][3] = clip[15] - clip[13];
	normalize(fFrustumPlane[3]);

	// Near
	fFrustumPlane[4][0] = clip[3] + clip[2];
	fFrustumPlane[4][1] = clip[7] + clip[6];
	fFrustumPlane[4][2] = clip[11] + clip[10];
	fFrustumPlane[4][3] = clip[15] + clip[14];
	normalize(fFrustumPlane[4]);

	// Far
	fFrustumPlane[5][0] = clip[3] - clip[2];
	fFrustumPlane[5][1] = clip[7] - clip[6];
	fFrustumPlane[5][2] = clip[11] - clip[10];
	fFrustumPlane[5][3] = clip[15] - clip[14];

	normalize(fFrustumPlane[5]);


	if (SceneFlag == MAIN_SCENE)
	{
		float FrustrumMaxX = (float)TERRAIN_SIZE * TERRAIN_SCALE;
		float FrustrumMaxY = (float)TERRAIN_SIZE * TERRAIN_SCALE;
		float FrustrumMinX = 0.f;
		float FrustrumMinY = 0.f;

		int tileWidth = 4;

		iFrustrumBoundMinX = (int)(FrustrumMinX / TERRAIN_SCALE) / tileWidth * tileWidth - tileWidth;
		iFrustrumBoundMinY = (int)(FrustrumMinY / TERRAIN_SCALE) / tileWidth * tileWidth - tileWidth;
		iFrustrumBoundMaxX = (int)(FrustrumMaxX / TERRAIN_SCALE) / tileWidth * tileWidth + tileWidth;
		iFrustrumBoundMaxY = (int)(FrustrumMaxY / TERRAIN_SCALE) / tileWidth * tileWidth + tileWidth;

		if (iFrustrumBoundMinX < 0)
			iFrustrumBoundMinX = 0;

		if (iFrustrumBoundMinY < 0)
			iFrustrumBoundMinY = 0;

		if (iFrustrumBoundMaxX < 0)
			iFrustrumBoundMaxX = 0;

		if (iFrustrumBoundMaxY < 0)
			iFrustrumBoundMaxY = 0;

		if (iFrustrumBoundMinX > TERRAIN_SIZE_MASK - tileWidth)
			iFrustrumBoundMinX = TERRAIN_SIZE_MASK - tileWidth;

		if (iFrustrumBoundMinY > TERRAIN_SIZE_MASK - tileWidth)
			iFrustrumBoundMinY = TERRAIN_SIZE_MASK - tileWidth;

		if (iFrustrumBoundMaxX > TERRAIN_SIZE_MASK - tileWidth)
			iFrustrumBoundMaxX = TERRAIN_SIZE_MASK - tileWidth;

		if (iFrustrumBoundMaxY > TERRAIN_SIZE_MASK - tileWidth)
			iFrustrumBoundMaxY = TERRAIN_SIZE_MASK - tileWidth;

		this->Frustrum2DFromCamera(position, CameraFOV, CameraViewNear, CameraViewFar);
	}
}

void MUCullingFace::Frustrum2DFromCamera(const vec3_t position, float fovDegrees, float nearDist, float farDist)
{
	const float aspect = 1.f; // en 2D no importa, pero podes ajustar si queres rango horizontal distinto
	const float fovRadians = fovDegrees * Q_PI / 180.f;
	const float halfFOV = tanf(fovRadians * 0.5f);

	ResolutionConfig* conf = gwinhandle->LoadCurrentConfig();

	float WidthFar, WidthNear;

	if (conf)
	{
		WidthFar = conf->width_far * aspect;
		WidthNear = conf->width_near * aspect;
	}
	else
	{
		WidthFar = 1190.0f * aspect; // 1140.f
		WidthNear = 540.0f * aspect; // 540.f
	}

	const float nearHalfWidth = WidthNear;
	const float farHalfWidth = WidthFar;

	vec3_t p[4];
	Vector(-farHalfWidth, farDist, 0.f, p[0]);
	Vector(farHalfWidth, farDist, 0.f, p[1]);
	Vector(nearHalfWidth, nearDist, 0.f, p[2]);
	Vector(-nearHalfWidth, nearDist, 0.f, p[3]);

	vec3_t angle;
	if (World == WD_73NEW_LOGIN_SCENE || World == WD_77NEW_LOGIN_SCENE)
	{
		VectorScale(CameraAngle, -1.0f, angle);

		if (World == WD_73NEW_LOGIN_SCENE)
		{
			CCameraMove::GetInstancePtr()->SetFrustumAngle(89.5f);
			vec3_t _Temp = { CCameraMove::GetInstancePtr()->GetFrustumAngle(), 0.0f, 0.0f };
			VectorAdd(angle, _Temp, angle);
		}
	}
	else if (World == WD_55LOGINSCENE)
	{
		VectorSet(angle, 0.f, 0.f, CameraAngle[2]);
	}
	else
	{
		VectorSet(angle, 0.f, 0.f, -CameraAngle[2]);
	}

	float mat[3][4];
	AngleMatrix(angle, mat);

	for (int i = 0; i < 4; ++i)
	{
		vec3_t rotated;
		VectorRotate(p[i], mat, rotated);
		VectorAdd(rotated, CameraPosition, rotated);
		fFrustrumX[i] = rotated[0] * 0.01f; // convertir a escala del terreno
		fFrustrumY[i] = rotated[1] * 0.01f;
	}
}

bool MUCullingFace::SphereInFrustum(const vec3_t center, float radius, float visibleRatio)
{
	const float tolerance = radius * visibleRatio; // expandimos un 25%
	float adjustedRadius = radius + tolerance;

	for (int i = 0; i < 6; ++i)
	{
		float d = fFrustumPlane[i][0] * center[0] +
			fFrustumPlane[i][1] * center[1] +
			fFrustumPlane[i][2] * center[2] +
			fFrustumPlane[i][3];

		if (d < -adjustedRadius)
			return false;
	}
	return true;
}

bool MUCullingFace::CheckFrustrum2DFromCamera(float x, float y, float Range)
{
	int j = 3;
	for (int i = 0; i < 4; j = i, i++)
	{
		float distance = (fFrustrumX[i] - x) * (fFrustrumY[j] - y) - (fFrustrumX[j] - x) * (fFrustrumY[i] - y);

		if (distance <= Range)
			return false;
	}

	return true;
}
