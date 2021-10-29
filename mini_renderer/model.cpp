#include "model.h"
#include <GLFW/glfw3.h>
#include "Util.h"


void Model:: Draw(std::shared_ptr<Shader>shader )
{
	shader->Bind();

	shader->setMat4("model", m_modelMatrix);
   
	shader->setInt("shadingID", (unsigned int)m_shadingType);

	if (m_hasBones)
	{
		vector<glm::mat4>transforms;

		boneTransform(glfwGetTime(), transforms);

		for (unsigned int i = 0; i < transforms.size(); i++)
		{
			shader->setMat4("Bones[" + to_string(i) + "]", transforms[i]);
		}

		shader->setInt("skining", 1.0f);
	}
	else 
	{
		shader->setInt("skining", 0.0f);
	}

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(*shader.get());

	shader->unBind();
}


void Model:: boneTransform(double time_in_sec, vector<glm::mat4>& transforms)
{
	aiMatrix4x4 identity_matrix;

	float TicksPerSecond = (float)(m_scene->mAnimations[0]->mTicksPerSecond != 0 ? m_scene->mAnimations[0]->mTicksPerSecond : 25.0f);

	double time_in_ticks = time_in_sec * TicksPerSecond;
	float animation_time = fmod(time_in_ticks, m_scene->mAnimations[0]->mDuration);

	readNodeHierarchy(animation_time, m_scene->mRootNode, identity_matrix);

	transforms.resize(m_num_bones);

	for (uint i = 0; i < m_num_bones; i++)
	{
		transforms[i] = aiToGlm(m_bone_matrices[i].final_world_transform);
	}
}


uint Model:: findPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
{

	for (uint i = 0; i < p_node_anim->mNumPositionKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

uint Model:: findRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
{

	for (uint i = 0; i < p_node_anim->mNumRotationKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

uint Model:: findScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
{

	for (uint i = 0; i < p_node_anim->mNumScalingKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

aiVector3D Model:: calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumPositionKeys == 1)
	{
		return p_node_anim->mPositionKeys[0].mValue;
	}

	uint position_index = findPosition(p_animation_time, p_node_anim);
	uint next_position_index = position_index + 1;
	assert(next_position_index < p_node_anim->mNumPositionKeys);

	float delta_time = (float)(p_node_anim->mPositionKeys[next_position_index].mTime - p_node_anim->mPositionKeys[position_index].mTime);

	float factor = (p_animation_time - (float)p_node_anim->mPositionKeys[position_index].mTime) / delta_time;
	assert(factor >= 0.0f && factor <= 1.0f);
	aiVector3D start = p_node_anim->mPositionKeys[position_index].mValue;
	aiVector3D end = p_node_anim->mPositionKeys[next_position_index].mValue;
	aiVector3D delta = end - start;

	return start + factor * delta;
}

aiQuaternion Model:: calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumRotationKeys == 1)
	{
		return p_node_anim->mRotationKeys[0].mValue;
	}

	uint rotation_index = findRotation(p_animation_time, p_node_anim);
	uint next_rotation_index = rotation_index + 1;
	assert(next_rotation_index < p_node_anim->mNumRotationKeys);

	float delta_time = (float)(p_node_anim->mRotationKeys[next_rotation_index].mTime - p_node_anim->mRotationKeys[rotation_index].mTime);

	float factor = (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) / delta_time;


	assert(factor >= 0.0f && factor <= 1.0f);
	aiQuaternion start_quat = p_node_anim->mRotationKeys[rotation_index].mValue;
	aiQuaternion end_quat = p_node_anim->mRotationKeys[next_rotation_index].mValue;

	aiQuaternion q;
	aiQuaternion::Interpolate(q, start_quat, end_quat, factor);

	return q.Normalize();
}

aiVector3D Model:: calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumScalingKeys == 1)
	{
		return p_node_anim->mScalingKeys[0].mValue;
	}

	uint scaling_index = findScaling(p_animation_time, p_node_anim);
	uint next_scaling_index = scaling_index + 1;
	assert(next_scaling_index < p_node_anim->mNumScalingKeys);

	float delta_time = (float)(p_node_anim->mScalingKeys[next_scaling_index].mTime - p_node_anim->mScalingKeys[scaling_index].mTime);

	float  factor = (p_animation_time - (float)p_node_anim->mScalingKeys[scaling_index].mTime) / delta_time;
	assert(factor >= 0.0f && factor <= 1.0f);
	aiVector3D start = p_node_anim->mScalingKeys[scaling_index].mValue;
	aiVector3D end = p_node_anim->mScalingKeys[next_scaling_index].mValue;
	aiVector3D delta = end - start;

	return start + factor * delta;
}

const aiNodeAnim* Model:: findNodeAnim(const aiAnimation* p_animation, const string p_node_name)
{

	for (uint i = 0; i < p_animation->mNumChannels; i++)
	{
		const aiNodeAnim* node_anim = p_animation->mChannels[i];
		if (string(node_anim->mNodeName.data) == p_node_name)
		{
			return node_anim;
		}
	}

	return nullptr;
}
void Model:: readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform)
{

	string node_name(p_node->mName.data);

	const aiAnimation* animation = m_scene->mAnimations[0];
	aiMatrix4x4 node_transform = p_node->mTransformation;

	const aiNodeAnim* node_anim = findNodeAnim(animation, node_name);

	if (node_anim)
	{
		aiVector3D scaling_vector = calcInterpolatedScaling(p_animation_time, node_anim);
		aiMatrix4x4 scaling_matr;
		aiMatrix4x4::Scaling(scaling_vector, scaling_matr);

		//rotation

		aiQuaternion rotate_quat = calcInterpolatedRotation(p_animation_time, node_anim);
		aiMatrix4x4 rotate_matr = aiMatrix4x4(rotate_quat.GetMatrix());

		//translation

		aiVector3D translate_vector = calcInterpolatedPosition(p_animation_time, node_anim);
		aiMatrix4x4 translate_matr;
		aiMatrix4x4::Translation(translate_vector, translate_matr);

		node_transform = translate_matr * rotate_matr * scaling_matr;
	}

	aiMatrix4x4 global_transform = parent_transform * node_transform;

	if (m_bone_mapping.find(node_name) != m_bone_mapping.end())
	{
		uint bone_index = m_bone_mapping[node_name];
		m_bone_matrices[bone_index].final_world_transform = m_global_inverse_transform * global_transform * m_bone_matrices[bone_index].offset_matrix;
	}

	for (uint i = 0; i < p_node->mNumChildren; i++)
	{
		readNodeHierarchy(p_animation_time, p_node->mChildren[i], global_transform);
	}
}

glm::mat4 Model:: aiToGlm(aiMatrix4x4 ai_matr)
{
	glm::mat4 result;
	result[0].x = ai_matr.a1; result[0].y = ai_matr.b1; result[0].z = ai_matr.c1; result[0].w = ai_matr.d1;
	result[1].x = ai_matr.a2; result[1].y = ai_matr.b2; result[1].z = ai_matr.c2; result[1].w = ai_matr.d2;
	result[2].x = ai_matr.a3; result[2].y = ai_matr.b3; result[2].z = ai_matr.c3; result[2].w = ai_matr.d3;
	result[3].x = ai_matr.a4; result[3].y = ai_matr.b4; result[3].z = ai_matr.c4; result[3].w = ai_matr.d4;


	return result;
}

void Model:: loadModel(string const& path)
{
	
	m_scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_GenSmoothNormals | aiProcess_OptimizeMeshes);

	if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	
	directory = path.substr(0, path.find_last_of('/'));

	processNode(m_scene->mRootNode, m_scene);

}
void Model:: processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));

	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

Mesh Model:: processMesh(aiMesh* mesh, const aiScene* scene)
{
	
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	vector<VertexBoneData> bones_id_weights_for_each_vertex;


	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) 
		{
			glm::vec2 vec;

			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);



		vertices.push_back(vertex);
	}


	bones_id_weights_for_each_vertex.resize(mesh->mNumVertices);

	if (mesh->HasBones())
	{
		m_hasBones = true;

		// load bones
		for (uint i = 0; i < mesh->mNumBones; i++)
		{
			uint bone_index = 0;
			string bone_name(mesh->mBones[i]->mName.data);

			if (m_bone_mapping.find(bone_name) == m_bone_mapping.end())
			{

				bone_index = m_num_bones;
				m_num_bones++;
				BoneMatrix bi;
				m_bone_matrices.push_back(bi);
				m_bone_matrices[bone_index].offset_matrix = mesh->mBones[i]->mOffsetMatrix;
				m_bone_mapping[bone_name] = bone_index;

			}
			else
			{
				bone_index = m_bone_mapping[bone_name];
			}

			for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
			{
				uint vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = mesh->mBones[i]->mWeights[j].mWeight;
				bones_id_weights_for_each_vertex[vertex_id].addBoneData(bone_index, weight);
			}
		}
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


	if (!m_useIndexBuffer && indices.size()>0)
	{
		std::vector<Vertex>outVertices;

		for (size_t i = 0; i < indices.size(); i++)
		{
			outVertices.push_back(vertices[indices[i]]);
		}

		return Mesh(outVertices, {}, textures, bones_id_weights_for_each_vertex);

	}


	return Mesh(vertices, indices, textures, bones_id_weights_for_each_vertex);
}

vector<Texture> Model:: loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{

	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < m_texturesLoaded.size(); j++)
		{
			if (std::strcmp(m_texturesLoaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(m_texturesLoaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id =Util::TextureFromFile(str.C_Str(), this->directory,false);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			m_texturesLoaded.push_back(texture);
		}
	}
	return textures;
}

