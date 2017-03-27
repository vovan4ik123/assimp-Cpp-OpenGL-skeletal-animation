#include "Model.h"
#include "Triangle.h"
#include "InputHandler.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Model::Model()
{
	scene = nullptr;
}


Model::~Model()
{

}

void Model::init(GLuint shader_program)
{
	for (uint i = 0; i < MAX_BONES; i++) // get location all matrices of bones
	{
		string name = "bones[" + to_string(i) + "]";
		char name_char[128];
		strcpy_s(name_char, name.c_str());
		m_bone_location[i] = glGetUniformLocation(shader_program, name_char);
	}

	music = Mix_LoadMUS("music/modern_talking_jet_airliner.mp3");
	Mix_VolumeMusic(16);
	Mix_PlayMusic(music, 0); // -1 = NONSTOP playing

	music2 = Mix_LoadMUS("music/02_modern_talking_you_can_win_if_you_want.mp3");

	// rotate head AND AXIS(y_z) about x !!!!!  Not be gimbal lock
	//rotate_head_xz *= glm::quat(cos(glm::radians(-45.0f / 2)), sin(glm::radians(-45.0f / 2)) * glm::vec3(1.0f, 0.0f, 0.0f));
}

void Model::update()
{
	// making new quaternions for rotate head
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_1))
	{
		rotate_head_xz *= glm::quat(cos(glm::radians(1.0f / 2)), sin(glm::radians(1.0f / 2)) * glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_2))
	{
		rotate_head_xz *= glm::quat(cos(glm::radians(-1.0f / 2)), sin(glm::radians(-1.0f / 2)) * glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_3))
	{
		rotate_head_xz *= glm::quat(cos(glm::radians(1.0f / 2)), sin(glm::radians(1.0f / 2)) * glm::vec3(0.0f, 0.0f, 1.0f));
	}

	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_4))
	{
		rotate_head_xz *= glm::quat(cos(glm::radians(-1.0f / 2)), sin(glm::radians(-1.0f / 2)) * glm::vec3(0.0f, 0.0f, 1.0f));
	}

}

void Model::draw(GLuint shaders_program)
{
	vector<aiMatrix4x4> transforms;
	boneTransform((float) SDL_GetTicks() / 1000.0f, transforms);

	for (uint i = 0; i < transforms.size(); i++)
	{
		glUniformMatrix4fv(m_bone_location[i], 1, GL_TRUE, (const GLfloat*)&transforms[i]);
	}

	mesh.Draw(shaders_program);
}

void Model::playSound()
{
	if(Mix_PlayingMusic() == 0) 
		Mix_PlayMusic(music2, 1); // play next ( two time <- loop == 1 )
}

void Model::loadModel(const string& path)
{
	// how work skeletal animation in assimp :
	// node это отдельная часть загруженной модели (модель это не только персонаж)
	// например камера, арматура, куб, источник света, часть тела персонажа(нога, руга, голова).
	// к node может быть прикреплена кость 
	// в кости есть массив вершин на которые кость влияет (веса от 0 до 1).
	// каждый канал это одна aiNodeAnim.
	// В aiNodeAnim собраны преобразования(scaling rotate translate) для той кости с которой у них обшее название
	// эти преобразования изменят те вершины, ID которых есть в кости с силой равной весу.
	// кость просто содержит ID и вес вершин на которые повляет трансформация из aiNodeAnim ( у ниe c костью общее имя )
	// (массив вершин и вес преобразований для вершины есть в каждой кости)

	// результат: конкретная трансформация повлияет на конкретную вершину с определенной силой.

	scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "error assimp : " << import.GetErrorString() << endl;
		return;
	}
	m_global_inverse_transform = scene->mRootNode->mTransformation;
	m_global_inverse_transform.Inverse();

	cout << "scene->HasAnimations() 1: " << scene->HasAnimations() << endl;
	cout << "scene->mNumMeshes 1: " << scene->mNumMeshes << endl;
	cout << "scene->mAnimations[0]->mNumChannels 1: " << scene->mAnimations[0]->mNumChannels << endl;
	cout << "scene->mAnimations[0]->mDuration 1: " << scene->mAnimations[0]->mDuration << endl;
	cout << "scene->mAnimations[0]->mTicksPerSecond 1: " << scene->mAnimations[0]->mTicksPerSecond << endl;

	//we give road to model.obj ! directoru = container for model.obj and textures and other files
	directory = path.substr(0, path.find_last_of('/'));

	cout << "		name nodes : " << endl;
	showNodeName(scene->mRootNode);
	cout << endl;

	cout << "		name bones : " << endl;
	processNode(scene->mRootNode, scene);

	cout << "		name nodes animation : " << endl;
	for (int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
	{// у нас только одна анимация в этой сцене
		cout<< scene->mAnimations[0]->mChannels[i]->mNodeName.C_Str() << endl;
	}
	cout << endl;
}

void Model::showNodeName(aiNode* node)
{
	cout << node->mName.data << endl;
	for (int i = 0; i < node->mNumChildren; i++)
	{
		showNodeName(node->mChildren[i]);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	vector<VertexBoneData> bones_id_weights_for_each_vertex;

	m_mesh_infos.resize(scene->mNumMeshes);

	int num_vertic = 0;
	for (int i = 0; i < scene->mNumMeshes; i++) // определяем где в ашем общем векторе будет находится начало вершин следующей сетки
	{
		m_mesh_infos[i].base_index = num_vertic; // base_index == base_vertex
		num_vertic += scene->mMeshes[i]->mNumVertices;
	}
	vertices.reserve(num_vertic);
	indices.reserve(num_vertic);
	bones_id_weights_for_each_vertex.resize(num_vertic);

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		processMesh(mesh, i, scene, vertices, indices, textures, bones_id_weights_for_each_vertex); // собираем все вершины из всех мешей в одном векторе
	}

	mesh = Mesh(vertices, indices, textures, bones_id_weights_for_each_vertex); // собрать все в одном меше
}

void Model::processMesh(aiMesh* mesh, int mesh_index, const aiScene* scene, vector<Vertex> &vertices, vector<GLuint> &indices, vector<Texture> &textures, vector<VertexBoneData> &bones_id_weights)
{
	std::cout << "bones: " << mesh->mNumBones << " vertices: " << mesh->mNumVertices << std::endl;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		if (mesh->mNormals != NULL)
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		else
		{
			vertex.normal = glm::vec3();
		}
 

		// in assimp model can have 8 different texture coordinates
		// we only care about the first set of texture coordinates
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.text_coords = vec;
		}
		else
		{
			vertex.text_coords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	// indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i]; // индексы вершин в нашем собранном векторе из нескольких мешей
		indices.push_back(m_mesh_infos[mesh_index].base_index + face.mIndices[0]); // индексы начнутся с того места с которого добавятся новые вершины 
		indices.push_back(m_mesh_infos[mesh_index].base_index + face.mIndices[1]); // из новой сетки (в новой сетке вершины и их индексы начинаются с начала)
		indices.push_back(m_mesh_infos[mesh_index].base_index + face.mIndices[2]);
	}

	// material
	if (mesh->mMaterialIndex >= 0)
	{

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuse_maps = this->LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
		bool exist = false;
		for (int i = 0; i < textures.size() && diffuse_maps.size() != 0; i++)
		{
			if (textures[i].path ==  diffuse_maps[0].path) // должна быть максимум 1 текстура диффузе и 1 спекуляр в одном меше
			{
				exist = true;
			}
		} // add if texture not exist
		if(!exist && diffuse_maps.size() != 0) textures.push_back(diffuse_maps[0]);
		//textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		vector<Texture> specular_maps = this->LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
		exist = false;
		for (int i = 0; i < textures.size() && specular_maps.size() != 0; i++)
		{
			if (textures[i].path == specular_maps[0].path) // должна быть максимум 1 текстура диффузе и 1 спекуляр в одном меше
			{
				exist = true;
			}
		}// add if texture not exist
		if (!exist  && specular_maps.size() != 0) textures.push_back(specular_maps[0]);
		//textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	// load bones
	for (uint i = 0; i < mesh->mNumBones; i++)
	{
		uint bone_index = 0;
		string bone_name(mesh->mBones[i]->mName.data);
		cout << mesh->mBones[i]->mName.data << endl;

		if (m_bone_mapping.find(bone_name) == m_bone_mapping.end()) // проверить нет ли в векторе элемента
		{
			// Allocate an index for a new bone
			bone_index = m_num_bones;
			m_num_bones++;
			BoneMatrix bi;
			m_bone_matrices.push_back(bi);
			m_bone_matrices[bone_index].offset_matrix = mesh->mBones[i]->mOffsetMatrix;
			m_bone_mapping[bone_name] = bone_index;

			//cout << "bone_name: " << bone_name << "			 bone_index: " << bone_index << endl;
		}
		else
		{
			bone_index = m_bone_mapping[bone_name];
		}

		for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			uint vertex_id = m_mesh_infos[mesh_index].base_index + mesh->mBones[i]->mWeights[j].mVertexId; // ид вершины на которую влияет выбранная кость
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			bones_id_weights[vertex_id].addBoneData(bone_index, weight); // у каждой вершины будет кость и ее вес

			// индекс вершины vertex_id на которую кость с индексом bone_index  имеет вес weight
			//cout << " vertex_id: " << vertex_id << "	bone_index: " << bone_index << "		weight: " << weight << endl;
		}
	} 
}

vector<Texture> Model::LoadMaterialTexture(aiMaterial* mat, aiTextureType type, string type_name)
{
	vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString ai_str;
		mat->GetTexture(type, i, &ai_str);

		string filename = string(ai_str.C_Str());
		filename = directory + '/' + filename;

		//cout << filename << endl;

		Texture texture;
		texture.id = Triangle::loadImageToTexture(filename.c_str()); // return prepaired openGL texture
		texture.type = type_name;
		texture.path = ai_str;
		textures.push_back(texture);
	}
	return textures;
}

uint Model::findPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	for (uint i = 0; i < p_node_anim->mNumPositionKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mPositionKeys[i + 1].mTime) // время меньше чем время следующей позиции
		{
			return i; // значит возвращаем индекс текущей
		}
	}

	assert(0);
	return 0;
}

uint Model::findRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	assert(p_node_anim->mNumRotationKeys > 0);

	for (uint i = 0; i < p_node_anim->mNumRotationKeys - 1; i++)
	{
		if (p_animation_time < (float)p_node_anim->mRotationKeys[i + 1].mTime) // время меньше чем время следующей позиции
		{
			return i; // значит возвращаем индекс текущей
		}
	}

	assert(0);
	return 0;
}

uint Model::findScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	assert(p_node_anim->mNumScalingKeys > 0);

	for (uint i = 0; i < p_node_anim->mNumScalingKeys - 1; i++)
	{
 		if (p_animation_time < (float)p_node_anim->mScalingKeys[i + 1].mTime) // время меньше чем время следующей позиции
		{
			//cout << "p_node_anim->mScalingKeys[i + 1].mTime: " << p_node_anim->mScalingKeys[i + 1].mTime << endl;

			return i; // значит возвращаем индекс текущей
		}
	}

	assert(0);
	return 0;
}

aiVector3D Model::calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
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

aiQuaternion Model::calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
{
	if (p_node_anim->mNumRotationKeys == 1)
	{
		return p_node_anim->mRotationKeys[0].mValue;
	}

	uint rotation_index = findRotation(p_animation_time, p_node_anim);
	uint next_rotaion_index = rotation_index + 1;
	assert(next_rotaion_index < p_node_anim->mNumRotationKeys);

	float delta_time = (float)(p_node_anim->mRotationKeys[next_rotaion_index].mTime - p_node_anim->mRotationKeys[rotation_index].mTime);
	float factor = (p_animation_time - (float)p_node_anim->mRotationKeys[rotation_index].mTime) / delta_time;
	assert(factor >= 0.0f && factor <= 1.0f);
	aiQuaternion start_quat = p_node_anim->mRotationKeys[rotation_index].mValue;
	aiQuaternion end_quat = p_node_anim->mRotationKeys[next_rotaion_index].mValue;

	return nlerp(start_quat, end_quat, factor);
}

aiVector3D Model::calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
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

const aiNodeAnim * Model::findNodeAnim(const aiAnimation * p_animation, const string p_node_name)
{
	// channel in animation its key frame
	for (uint i = 0; i < p_animation->mNumChannels; i++) //каждый канал затрагивает один узел
	{
		const aiNodeAnim* node_anim = p_animation->mChannels[i]; // Описывает анимацию одного node ( один ключевой кадр)
		if (string(node_anim->mNodeName.data) == p_node_name) // если имена совпадают то анимация node представлена этой node_anim
		{
			return node_anim;
		}
	}

	return nullptr;
}
								// start from RootNode
void Model::readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform)
{

	string node_name(p_node->mName.data);

	//aiAnimation состоит из key frames data для нескольких node.
	//Каждому node, на который воздействует анимация, предоставляется отдельная серия данных(aiNodeAnim).
	const aiAnimation* animation = scene->mAnimations[0];
	aiMatrix4x4 node_transform = p_node->mTransformation;

	const aiNodeAnim* node_anim = findNodeAnim(animation, node_name); // найти анимацию для этого узла
	
	if (node_anim)
	{

		//scaling
		//aiVector3D scaling_vector = node_anim->mScalingKeys[2].mValue;
		aiVector3D scaling_vector = calcInterpolatedScaling(p_animation_time, node_anim);
		aiMatrix4x4 scaling_matr;
		aiMatrix4x4::Scaling(scaling_vector, scaling_matr);
		
		//rotation
		//aiQuaternion rotate_quat = node_anim->mRotationKeys[2].mValue;
		aiQuaternion rotate_quat = calcInterpolatedRotation(p_animation_time, node_anim);
		aiMatrix4x4 rotate_matr = aiMatrix4x4(rotate_quat.GetMatrix());

		//translation
		//aiVector3D translate_vector = node_anim->mPositionKeys[2].mValue;
		aiVector3D translate_vector = calcInterpolatedPosition(p_animation_time, node_anim);
		aiMatrix4x4 translate_matr;
		aiMatrix4x4::Translation(translate_vector, translate_matr);

		if ( string(node_anim->mNodeName.data) == string("Head"))
		{
			aiQuaternion rotate_head = aiQuaternion(rotate_head_xz.w, rotate_head_xz.x, rotate_head_xz.y, rotate_head_xz.z);

			node_transform = translate_matr * (rotate_matr * aiMatrix4x4(rotate_head.GetMatrix())) * scaling_matr;
		}
		else
		{
			node_transform = translate_matr * rotate_matr * scaling_matr;
		} 

	}

	aiMatrix4x4 global_transform = parent_transform * node_transform;

	// Если node представляет собой bone в иерархии, то имя node должно совпадать с именем bone !!!
	if (m_bone_mapping.find(node_name) != m_bone_mapping.end()) // true if node_name exist in bone_mapping
	{
		uint bone_index = m_bone_mapping[node_name];
		m_bone_matrices[bone_index].final_world_transform = m_global_inverse_transform * global_transform * m_bone_matrices[bone_index].offset_matrix;
	}

	for (uint i = 0; i < p_node->mNumChildren; i++)
	{
		readNodeHierarchy(p_animation_time, p_node->mChildren[i], global_transform);
	}

}

void Model::boneTransform(float time_in_sec, vector<aiMatrix4x4>& transforms)
{
	aiMatrix4x4 identity_matrix; // = mat4(1.0f);

	float ticks_per_second = 0.0f;
	if (scene->mAnimations[0]->mTicksPerSecond != 0)
	{
		ticks_per_second = scene->mAnimations[0]->mTicksPerSecond;
	}
	else
	{
		ticks_per_second = 25.0f;
	}
	float time_in_ticks = time_in_sec * ticks_per_second;
	float animation_time = fmod(time_in_ticks, (float)scene->mAnimations[0]->mDuration); //деление по модулю флот чисел
	assert(animation_time < scene->mAnimations[0]->mDuration);
	//cout << "animation_time: " << animation_time << endl;

	readNodeHierarchy(animation_time, scene->mRootNode, identity_matrix);
	
	transforms.resize(m_num_bones);

	for (uint i = 0; i < m_num_bones; i++)
	{
		transforms[i] = m_bone_matrices[i].final_world_transform;
	}
}

glm::mat4 Model::aiToGlm(aiMatrix4x4 ai_matr)
{
	glm::mat4 result;
	result[0].x = ai_matr.a1; result[0].y = ai_matr.b1; result[0].z = ai_matr.c1; result[0].w = ai_matr.d1;
	result[1].x = ai_matr.a2; result[1].y = ai_matr.b2; result[1].z = ai_matr.c2; result[1].w = ai_matr.d2;
	result[2].x = ai_matr.a3; result[2].y = ai_matr.b3; result[2].z = ai_matr.c3; result[2].w = ai_matr.d3;
	result[3].x = ai_matr.a4; result[3].y = ai_matr.b4; result[3].z = ai_matr.c4; result[3].w = ai_matr.d4;

	//cout << " " << result[0].x << "		 " << result[0].y << "		 " << result[0].z << "		 " << result[0].w << endl;
	//cout << " " << result[1].x << "		 " << result[1].y << "		 " << result[1].z << "		 " << result[1].w << endl;
	//cout << " " << result[2].x << "		 " << result[2].y << "		 " << result[2].z << "		 " << result[2].w << endl;
	//cout << " " << result[3].x << "		 " << result[3].y << "		 " << result[3].z << "		 " << result[3].w << endl;
	//cout << endl;

	//cout << " " << ai_matr.a1 << "		 " << ai_matr.b1 << "		 " << ai_matr.c1 << "		 " << ai_matr.d1 << endl;
	//cout << " " << ai_matr.a2 << "		 " << ai_matr.b2 << "		 " << ai_matr.c2 << "		 " << ai_matr.d2 << endl;
	//cout << " " << ai_matr.a3 << "		 " << ai_matr.b3 << "		 " << ai_matr.c3 << "		 " << ai_matr.d3 << endl;
	//cout << " " << ai_matr.a4 << "		 " << ai_matr.b4 << "		 " << ai_matr.c4 << "		 " << ai_matr.d4 << endl;
	//cout << endl;

	return result;
}

aiQuaternion Model::nlerp(aiQuaternion a, aiQuaternion b, float blend)
{
	a.Normalize();
	b.Normalize();
	aiQuaternion result;
	float dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	float one_minus_blend = 1.0f - blend;

	if (dot_product < 0.0f)
	{
		result.x = a.x * one_minus_blend + blend * -b.x;
		result.y = a.y * one_minus_blend + blend * -b.y;
		result.z = a.z * one_minus_blend + blend * -b.z;
		result.w = a.w * one_minus_blend + blend * -b.w;
	}
	else
	{
		result.x = a.x * one_minus_blend + blend * b.x;
		result.y = a.y * one_minus_blend + blend * b.y;
		result.z = a.z * one_minus_blend + blend * b.z;
		result.w = a.w * one_minus_blend + blend * b.w;
	}

	return result.Normalize();
}