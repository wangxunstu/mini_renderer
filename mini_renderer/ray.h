#pragma once
#include <limits>
#include <glm/glm.hpp>

class Ray
{
public:
	glm::vec3 o;
	glm::vec3 d;
	float t0;
	float t1;

	Ray()
		: Ray({}, { 1, 0, 0 })
	{

	}

	Ray(
		const glm::vec3& o,
		const glm::vec3& d,
		float         t0 = 0,
		float         t1 = std::numeric_limits<float>::infinity())
		: o(o), d(d), t0(t0), t1(t1)
	{

	}

	glm::vec3 at(float t) const
	{
		return o + t * d;
	}

	bool isBetween(float t) const
	{
		return t0 <= t && t <= t1;
	}
};
