#pragma once
#include "aabb.h"
#include <algorithm>


aabb::aabb() noexcept
{

}

aabb::aabb(const glm::vec3& low, const glm::vec3& high) noexcept
	: low(low), high(high)
{

}

 bool aabb::is_valid() const noexcept
{
	return low.x <= high.x && low.y <= high.y && low.z <= high.z;
}

 bool aabb::contains(const glm::vec3& p) const noexcept
{
	return low.x <= p.x && p.x <= high.x &&
		low.y <= p.y && p.y <= high.y &&
		low.z <= p.z && p.z <= high.z;
}

aabb aabb::operator|(const glm::vec3& rhs) const noexcept
{

	return aabb(glm::vec3(std::min(low.x, rhs.x),
		std::min(low.y, rhs.y),
		std::min(low.z, rhs.z)),

		glm::vec3(glm::vec3(std::max(high.x, rhs.x),
			                std::max(high.y, rhs.y),
			                std::max(high.z, rhs.z)))
	);
}

aabb& aabb::operator|=(const glm::vec3& rhs) noexcept
{
	return *this = *this | rhs;
}

aabb aabb::operator|(
	const aabb& rhs) const noexcept
{
	return aabb(glm::vec3(std::min(low.x, rhs.low.x), 
		                  std::min(low.y, rhs.low.y), 
		                  std::min(low.z, rhs.low.z)),

		glm::vec3(glm::vec3(std::max(high.x, rhs.high.x),
			                std::max(high.y, rhs.high.y),
			                std::max(high.z, rhs.high.z)))
	);
}



aabb& aabb::operator|=(
	const aabb& rhs) noexcept
{
	return *this = *this | rhs;
}

 aabb aabb::operator*(
	float rhs) const noexcept
{
	return aabb(low * rhs, high * rhs);
}

 aabb& aabb::operator*=(
	float rhs) noexcept
{
	return *this = *this * rhs;
}

aabb operator|(
	const glm::vec3& lhs, const aabb& rhs) noexcept
{
	return rhs | lhs;
}

 aabb operator*(float lhs, const aabb& rhs) noexcept
{
	return rhs * lhs;
}