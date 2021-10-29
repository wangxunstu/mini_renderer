#pragma once
#include <glad/glad.h>
#include<map>
#include<vector>
#include<string>

class FrameBuffer
{
private:
	
	GLuint m_renderBuffer;
	GLint m_prevFrameBuffer;
	std::map<std::string, GLuint> m_buffers;
	std::vector<GLenum> m_drawBuffers;
	
	int m_width;
	int m_height;
	  
public:

	GLuint m_frameBuffer;


	FrameBuffer();
	void AttachColorBuffer(const char*bufferName,GLenum attachment,int width,int height,
		GLenum samplerType = GL_LINEAR, GLenum internalformat=GL_RGBA32F, GLenum format=GL_RGBA,GLenum type=GL_FLOAT);
	void AttachDepthBuffer(const char* bufferName, int width, int height, bool hwShadow = false);
	void AttachDepthBufferWithMSAA(const char* bufferName, int width, int height, bool hwShadow = false);
	void Finish();
	void BindTargetTex(GLuint texture,GLuint targetTex = GL_TEXTURE_2D, GLuint level = 0);
	void resizeBufferStorage(unsigned int width, unsigned int height);
	void Bind();
	void Unbind();

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

	GLuint GetBuffer(const char*bufferName);
};