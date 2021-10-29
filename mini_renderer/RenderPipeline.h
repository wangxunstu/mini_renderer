#pragma once
#include <unordered_map>
#include "RenderPass.h"
#include <memory>
#include <typeindex>
#include "camera.h"

//using RenderPasses = std::vector<std::shared_ptr<RenderPass>>;

class RenderPipeline
{
public:
	explicit RenderPipeline(std::shared_ptr<Camera> camera);

	void addRenderPass(std::shared_ptr<RenderPass> renderPass);

	std::shared_ptr<RenderPass> getRenderPass(const std::string name, size_t idx = 0);

	void update();

	std::shared_ptr<Camera>getCamera() { return m_camera; }

private:

	std::shared_ptr<Camera> m_camera;

	std::vector<std::shared_ptr<RenderPass>> m_renderPasses;
	//std::unordered_map<std::string, RenderPasses> m_renderPassesMap;
	//std::unordered_map<std::type_index, RenderPasses> m_renderPassesByTypeIdx;
};

