#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp\Importer.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

typedef unsigned int uint;
#define NUM_BONES_PER_VEREX 4

struct Vertex 
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
 
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};


struct BoneMatrix
{
    aiMatrix4x4 offset_matrix;
    aiMatrix4x4 final_world_transform;

};

struct VertexBoneData
{
    uint ids[NUM_BONES_PER_VEREX];   
    float weights[NUM_BONES_PER_VEREX];

    VertexBoneData()
    {
        memset(ids, 0, sizeof(ids));  
        memset(weights, 0, sizeof(weights));
    }

	void addBoneData(uint bone_id, float weight)
	{
		for (uint i = 0; i < NUM_BONES_PER_VEREX; i++)
		{
			if (weights[i] == 0.0)
			{
				ids[i] = bone_id;
				weights[i] = weight;
				return;
			}
		}
	}
};


//void VertexBoneData::addBoneData(uint bone_id, float weight)
//{
//    for (uint i = 0; i < NUM_BONES_PER_VEREX; i++)
//    {
//        if (weights[i] == 0.0)
//        {
//            ids[i] = bone_id;
//            weights[i] = weight;
//            return;
//        }
//    }
//}


class Mesh {
public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    vector<VertexBoneData> bones_id_weights_for_each_vertex;




    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, vector<VertexBoneData> bone_id_weights)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        bones_id_weights_for_each_vertex = bone_id_weights;

        setupMesh();
    }

    void Draw(Shader &shader) 
    {
        // bind  textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;

        if (textures.size() < 1)
        {
			shader.setFloat("u_hasDiffuseTexture", 0.0f);

        }

        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
            {
                number = std::to_string(diffuseNr++);
                shader.setFloat("u_hasDiffuseTexture", 1.0f);
            }
            else if (name == "texture_specular")
            {
                number = std::to_string(specularNr++);
				shader.setFloat("u_hasSpecularTexture", 1.0f);

            }
            else if (name == "texture_normal")
            {
                number = std::to_string(normalNr++);
				shader.setFloat("u_hasNormalTexture", 1.0f);
            }
            else if (name == "texture_height")
            {
                number = std::to_string(heightNr++);
				shader.setFloat("u_hasHeightTexture", 1.0f);

            }

            GLint id = 0;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);

            shader.bindTexture2D(textures[i].id, (name + number).c_str(), id+1);
         
           // glUniform1i(glGetUniformLocation(shader.shaderProgram, (name + number).c_str()), i);
           //
           // glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
      
        if (indices.size() > 0)
        {
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }
        else 
        {
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        }

        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

private:
   
    //buffers
    GLuint VAO;
    GLuint VBO_vertices;
    GLuint VBO_bones;
    GLuint EBO_indices;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        //vertices data
        glGenBuffers(1, &VBO_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //bones data
        glGenBuffers(1, &VBO_bones);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
        glBufferData(GL_ARRAY_BUFFER, bones_id_weights_for_each_vertex.size() * sizeof(bones_id_weights_for_each_vertex[0]), &bones_id_weights_for_each_vertex[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (indices.size() > 0)
        {
            //numbers for sequence indices
            glGenBuffers(1, &EBO_indices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        // create VAO and binding data from buffers to shaders
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
        //vertex 
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        glEnableVertexAttribArray(1); 
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //bones
        glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (GLvoid*)0); // for INT Ipointer
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (GLvoid*)offsetof(VertexBoneData, weights));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //indices
        
        if (indices.size() > 0)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_indices);
        }

        glBindVertexArray(0);
    }
};
#endif
