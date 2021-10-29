#pragma once

#include <string>
#include <glm/glm.hpp>

class DirectionalLight 
{
public:


	DirectionalLight() { }

	DirectionalLight(const glm::vec3& color, float intensity)
		: color(color), intensity(intensity) { }

	std::string getName() const  { return "Directional Light"; }

	glm::vec3 color{ 1.0f };

	glm::vec3 direction{ 1.0f };

	float intensity{ 1.0f };

	bool shadowsEnabled{ true };
	unsigned int shadowMap{ 0 };

	glm::mat4 view;
	glm::mat4 proj;

	float zNear{ 0.3f };
	float zFar{ 30.0f };
	float pcfRadius{ 0.001f };
};

