#pragma once
#include"FrameBuffer.h"
#include"shader.h"
#include <memory>

class HDR
{
public:
	HDR();
	~HDR();

	void initialize(unsigned int width,unsigned int height);
	
	void uninitialize();

	void process(std::shared_ptr<FrameBuffer> sceneRT);



private:
	std::shared_ptr<FrameBuffer>    m_halfSizeRT;
	std::shared_ptr<FrameBuffer>    m_quarterSizeRT;
	std::shared_ptr<FrameBuffer>    m_64x64;
	std::shared_ptr<FrameBuffer>    m_16x16;
	std::shared_ptr<FrameBuffer>    m_4x4;
	std::shared_ptr<FrameBuffer>    m_1x1;
	std::shared_ptr<FrameBuffer>    m_adapter[2];
	std::shared_ptr<FrameBuffer>    m_bloom0[2];
	std::shared_ptr<FrameBuffer>    m_bloom1[2];
	std::shared_ptr<FrameBuffer>    m_bloom2[2];

	std::shared_ptr<Shader>  m_halfDownsampleShader;
	std::shared_ptr<Shader>  m_sampleInitialShader;
	std::shared_ptr<Shader>  m_sampleIterativeShader;
	std::shared_ptr<Shader>  m_adapteShader;
	std::shared_ptr<Shader>  m_brightShader;
	std::shared_ptr<Shader>  m_blurShader;
	std::shared_ptr<Shader>  m_toneMapingShader;

	unsigned int VAO;

	void renderFullSreenquad();
	
	void sampleLumIterative(std::shared_ptr<FrameBuffer> destRT, std::shared_ptr<FrameBuffer>srcRT);
	
	void halfDownsample(std::shared_ptr<FrameBuffer> destRT, std::shared_ptr<FrameBuffer> srcRT);
	
	void measureLuminance();

	void bright();

	void blur(std::shared_ptr<FrameBuffer> dest, std::shared_ptr<FrameBuffer>src);

	void eyeAdaptation();

	void bloomAndFlares();

	void toneMapping(std::shared_ptr<FrameBuffer> sceneRT);


	const char* colorBuffer = "colorBuffer";


	float		 eyeAdaptationParam[4] = { 0.149175361f, 0.690104187f, 0.500000000f, 0.0500000007f };
	float		 brightParam[4] = { 2.00000000f, 3.00000000f, 1.00000000f,0.00100000005f };
	float		 toneMappingParam[4] = { 0.690453827f, 0.400000006f, 1.00000000f, 0.14f };
	float		 toneMappingCurve[4] = { 2.51f,0.03f,2.43f,0.59f };
	float		 colorBalance[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	unsigned int adapterSourceIndex = 0;
	unsigned int adapterTargetIndex = 1;

	bool m_initialized;
};

