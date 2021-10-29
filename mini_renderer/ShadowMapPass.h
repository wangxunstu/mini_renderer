#pragma once
#include "RenderPass.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader;
class Model;
class DirectionalLight;
class FrameBuffer;


#define  MAX_DIR_LIGHT_COUNT 100

class ShadowMapPass:public RenderPass
{
public:
	ShadowMapPass(unsigned int shadowMapResolution);

	void update()override;

	unsigned int getDepthTexture(int index)const;

	void setProjSize(glm::vec2 projSize) { m_projectionSize = projSize; }
 
	void render(glm::mat4 view, glm::mat4 proj) const;

	void addModel(const std::shared_ptr<Model> model);
	void addModel(const std::vector <std::shared_ptr<Model>>models);

	void addLight(const std::shared_ptr<DirectionalLight>& light);
	void addLight(const std::vector<std::shared_ptr<DirectionalLight>>& lights);

private:

	std::vector<std::shared_ptr<Model>> m_models;

	std::vector<std::shared_ptr<DirectionalLight> > m_lights;

	std::unique_ptr<FrameBuffer> m_framebuffers[MAX_DIR_LIGHT_COUNT];

	std::shared_ptr<Shader> m_shader;

	std::shared_ptr<Shader> m_quadShader;

	glm::vec2 m_projectionSize{ 20.0f, 20.0f };

	uint32_t m_resolution;
};

