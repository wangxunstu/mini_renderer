#pragma once
#include <memory>
#include "RenderPass.h"
#include "Material.h"

class Shader;
class Camera;
class Model;
class DirectionalLight;
class FrameBuffer;
class Material;
class HDR;

class Graphics:public RenderPass
{
public:
	 Graphics(const RenderPath renderPath,const unsigned int width,const unsigned int height);

	void update() override;

	void setCamera(const std::shared_ptr<Camera> camera);

	void addModel(const std::shared_ptr<Model> model);
	
	void addModel(const std::vector <std::shared_ptr<Model>>models);

	void addLight(const std::shared_ptr<DirectionalLight>& light);
	
	void addLight(const std::vector<std::shared_ptr<DirectionalLight>>& lights);

	void enableHdrPost(const bool enable_hdr_post);

	std::shared_ptr<Material> getBlinnMaterial() { return m_blinnMaterial; }



private:

	void render() const;

	void setUniforms() const;

	void init() ;

private:

	void renderGBuffer();

	void setCommonUniforms(std::shared_ptr<Material>material)const;

	std::shared_ptr<Camera> m_camera;

	std::vector<std::shared_ptr<Model>> m_models;

    std::vector<std::shared_ptr<DirectionalLight>> m_lights;

	unsigned int m_prefilterMap;

	unsigned int m_irradianceMap;
	
	unsigned int m_brdfLutTexture;

	unsigned int m_aoTexture;
	
	unsigned int m_normalTexture;
	
	unsigned int m_rougnessTexture;
	
	unsigned int m_metalicTexture;
	
	unsigned int m_albedoTexture;

	std::shared_ptr<Shader> m_pbrShader;
	
	std::shared_ptr<Shader> m_equirectangularToCubemapShader;
	
	std::shared_ptr<Shader> m_irradianceShader;
	
	std::shared_ptr<Shader> m_prefilterShader;
	
	std::shared_ptr<Shader> m_brdfShader;

	std::shared_ptr<FrameBuffer>  m_gBuffer;

	std::shared_ptr<Shader>       m_gBufferShader;

	std::shared_ptr<Shader>   m_deferShading;

	std::shared_ptr<Material> m_deferMaterial;

	std::shared_ptr<Material> m_pbrMaterial;

	std::shared_ptr<Material> m_blinnMaterial;

	std::shared_ptr<FrameBuffer> m_hdrRT;

	std::shared_ptr<HDR> m_hdr;
	
	bool m_enable_hdr_post;

	bool m_initialized;

	const unsigned int m_width;

	const unsigned int m_height;

	const RenderPath m_renderPath;

};

