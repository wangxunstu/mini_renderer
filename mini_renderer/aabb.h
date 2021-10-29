#pragma once

#include<glm/glm.hpp>

class aabb
{
public:
	glm::vec3 low;
	glm::vec3 high;

	aabb() noexcept;
	aabb(const glm::vec3& low, const glm::vec3& high) noexcept;

	bool is_valid() const noexcept;

	bool contains(const glm::vec3& p) const noexcept;

	aabb operator|(const glm::vec3& rhs) const noexcept;
	aabb& operator|=(const glm::vec3& rhs) noexcept;

	aabb operator|(const aabb& rhs) const noexcept;
	aabb& operator|=(const aabb& rhs) noexcept;

	aabb operator*(float rhs) const noexcept;
	aabb& operator*=(float rhs) noexcept;
};

