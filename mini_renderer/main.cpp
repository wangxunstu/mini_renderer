#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include "Util.h"
#include"shader.h"
#include"model.h"
#include"camera.h"
#include"FrameBuffer.h"
#include"HDR.h"
#include "Graphics.h"
#include "DirectionalLight.h"
#include "RenderPipeline.h"
#include "ShadowMapPass.h"
#include "Material.h"

#include "pre_mesh.h"
#include "Util.h"


#define GLM_ENABLE_EXPERIMENTAL

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

const unsigned int shadowResolution =  1024;

shared_ptr<Camera>camera = make_shared<Camera>((float)SCR_WIDTH / SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = 1280.0 / 2.0;
float lastY = 720.0 / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool eable_hdr_post = false;

glm::vec3 lightPos(-2.0f, 2.0f, 2.0f);


static constexpr int MAX_SH_ORDER = 4;
static constexpr int FULL_SH_COUNT = (MAX_SH_ORDER + 1) * (MAX_SH_ORDER + 1);

static constexpr int  SAMPLES_PER_VERTEX= 16;
static constexpr int SAMPLES_FOR_LIGHT = 1000000;

static constexpr float VERTEX_ALBEDO = 0.8f;

LightingMode lightingMode_ = LightingMode::Shadow;


unsigned int tboID, tboTextureID;




void shCofficientTexture(const std::vector<float>& cof)
{
	size_t buffer_size = sizeof(float) * cof.size();
	int limit;
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &limit);
	if (buffer_size > limit)
	{
		assert(false);
	}


    glGenBuffers(1, &tboID);
	glBindBuffer(GL_TEXTURE_BUFFER, tboID);
	glBufferData(GL_TEXTURE_BUFFER, buffer_size, cof.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);


    glGenTextures(1, &tboTextureID);

    glBindTexture(GL_TEXTURE_BUFFER, tboTextureID);

    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tboID);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

}

std::vector<float> generateMeshCoefs(const SHVertex* vertices, size_t vertexCount, const std::string& cacheFileName)
{
    
    auto result = computeVertexSHCoefs(vertices, static_cast<int>(vertexCount), VERTEX_ALBEDO, MAX_SH_ORDER, SAMPLES_PER_VERTEX, lightingMode_);


    return result;
}


void shPreMesh(const std::vector<SHVertex>& _vertices)
{


   std::string cache_filename("sh_cache.txt");
   std::vector<float> meshCoefs = generateMeshCoefs(_vertices.data(), _vertices.size(), cache_filename);
 
 
   shCofficientTexture(meshCoefs);

}


int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   // glfwWindowHint(GLFW_SAMPLES, 32);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "mini-renderer", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_POLYGON_OFFSET_FILL);
	//glEnable(GL_MULTISAMPLE);
	//glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glPolygonOffset(3.f, 3.f);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);



	shared_ptr<DirectionalLight>dirLight = make_shared<DirectionalLight>();
	dirLight->direction = lightPos;
	dirLight->color = glm::vec3(1.0);
	dirLight->intensity = 1.25f;
	dirLight->shadowsEnabled = true;
	dirLight->zFar = 7.5f;
	dirLight->zNear = 1.0f;


	glm::mat4 model1Matrix = glm::mat4(1.0f);
	model1Matrix = glm::translate(model1Matrix,     glm::vec3(0.0f, -0.3, 0.0f));
	model1Matrix = glm::scale(model1Matrix,         glm::vec3(0.1f, 0.1f, 0.1f));

	glm::mat4 model2Matrix = glm::mat4(1.0f);
	model2Matrix = glm::translate(model2Matrix,     glm::vec3(0.0f, -0.5f, 0.0f));
	model2Matrix = glm::rotate(model2Matrix,        glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model2Matrix = glm::scale(model2Matrix,         glm::vec3(10.0f, 10.0f, 10.0f));


	glm::mat4 model3Matrix = glm::mat4(1.0f);
	model3Matrix = glm::scale(model3Matrix,         glm::vec3(0.02f, 0.02f, 0.02f));
	model3Matrix = glm::translate(model3Matrix,     glm::vec3(80.0f, -25.5f, 2.0f));

	glm::mat4 model4Matrix = glm::mat4(1.0f);
	model4Matrix = glm::translate(model4Matrix,     glm::vec3(0.5f, 0.0f, 1.0f));
	model4Matrix = glm::rotate(model4Matrix, 15.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 model5Matrix = glm::mat4(1.0f);
	model5Matrix = glm::translate(model5Matrix,     glm::vec3(-0.8f, -0.45f, 1.0f));
	model5Matrix = glm::scale(model5Matrix,         glm::vec3(0.05f, 0.05f, 0.05f));

	std::shared_ptr<Model> model1 = std::make_shared<Model>("resources/objects/sphere.obj", ShadingType::BLIN_PHONG);

	std::shared_ptr<Model> model2 = std::make_shared<Model>("resources/objects/Quad.obj", ShadingType::BLIN_PHONG);
    //
    //std::shared_ptr<Model> model3=std::make_shared<Model>("resources/objects/boblampclean.md5mesh", ShadingType::BLIN_PHONG);
	//
    //std::shared_ptr<Model> model4=std::make_shared<Model>("resources/objects/cerberus.obj", ShadingType::PBR);
	//
	//std::shared_ptr<Model> model5 = std::make_shared<Model>("resources/objects/bunny.obj", ShadingType::BLIN_PHONG);

	//std::shared_ptr<Model> model = std::make_shared<Model>("resources/objects/testobj.obj", ShadingType::BLIN_PHONG);


	//model->setModelMatrix(model1Matrix);


    model1->setModelMatrix(model1Matrix);
    model2->setModelMatrix(model2Matrix);

    std::vector<std::shared_ptr<Model>>models;

	models.push_back(model1);
	models.push_back(model2);


  
    shared_ptr<Graphics> graphics =make_shared<Graphics>(RenderPath::RENDER_FORWARD,SCR_WIDTH,SCR_HEIGHT);



    graphics->addLight(dirLight);

    graphics->setCamera(camera);
   
    graphics->addModel(models);


	std::vector<SHVertex> _vertices;
	for (auto model : models)
	{
		for (const auto& mesh : model->meshes)
		{
			for (const auto& vertex : mesh.vertices)
			{
				_vertices.push_back({ vertex.Position,vertex.Normal });
			}
		}
	}


    shPreMesh(_vertices);

    unsigned int vao = Util::getMeshVAO(_vertices);
    
    
    std::shared_ptr<Material> blinnMaterial = graphics->getBlinnMaterial();


    blinnMaterial->setMatrix(std::string("model"),model1Matrix);

	blinnMaterial->setMatrix("view", camera->GetViewMatrix());
	blinnMaterial->setMatrix("projection", camera->GetProjectionMatrix());


    GLuint shCoefficients_location = glGetUniformLocation(blinnMaterial->getShader()->shaderProgram, "shCoefficients");
    

    while (!glfwWindowShouldClose(window))
    {

         float currentFrame = glfwGetTime();
         deltaTime = currentFrame - lastFrame;
         lastFrame = currentFrame;

         processInput(window);


		// graphics->enableHdrPost(eable_hdr_post);
		 //
		 //
		 //
		 //shadowMapPass->update();      


         glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
         glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
      
//		 graphics->update();

		 blinnMaterial->use();


         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, tboTextureID);
         glTexBuffer(GL_TEXTURE_2D,GL_R32F, tboID);
         glUniform1i(shCoefficients_location, 0);



		 glBindVertexArray(vao);

		 glDrawArrays(GL_TRIANGLES, 0, _vertices.size());


		 glGetError();

		 glBindVertexArray(0);



		 glUseProgram(0);

         glfwSwapBuffers(window);

         glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}



void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_KEY_DOWN)
        eable_hdr_post = !eable_hdr_post;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(yoffset);
}

