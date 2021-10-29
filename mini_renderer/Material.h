#pragma once

#include <string>
#include <unordered_map>
#include <glm/detail/type_vec3.hpp>
#include <memory>
#include "shader.h"

using TextureName = std::string;

using TextureID = GLuint;

using ShaderProgram = GLuint;

using UniformName = std::string;

enum class ShadingType
{
	BLIN_PHONG=0,
	
	PBR,	
};

enum class RenderPath
{
	RENDER_FORWARD = 0,
	RENDER_DEFER
};

class Material
{
public:
	Material(std::shared_ptr<Shader>shader);

	void setShader(std::shared_ptr<Shader> shader);

	void setTexture2D(const TextureName& textureName, TextureID textureID);

	void setTextureCubeMap(const TextureName& textureName, TextureID textureID);

	std::shared_ptr<Shader> getShader() const { return m_shader; }

	void setFloat(const UniformName& uniformName, float v) noexcept { m_floatMap[uniformName] = v; }
	
	void setInt(const UniformName& uniformName, unsigned int v) noexcept { m_integerMap[uniformName] = v; }

	void setVector(const UniformName& uniformName, const glm::vec2& v) noexcept { m_vec2Map[uniformName] = v; }

	void setVector(const UniformName& uniformName, const glm::vec3& v) noexcept { m_vec3Map[uniformName] = v; }

	void setVector(const UniformName& uniformName, const glm::vec4& v) noexcept { m_vec4Map[uniformName] = v; }

	void setColor(const UniformName& uniformName, const glm::vec3& v) noexcept { m_vec3Map[uniformName] = v; }

	void setColor(const UniformName& uniformName, const glm::vec4& v) noexcept { m_vec4Map[uniformName] = v; }

	void setMatrix(const UniformName& uniformName, const glm::mat2& m) noexcept { m_mat2Map[uniformName] = m; }

	void setMatrix(const UniformName& uniformName, const glm::mat3& m) noexcept { m_mat3Map[uniformName] = m; }

	void setMatrix(const UniformName& uniformName, const glm::mat4& m) noexcept { m_mat4Map[uniformName] = m; }

	void use();
	void use(Shader* shader, bool bind = false);

private:
	std::shared_ptr<Shader> m_shader;
	
	std::unordered_map<TextureName, TextureID> m_textures2D;
	
	std::unordered_map<TextureName, TextureID> m_texturesCubeMap;

	std::unordered_map<UniformName, float> m_floatMap;
	std::unordered_map<UniformName, unsigned int> m_integerMap;
	std::unordered_map<UniformName, glm::vec2> m_vec2Map;
	std::unordered_map<UniformName, glm::vec3> m_vec3Map;
	std::unordered_map<UniformName, glm::vec4> m_vec4Map;
	std::unordered_map<UniformName, glm::mat2> m_mat2Map;
	std::unordered_map<UniformName, glm::mat3> m_mat3Map;
	std::unordered_map<UniformName, glm::mat4> m_mat4Map;

};
