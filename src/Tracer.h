#pragma once

#include "glm/glm.hpp"
#include "Types.h"
#include "Scene.h"

#include "string"
#include "atlimage.h"

class CTracer
{
public:
	SRay MakeRay(glm::uvec2 pixelPos, float coef1, float coef2);  // Create ray for specified pixel
	glm::vec3 TraceRay(SRay ray); // Trace ray, compute its color
	void RenderImage(int xRes, int yRes);
	void SaveImageToFile(std::string fileName);
	float RayFlatnessIntersection(SRay ray);
	float RaySphereIntersection(SRay ray, glm::vec3 spos, float t);
	CImage* LoadImageFromFile(std::string fileName);
	CImage *stars;
	CImage *disk;
	CImage *earth;

public:
	SCamera m_camera;
	CScene* m_pScene;
};
