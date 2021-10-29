#pragma once

#include <glm/glm.hpp>
#include <vector>

struct SHVertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

enum class LightingMode
{
	NoShadow,
	Shadow,
	InterRefl
};

std::vector<float> computeVertexSHCoefs(
	const SHVertex* vertices,
	int             vertexCount,
	float           vertexAlbedo,
	int             maxOrder,
	int             samplesPerVertex,
	LightingMode    mode);



