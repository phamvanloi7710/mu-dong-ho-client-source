#pragma once

#ifdef USING_SHADER_330

#include "PL/Shader/PL_ModelBMD.h"

class BMD;
class CGMNewRenderBMD
{
public:
	virtual void Render(OGL330MODEL::RenderMeshVAO&) = 0;
};

extern CGMNewRenderBMD* g_NewRenderBMD;

class CGMShaderBMD : public CGMNewRenderBMD
{
public:
	CGMShaderBMD() {}
	~CGMShaderBMD() {}
	void Render(OGL330MODEL::RenderMeshVAO&);
};

#endif