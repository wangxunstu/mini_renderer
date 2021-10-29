#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <memory>
#include<unordered_map>
#include <vector>

#include"mesh.h"
#include"shader.h"
#include "Material.h"



using namespace std;


#define NUM_BONES_PER_VEREX  4


class Model 
{
private:

    map<string, uint>  m_bone_mapping; 
    uint               m_num_bones = 0;
    vector<BoneMatrix> m_bone_matrices;
    aiMatrix4x4        m_global_inverse_transform;

    const aiScene*     m_scene;

    glm::mat4          m_modelMatrix;


    bool               m_hasBones = false;
    
public:

     ShadingType      m_shadingType;
   
     bool             m_useIndexBuffer = false;
    // model data 
    vector<Texture> m_texturesLoaded;	
    vector<Mesh>    meshes;
    string          directory;
    bool            gammaCorrection;

    glm::mat4       gloabalInverseTransform;
 

    Model(string const &path,ShadingType shadingType  ,bool gamma = false) : gammaCorrection(gamma),m_shadingType(shadingType)
    {
            loadModel(path);
    }

    void Draw(std::shared_ptr<Shader>shader);


    void setModelMatrix(const glm::mat4& matrix)
    {
        m_modelMatrix = matrix;
    }

    bool skining()const { return m_hasBones; }

    ShadingType getShadingID()const { return m_shadingType; }

private:

    void boneTransform(double time_in_sec, vector<glm::mat4>& transforms);

    uint findPosition(float p_animation_time, const aiNodeAnim* p_node_anim);

    uint findRotation(float p_animation_time, const aiNodeAnim* p_node_anim);

    uint findScaling(float p_animation_time, const aiNodeAnim* p_node_anim);

    aiVector3D calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim);

    aiQuaternion calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim);

    aiVector3D calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim);

    const aiNodeAnim* findNodeAnim(const aiAnimation* p_animation, const string p_node_name);

    void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform);

    glm::mat4 aiToGlm(aiMatrix4x4 ai_matr);

    Assimp::Importer importer;
    void loadModel(string const& path);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    void processNode(aiNode* node, const aiScene* scene);

    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};
