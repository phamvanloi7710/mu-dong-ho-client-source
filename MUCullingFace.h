#pragma once
class MUCullingFace
{
public:
	MUCullingFace();
	void Frustum3DPlanes(vec3_t position);
	void Frustrum2DFromCamera(const vec3_t position, float fovDegrees, float nearDist, float farDist);
	bool SphereInFrustum(const vec3_t center, float radius, float visibleRatio = 0.25f);
	bool CheckFrustrum2DFromCamera(float x, float y, float Range);
private:
	float fFrustrumX[4];
	float fFrustrumY[4];
	int iFrustrumBoundMinX;
	int iFrustrumBoundMinY;
	int iFrustrumBoundMaxX;
	int iFrustrumBoundMaxY;
	float fFrustumPlane[6][4]; // Cada plano: [A, B, C, D]
};

extern MUCullingFace gCullingFace;