#pragma once
#include <glad/glad.h>
#include <string>
#include <memory>
#include "FrameBuffer.h"
#include <string>
#include <glm/glm.hpp>

#include "pre_mesh.h"

class Util
{
public:
	static void renderCube();

	static void renderQuad(bool clear=true);

	static void copyRenderTarget(std::shared_ptr<FrameBuffer>srcRT, std::shared_ptr<FrameBuffer>dstRT);

	static unsigned int createCubeMap(unsigned int resolution, GLenum internalFormat, GLenum format, GLenum type, bool genMips,GLenum minFilter=GL_LINEAR);
	
	static unsigned int createTexture2D(unsigned int resolution, GLenum internalFormat, GLenum format, GLenum type);

	static unsigned int loadHdrTexture(const std::string& path);

	static unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);

	static void renderSphere();
	
	static void debug(unsigned int tex);

	static void bindToDefaultRT();

	static glm::vec3 getRandomNumberVec3();

	static unsigned int getMeshVAO(const std::vector<SHVertex>& vertex);

};

