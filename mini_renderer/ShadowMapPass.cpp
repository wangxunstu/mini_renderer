#include "ShadowMapPass.h"
#include "shader.h"
#include "DirectionalLight.h"
#include "model.h"
#include "FrameBuffer.h"


ShadowMapPass::ShadowMapPass(unsigned int shadowMapResolution):RenderPass("ShadowMapPass"),m_resolution(shadowMapResolution)
{
	for (int i = 0; i < MAX_DIR_LIGHT_COUNT; i++)
	{
		m_framebuffers[i] = std::make_unique<FrameBuffer>();
		m_framebuffers[i]->AttachDepthBuffer("depthBuffer", shadowMapResolution, shadowMapResolution, true);

		m_framebuffers[i]->Finish();
	}

	m_shader = std::make_shared<Shader>("shader/depth.vs", "shader/depth.fs");
}

void ShadowMapPass::update()
{
	glEnable(GL_POLYGON_OFFSET_FILL);

	for (unsigned int i = 0; i < m_lights.size(); i++)
	{
		m_framebuffers[i]->Bind();

		glViewport(0, 0, m_resolution, m_resolution);

		float w = m_projectionSize.x * 0.5f;
		float h = m_projectionSize.y * 0.5f;

		glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
		float val = std::abs(glm::dot(m_lights[i]->direction, up));

		if (std::abs(val - 1.0f) < 0.00001f)
		{
			up = glm::vec3(0.0f,0.0f,1.0f);
		}

	    glm::mat4  lightProjection = glm::ortho(-w, w, -h, h, m_lights[i]->zNear, m_lights[i]->zFar);

	    glm::mat4 lightView = glm::lookAt(m_lights[i]->direction, glm::vec3(0.0f, 0.0f, 0.0f), up);


		m_lights[i]->view = lightView;
		m_lights[i]->proj = lightProjection;
		m_lights[i]->shadowMap = getDepthTexture(i);

	    render(lightView, lightProjection);

		m_framebuffers[i]->Unbind();
	}

	glDisable(GL_POLYGON_OFFSET_FILL);

}

unsigned int ShadowMapPass::getDepthTexture(int index) const
{
	 return m_framebuffers[index]->GetBuffer("depthBuffer"); 
}

void ShadowMapPass::render(glm::mat4 view, glm::mat4 proj) const
{

	m_shader->Bind();

	m_shader->setMat4("lightSpaceMatrix", proj * view);

	for (auto model : m_models)
	{
		model->Draw(m_shader);
	}

}

void ShadowMapPass::addModel(const std::shared_ptr<Model> model)
{
	m_models.push_back(model);

}

void ShadowMapPass::addModel(const std::vector<std::shared_ptr<Model>> models)
{
	m_models.insert(m_models.end(), models.begin(), models.end());
}

void ShadowMapPass::addLight(const shared_ptr<DirectionalLight> &light)
{
	m_lights.push_back(light);
}

void ShadowMapPass::addLight(const std::vector<std::shared_ptr<DirectionalLight>>& lights)
{
	m_lights.insert(m_lights.end(), lights.begin(), lights.end());
}
