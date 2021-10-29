#include "Material.h"

Material::Material(std::shared_ptr<Shader>shader):m_shader(shader)
{
}

void Material::setShader(std::shared_ptr<Shader> shader)
{
	m_shader = shader;
}

void Material::setTexture2D(const TextureName& textureName, TextureID textureID)
{
	m_textures2D[textureName] = textureID;
}

void Material::setTextureCubeMap(const TextureName& textureName, TextureID textureID)
{
	m_texturesCubeMap[textureName] = textureID;
}

void Material::use()
{
	use(m_shader.get(), true);
}

void Material::use(Shader* shader, bool bind)
{
	assert(shader);

	if (bind)
		shader->Bind();

	for (auto& p : m_floatMap)
		shader->setFloat(p.first, p.second);

	for (auto& p : m_integerMap)
		shader->setInt(p.first, p.second);

	for (auto& p : m_vec2Map)
		shader->setVec2(p.first, p.second);	

	for (auto& p : m_vec3Map)
		shader->setVec3(p.first, p.second);

	for (auto& p : m_vec4Map)
		shader->setVec4(p.first, p.second);

	for (auto& p : m_mat2Map)
		shader->setMat2(p.first, p.second);

	for (auto& p : m_mat3Map)
		shader->setMat3(p.first, p.second);

	for (auto& p : m_mat4Map)
		shader->setMat4(p.first, p.second);

	GLint textureUnit = 0;

	for (auto& tp : m_textures2D)
		shader->bindTexture2D(tp.second, tp.first, textureUnit++);

	for (auto& tp : m_texturesCubeMap)
		shader->bindTextureCubeMap(tp.second, tp.first,textureUnit++);

}
