#include "HDR.h"
#include<vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

HDR::HDR():m_initialized(false)
{
}

HDR::~HDR()
{

}


void HDR::initialize(unsigned int width, unsigned int height)
{

	m_halfSizeRT = std::make_shared<FrameBuffer>();
	m_halfSizeRT->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0,  width >> 1, height >> 1,GL_POINT);
	m_halfSizeRT->Finish();

	m_quarterSizeRT = std::make_shared<FrameBuffer>();
	m_quarterSizeRT->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, width >> 2, height >> 2, GL_POINT);
	m_quarterSizeRT->Finish();

	m_64x64 = std::make_shared<FrameBuffer>();
	m_64x64->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, 64,64, GL_POINT);
	m_64x64->Finish();

	m_16x16 = std::make_shared<FrameBuffer>();
	m_16x16->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, 16, 16, GL_POINT);
	m_16x16->Finish();

	m_4x4 = std::make_shared<FrameBuffer>();
	m_4x4->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, 64, 64, GL_POINT);
	m_4x4->Finish();

	m_1x1 = std::make_shared<FrameBuffer>();
	m_1x1->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, 1, 1);
	m_1x1->Finish();

	m_adapter[0] = std::make_shared<FrameBuffer>();
	m_adapter[0]->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, 1, 1);
	m_adapter[0]->Finish();

	m_adapter[1] = std::make_shared<FrameBuffer>();
	m_adapter[1]->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, 1, 1);
	m_adapter[1]->Finish();
	//bloom0
	m_bloom0[0] = std::make_shared<FrameBuffer>();
	m_bloom0[0]->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, width >> 2, height >> 2, GL_POINT);
	m_bloom0[0]->Finish();

	m_bloom0[1] = std::make_shared<FrameBuffer>();
	m_bloom0[1]->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, width >> 2, height >> 2, GL_POINT);
	m_bloom0[1]->Finish();
	//bloom1
	m_bloom1[0] = std::make_shared<FrameBuffer>();
	m_bloom1[0]->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, width >> 3, height >> 3, GL_POINT);
	m_bloom1[0]->Finish();

	m_bloom1[1] = std::make_shared<FrameBuffer>();
	m_bloom1[1]->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, width >> 3, height >> 3, GL_POINT);
	m_bloom1[1]->Finish();
	//bloom2
	m_bloom2[0] = std::make_shared<FrameBuffer>();
	m_bloom2[0]->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, width >> 4, height >> 4, GL_POINT);
	m_bloom2[0]->Finish();

	m_bloom2[1] = std::make_shared<FrameBuffer>();
	m_bloom2[1]->AttachColorBuffer(colorBuffer, GL_COLOR_ATTACHMENT0, width >> 4, height >> 4, GL_POINT);
	m_bloom2[1]->Finish();


	m_halfDownsampleShader  = std::make_shared<Shader>("shader/postProcessStandard.vs", "shader/halfDownsample.fs");
	m_sampleInitialShader   = std::make_shared<Shader>("shader/postProcessStandard.vs", "shader/sampleLumInitial.fs");
	m_sampleIterativeShader = std::make_shared<Shader>("shader/postProcessStandard.vs", "shader/sampleLumIterative.fs");
	m_adapteShader          = std::make_shared<Shader>("shader/postProcessStandard.vs", "shader/adapte.fs");
	m_brightShader          = std::make_shared<Shader>("shader/postProcessStandard.vs", "shader/bright.fs");
	m_blurShader            = std::make_shared<Shader>("shader/postProcessStandard.vs", "shader/blur.fs");
	m_toneMapingShader      = std::make_shared<Shader>("shader/postProcessStandard.vs", "shader/toneMapping.fs");
   
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	unsigned int VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));	
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);


	m_initialized = true;

}


void HDR:: halfDownsample(std::shared_ptr<FrameBuffer> destRT, std::shared_ptr<FrameBuffer> sourceRT)
{

	destRT->Bind();

	float s1 = 1.0f / sourceRT->GetWidth();
	float t1 = 1.0f / sourceRT->GetHeight();

	glm::vec4 offset1 = glm::vec4(-s1, -t1, s1, -t1);
	glm::vec4 offset2 = glm::vec4(s1, t1, -s1, t1);

	m_halfDownsampleShader->Bind();

	m_halfDownsampleShader->setVec4("offset1", offset1);
	m_halfDownsampleShader->setVec4("offset2", offset2);


	m_halfDownsampleShader->bindTexture2D(sourceRT->GetBuffer(colorBuffer), std::string("tex"), 0);

	//glBindTexture(GL_TEXTURE_2D, sourceRT->GetBuffer(colorBuffer));
	
	renderFullSreenquad();

	m_halfDownsampleShader->unBind();
	
	destRT->Unbind();
}

void HDR::sampleLumIterative(std::shared_ptr<FrameBuffer> destRT, std::shared_ptr<FrameBuffer> srcRT)
{
	destRT->Bind();

	float s1 = 1.0f / srcRT->GetWidth();
	float t1 = 1.0f / srcRT->GetHeight();

	glm::vec4 offset1 = glm::vec4(-s1, -t1, s1, -t1);
	glm::vec4 offset2 = glm::vec4(s1, t1, -s1, t1);

	m_sampleIterativeShader->Bind();
	m_sampleIterativeShader->setVec4("offset1", offset1);
	m_sampleIterativeShader->setVec4("offset2", offset2);
	glBindTexture(GL_TEXTURE_2D, srcRT->GetBuffer(colorBuffer));
	renderFullSreenquad();
	m_sampleIterativeShader->unBind();

	destRT->Unbind();

}

void HDR:: measureLuminance()
{
	m_64x64->Bind();

	float s1 = 1.0f / m_quarterSizeRT->GetWidth();
	float t1 = 1.0f / m_quarterSizeRT->GetHeight();

	glm::vec4 offset1 = glm::vec4(-s1, -t1, s1, -t1);
	glm::vec4 offset2 = glm::vec4(s1, t1, -s1, t1);

	m_sampleInitialShader->Bind();
	m_sampleInitialShader->setVec4("offset1", offset1);
	m_sampleInitialShader->setVec4("offset2", offset2);

	glBindTexture(GL_TEXTURE_2D, m_quarterSizeRT->GetBuffer(colorBuffer));
	renderFullSreenquad();
	m_sampleInitialShader->unBind();

	m_64x64->Unbind();


	sampleLumIterative(m_16x16, m_64x64);
	
	sampleLumIterative(m_4x4, m_16x16);
	
	sampleLumIterative(m_1x1, m_4x4);

	m_64x64->Unbind();
}

void HDR::bright()
{
	m_bloom0[0]->Bind();

	m_brightShader->Bind();

	m_brightShader->setVec4("brightParams", glm::vec4(brightParam[0], brightParam[1], brightParam[2], brightParam[3]));

	m_brightShader->setInt("brightTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_quarterSizeRT->GetBuffer(colorBuffer));

	renderFullSreenquad();

	m_brightShader->unBind();

	m_bloom0[0]->Unbind();

}

void HDR::blur(std::shared_ptr<FrameBuffer> dest, std::shared_ptr<FrameBuffer> src)
{

	float scaleH = 1.0f / src->GetHeight();
	float scaleW = scaleH * (float)src->GetHeight() / (float)src->GetWidth();

	dest->Bind();
	//blur horizontal
	m_blurShader->Bind();
	m_blurShader->setVec4("blurParams",glm::vec4(scaleW,0.0f,0.0f,0.0f));
	glBindTexture(GL_TEXTURE_2D, src->GetBuffer(colorBuffer));
	renderFullSreenquad();
	//blur vertical
	m_blurShader->setVec4("blurParams", glm::vec4(0.0f, scaleH, 0.0f, 0.0f));
	renderFullSreenquad();

	dest->Unbind();

}

void HDR::eyeAdaptation()
{
	m_adapter[adapterTargetIndex]->Bind();

	m_adapteShader->Bind();

	m_adapteShader->setVec4("adapteParams", eyeAdaptationParam);

	m_adapteShader->setInt("tex0", 0);
	m_adapteShader->setInt("tex1", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_1x1->GetBuffer(colorBuffer));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_adapter[adapterSourceIndex]->GetBuffer(colorBuffer));


	renderFullSreenquad();

	m_adapteShader->unBind();

	m_adapter[adapterTargetIndex]->Unbind();

}

void HDR::bloomAndFlares()
{
	bright();
	blur(m_bloom0[1], m_bloom0[0]);
	
	halfDownsample(m_bloom1[0], m_bloom0[0]);
	blur(m_bloom1[1], m_bloom1[0]);

	halfDownsample(m_bloom2[0], m_bloom1[0]);
	blur(m_bloom2[1], m_bloom2[0]);
}	

void HDR::toneMapping(std::shared_ptr<FrameBuffer> sceneRT)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, 1280, 720);

	m_toneMapingShader->Bind();

	m_toneMapingShader->setVec4("toneMappingParam", toneMappingParam);
	m_toneMapingShader->setVec4("toneMappingCurve", toneMappingCurve);
	m_toneMapingShader->setVec4("colorBalance", colorBalance);

	m_toneMapingShader->setInt("sceneTex",0);
	m_toneMapingShader->setInt("lumTex", 1);
	m_toneMapingShader->setInt("bloom0", 2);
	m_toneMapingShader->setInt("bloom1", 3);
	m_toneMapingShader->setInt("bloom2", 4);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneRT->GetBuffer(colorBuffer));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_adapter[adapterTargetIndex]->GetBuffer(colorBuffer));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_bloom0[0]->GetBuffer(colorBuffer));
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_bloom1[0]->GetBuffer(colorBuffer));
	
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_bloom2[0]->GetBuffer(colorBuffer));

	renderFullSreenquad();

	m_toneMapingShader->unBind();

}

void HDR::uninitialize()
{


}

void HDR::process(std::shared_ptr<FrameBuffer> sceneRT)
{
	if (!m_initialized)
		initialize(sceneRT->GetWidth(), sceneRT->GetHeight());


	halfDownsample(m_halfSizeRT, sceneRT);
	
	halfDownsample(m_quarterSizeRT, m_halfSizeRT);	

	measureLuminance();

	eyeAdaptation();

	bloomAndFlares();
	
	toneMapping(sceneRT);
	
	adapterTargetIndex = adapterTargetIndex + adapterSourceIndex;
	adapterSourceIndex = adapterTargetIndex - adapterSourceIndex;
	adapterTargetIndex = adapterTargetIndex - adapterSourceIndex;

}

void HDR::renderFullSreenquad()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}




