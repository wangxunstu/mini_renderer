#include "Graphics.h"
#include "camera.h"
#include "model.h"
#include "shader.h"
#include "DirectionalLight.h"
#include "FrameBuffer.h"
#include "Util.h"
#include "Material.h"
#include "HDR.h"


const std::string albedoMapUniformName = std::string("albedoMap");
const std::string normaMapUniformName = std::string("normalMap");
const std::string metallicMapUniformName = std::string("metallicMap");
const std::string roughnessMapUniformName = std::string("roughnessMap");
const std::string aoUniformName = std::string("aoMap");
const std::string hdrFilePath = string("resources/textures/hdr/newport_loft.hdr");

unsigned int albeddoMapSlot = 0;
unsigned int normalMapSlot = 1;
unsigned int metallicMapSlot = 2;
unsigned int roughnesspSlot = 3;
unsigned int aoMapSlot = 4;


Graphics::Graphics(const RenderPath renderPath,const unsigned int width, const unsigned int height): RenderPass("ForwardScenePass"),m_renderPath(renderPath),m_width(width),m_height(height),m_initialized(false)
{

	m_pbrShader = std::make_shared<Shader>("shader/pbr.vs", "shader/pbr.fs");

	m_equirectangularToCubemapShader = make_shared<Shader>("shader/cubemap.vs", "shader/equirectangular_to_cubemap.fs");

	m_irradianceShader = make_shared<Shader>("shader/cubemap.vs", "shader/irradiance_convolution.fs");

	m_prefilterShader = make_shared<Shader>("shader/cubemap.vs", "shader/prefilter.fs");

	m_brdfShader      = make_shared<Shader>("shader/brdf.vs", "shader/brdf.fs");
					  					  
	m_gBufferShader   = std::make_shared<Shader>("shader/gBuffer.vs", "shader/gBuffer_pbr.fs");
					  
	m_deferMaterial   = std::make_shared<Material>(std::make_shared<Shader>("shader/deferred.vs", "shader/deferred.fs"));
					  
	m_pbrMaterial     = std::make_shared<Material>(std::make_shared<Shader>("shader/pbr.vs", "shader/pbr.fs"));
					  
	m_blinnMaterial   = std::make_shared<Material>(std::make_shared<Shader>("shader/forward.vs", "shader/forward.fs"));

	m_hdr = std::make_shared<HDR>();

	m_hdrRT = std::make_shared<FrameBuffer>();
}

void Graphics::renderGBuffer()
{
	m_gBufferShader->Bind();
	m_gBuffer->Bind();

	m_gBufferShader->setMat4("view", m_camera->GetViewMatrix());
	m_gBufferShader->setMat4("projection", m_camera->GetProjectionMatrix());

	for (auto model : m_models)
	{
		glActiveTexture(GL_TEXTURE0 + aoMapSlot);
		glBindTexture(GL_TEXTURE_2D, m_aoTexture);

		glActiveTexture(GL_TEXTURE0 + albeddoMapSlot);
		glBindTexture(GL_TEXTURE_2D, m_albedoTexture);

		glActiveTexture(GL_TEXTURE0 + normalMapSlot);
		glBindTexture(GL_TEXTURE_2D, m_normalTexture);

		glActiveTexture(GL_TEXTURE0 + roughnesspSlot);
		glBindTexture(GL_TEXTURE_2D, m_rougnessTexture);

		glActiveTexture(GL_TEXTURE0 + metallicMapSlot);
		glBindTexture(GL_TEXTURE_2D, m_metalicTexture);

		model->Draw(m_gBufferShader);

	}

	m_gBuffer->Unbind();
	m_gBufferShader->unBind();
}

void Graphics::update()
{
	if (!m_initialized)
		init();


	if (m_renderPath == RenderPath::RENDER_DEFER)
		renderGBuffer();

	setUniforms();




	
	if (m_enable_hdr_post)
	{
		m_hdrRT->Bind();
	}

	render();

	if (m_enable_hdr_post)
	{
		Util::bindToDefaultRT();


		m_hdr->process(m_hdrRT);
	}


}

void Graphics::setCamera(const std::shared_ptr<Camera> camera)
{
	m_camera = camera;
}

void Graphics::addModel(const std::shared_ptr<Model> model)
{
	m_models.push_back(model);

}

void Graphics::addModel(const std::vector<std::shared_ptr<Model>> models)
{
	m_models.insert(m_models.end(), models.begin(), models.end());
}

void Graphics::addLight(const shared_ptr<DirectionalLight>& light)
{
	m_lights.push_back(light);
}

void Graphics::addLight(const std::vector<std::shared_ptr<DirectionalLight>>& lights)
{
	m_lights.insert(m_lights.end(), lights.begin(), lights.end());
}

void Graphics::enableHdrPost(const bool enable_hdr_post)
{
	m_enable_hdr_post = enable_hdr_post;

	m_hdr->initialize(m_width, m_height);
}

void Graphics::render() const
{
	if (m_renderPath==RenderPath::RENDER_FORWARD)
	{
		for (auto& model : m_models)
		{
			if (model->getShadingID() == ShadingType::BLIN_PHONG)
			{
				m_blinnMaterial->use();
				model->Draw(m_blinnMaterial->getShader());
			}
			else if (model->getShadingID() == ShadingType::PBR)
			{
				m_pbrMaterial->use();
				model->Draw(m_pbrMaterial->getShader());
			}
		}
	}
	else
	{
		m_deferMaterial->use();
		Util::renderQuad();
	}
}


void Graphics::setCommonUniforms(std::shared_ptr<Material>material)const
{
	material->setMatrix("view", m_camera->GetViewMatrix());
	material->setMatrix("projection", m_camera->GetProjectionMatrix());

	material->setInt("u_enable_hdr_post", (unsigned int)m_enable_hdr_post);
	material->setInt("u_shininess", 16);


	for (unsigned int i = 0; i < m_lights.size(); i++)
	{
		std::string arrayIndex = "[" + std::to_string(i) + "]";
		std::string directionLightUniformName = "u_directionLights" + arrayIndex;

		material->setVector(directionLightUniformName + ".direction", m_lights[i]->direction);
		material->setVector(directionLightUniformName + ".color", m_lights[i]->color);
		material->setFloat(directionLightUniformName + ".intensity", m_lights[i]->intensity);
		material->setInt(directionLightUniformName + ".shadowEnabled", m_lights[i]->shadowsEnabled);


		if (m_lights[i]->shadowsEnabled)
		{
			material->setMatrix("lightSpaceMatrix", m_lights[i]->proj * m_lights[i]->view);

			material->setTexture2D("u_shadowMap", m_lights[i]->shadowMap);

		}

		material->setVector("cameraPos", m_camera->Position);
	}

}
void Graphics::setUniforms()const
{

	if (m_renderPath == RenderPath::RENDER_FORWARD)
	{

		m_pbrMaterial->setTextureCubeMap("irradianceMap", m_irradianceMap);
		m_pbrMaterial->setTextureCubeMap("prefilterMap",  m_prefilterMap);
		m_pbrMaterial->setTexture2D("brdfLUT",            m_brdfLutTexture);
		m_pbrMaterial->setTexture2D("aoMap",              m_aoTexture);
		m_pbrMaterial->setTexture2D("normalMap",          m_normalTexture);
		m_pbrMaterial->setTexture2D("metallicMap",        m_metalicTexture);
		m_pbrMaterial->setTexture2D("roughnessMap",       m_rougnessTexture);
		m_pbrMaterial->setTexture2D("albedoMap",          m_albedoTexture);

		m_blinnMaterial->setVector("u_diffuseColor", glm::vec3(1.f));
		m_blinnMaterial->setVector("u_specularColor", glm::vec3(1.f,1.f,1.f));
	
		setCommonUniforms(m_blinnMaterial);
		
		setCommonUniforms(m_pbrMaterial);
		
	}
	else
	{
		setCommonUniforms(m_deferMaterial);

		m_deferMaterial->setTextureCubeMap("irradianceMap", m_irradianceMap);
		m_deferMaterial->setTextureCubeMap("prefilterMap", m_prefilterMap);
		m_deferMaterial->setTexture2D("brdfLUT", m_brdfLutTexture);
		m_deferMaterial->setTexture2D("gBuffer0", m_gBuffer->GetBuffer("gBuffer0"));
		m_deferMaterial->setTexture2D("gBuffer1", m_gBuffer->GetBuffer("gBuffer1"));
		m_deferMaterial->setTexture2D("gBuffer2", m_gBuffer->GetBuffer("gBuffer2"));
		m_deferMaterial->setTexture2D("gBuffer3", m_gBuffer->GetBuffer("gBuffer3"));
		
	}
	
}

void Graphics::init() 
{

	if (m_initialized) return;

	m_aoTexture             = Util::TextureFromFile("ao.png", std::string("resources/textures/pbr/cerberus"), false);
					        
	m_normalTexture         = Util::TextureFromFile("normal.png", std::string("resources/textures/pbr/cerberus"), false);
					        
	m_rougnessTexture       = Util::TextureFromFile("roughness.png", std::string("resources/textures/pbr/cerberus"), false);
					        
	m_metalicTexture        = Util::TextureFromFile("metallic.png", std::string("resources/textures/pbr/cerberus"), false);
					        
	m_albedoTexture         = Util::TextureFromFile("albedo.png", std::string("resources/textures/pbr/cerberus"), false);


	m_brdfLutTexture        = Util::createTexture2D(512, GL_RG16F, GL_RG, GL_FLOAT);

	m_irradianceMap         = Util::createCubeMap(32, GL_RGBA16F, GL_RGBA, GL_FLOAT, false);

	m_prefilterMap          = Util::createCubeMap(128, GL_RGBA16F, GL_RGB, GL_FLOAT, true, GL_LINEAR_MIPMAP_LINEAR);

	unsigned int hdrTexture = Util::loadHdrTexture(hdrFilePath);

	unsigned int envCubemap = Util::createCubeMap(512, GL_RGB16F, GL_RGB, GL_FLOAT, true, GL_LINEAR_MIPMAP_LINEAR);


	std::shared_ptr<FrameBuffer> FBO = make_shared<FrameBuffer>();
	FBO->AttachColorBuffer("colorBuffer", GL_COLOR_ATTACHMENT0, 512, 512);
	FBO->Finish();

	glm::mat4 projections = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 views[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(0.0f, -1.0f,  0.0f))
	};


	FBO->Bind();
	//convert hdr texture to cube map
	m_equirectangularToCubemapShader->Bind();
	m_equirectangularToCubemapShader->setInt("equirectangularMap", 0);
	m_equirectangularToCubemapShader->setMat4("projection", projections);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	for (unsigned int i = 0; i < 6; ++i)
	{
		m_equirectangularToCubemapShader->setMat4("view", views[i]);

		FBO->BindTargetTex(envCubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Util::renderCube();
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


	FBO->Bind();
	FBO->resizeBufferStorage(32, 32);
	// get irrandiance map in low resolution
	m_irradianceShader->Bind();
	m_irradianceShader->setInt("environmentMap", 0);
	m_irradianceShader->setMat4("projection", projections);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	for (unsigned int i = 0; i < 6; ++i)
	{
		m_irradianceShader->setMat4("view", views[i]);

		FBO->BindTargetTex(m_irradianceMap, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0);

		Util::renderCube();
	}


	FBO->Bind();
	// get prefilter map for specular calculation
	m_prefilterShader->Bind();
	m_prefilterShader->setInt("environmentMap", 0);
	m_prefilterShader->setMat4("projection", projections);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);

		FBO->resizeBufferStorage(mipWidth, mipHeight);


		float roughness = (float)mip / (float)(maxMipLevels - 1);
		m_prefilterShader->setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			m_prefilterShader->setMat4("view", views[i]);

			FBO->BindTargetTex(m_prefilterMap, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);

			Util::renderCube();
		}
	}

	FBO->Unbind();

	/// get BRDF LUT texture
	FBO->resizeBufferStorage(512, 512);
	FBO->BindTargetTex(m_brdfLutTexture, GL_TEXTURE_2D, 0);

	FBO->Bind();

	m_brdfShader->Bind();
	Util::renderQuad();

	FBO->Unbind();


	m_gBuffer = std::make_shared<FrameBuffer>();
	m_gBuffer->AttachColorBuffer("gBuffer0", GL_COLOR_ATTACHMENT0, m_width, m_height);   
	m_gBuffer->AttachColorBuffer("gBuffer1", GL_COLOR_ATTACHMENT1, m_width, m_height);
	m_gBuffer->AttachColorBuffer("gBuffer2", GL_COLOR_ATTACHMENT2, m_width, m_height);
	m_gBuffer->AttachColorBuffer("gBuffer3", GL_COLOR_ATTACHMENT3, m_width, m_height);
	m_gBuffer->Finish();

	m_gBufferShader->Bind();
	m_gBufferShader->setInt(aoUniformName,           aoMapSlot);
	m_gBufferShader->setInt(albedoMapUniformName,    albeddoMapSlot);
	m_gBufferShader->setInt(normaMapUniformName,     normalMapSlot);
	m_gBufferShader->setInt(roughnessMapUniformName, roughnesspSlot);
	m_gBufferShader->setInt(metallicMapUniformName,  metallicMapSlot);
	m_gBufferShader->unBind();

	m_hdrRT->AttachColorBuffer("colorBuffer", GL_COLOR_ATTACHMENT0, m_width, m_height);
	m_hdrRT->Finish();


	if (m_enable_hdr_post)
	{
		m_hdr->initialize(m_width, m_height);
	}


	m_initialized = true;
}
