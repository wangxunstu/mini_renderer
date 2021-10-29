#include "FrameBuffer.h"
#include<assert.h>
#include <iostream>

FrameBuffer::FrameBuffer(){
	glGenFramebuffers(1, &m_frameBuffer);
	glGenRenderbuffers(1, &m_renderBuffer);
}
void FrameBuffer::AttachColorBuffer(const char*bufferName, GLenum attachment,
	int width, int height,GLenum samplerType, GLenum internalformat, GLenum format,GLenum type)
{
	m_width = width;
	m_height = height;

	GLuint colorBuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, colorBuffer, 0);
	m_drawBuffers.push_back(attachment);
	m_buffers.insert(std::pair<std::string,GLuint>(bufferName,colorBuffer));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}
void FrameBuffer::AttachDepthBuffer(const char*bufferName, int width, int height,bool hwShadow)
{
	GLuint depthMap;
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if (hwShadow)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	

	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	m_buffers.insert(std::pair<std::string, GLuint>(bufferName, depthMap));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::AttachDepthBufferWithMSAA(const char* bufferName, int width, int height, bool hwShadow)
{


}
void FrameBuffer::Finish(){
	int nCount = (int)m_drawBuffers.size();
	if (nCount>0){
		GLenum *buffers = new GLenum[nCount];
		int i = 0;
		while (i<nCount){
			buffers[i] = m_drawBuffers[i];
            i++;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
		glDrawBuffers(nCount, buffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
void FrameBuffer::BindTargetTex(GLuint texture, GLuint targetTex, GLuint level)
{
	//glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);
	//glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,targetTex, texture , level);
//	glBindFramebuffer(GL_FRAMEBUFFER, m_prevFrameBuffer);

}
void FrameBuffer::resizeBufferStorage(unsigned int width, unsigned int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	m_width = width;
	m_height = height;

//	glBindFramebuffer(GL_FRAMEBUFFER, m_prevFrameBuffer);

}
void FrameBuffer::Bind(){
	
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_prevFrameBuffer);
	
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_width, m_height);


}
void FrameBuffer::Unbind(){
	glBindFramebuffer(GL_FRAMEBUFFER, m_prevFrameBuffer);
}
GLuint FrameBuffer::GetBuffer(const char*bufferName){
	auto iter = m_buffers.find(bufferName);
	if (iter!=m_buffers.end()){
		return iter->second;
	}
	else {
		assert(false);
	}
	return 0;
}
