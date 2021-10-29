#include "RenderPipeline.h"
#include <iostream>

RenderPipeline::RenderPipeline(std::shared_ptr<Camera> camera)
{
}

void RenderPipeline::addRenderPass(std::shared_ptr<RenderPass> renderPass)
{
    m_renderPasses.push_back(renderPass);

	//auto it = m_renderPassesMap.find(renderPass->m_name);
	//if (it != m_renderPassesMap.end())
	//{
	//	std::cout<<"RenderPipeline - Error: Trying to add a second RenderPass with the same name: " << renderPass->m_name<<std::endl;
	//	return;
	//}
	//
	//renderPass->m_renderPipeline = this;
	//m_renderPassesMap[renderPass->m_name].push_back(renderPass);
	//m_renderPassesByTypeIdx[std::type_index(typeid(renderPass))].push_back(renderPass);

}

//std::shared_ptr<RenderPass> RenderPipeline::getRenderPass(const std::string name, size_t idx)
//{
//	auto it = m_renderPassesMap.find(name);
//	if (it == m_renderPassesMap.end())
//	{
//		std::cout << "RenderPipeline - Error: The requested RenderPass " << name << " does not exist." << std::endl;
//		return nullptr;
//	}
//
//	auto& renderPasses = it->second;
//	assert(renderPasses.size() > 0);
//
//	if (renderPasses.size() <= idx)
//	{
//		std::cout<<"RenderPipeline - Error: The requested Index " << idx << " of RenderPass " << name << " does not exist."<<std::endl;
//		return nullptr;
//	}
//
//	assert(renderPasses[idx]->m_name == name);
//
//	return renderPasses[idx];
//}

void RenderPipeline::update()
{
	for (auto& renderPass : m_renderPasses)
	{
		if (!renderPass->isEnabled())
			continue;

		renderPass->m_renderPipeline = this;
		renderPass->update();
	}
}
