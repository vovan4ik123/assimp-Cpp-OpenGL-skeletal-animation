#pragma once

#include "GL\glew.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include <map>

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

using namespace std;

class Model
{
public:
	Model();
	~Model();
	static const uint MAX_BONES = 100;


	void init(GLuint shader_program);
	void loadModel(const string& path);
	void draw(GLuint shaders_program);
	void callRecurs(aiNode* node);

	glm::mat4 aiToGlm(aiMatrix4x4 ai_matr);
	aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend); // super super n lerp =)

private:
	Assimp::Importer import;
	const aiScene* scene;
	Mesh mesh; // accumulate all meshes from scene in one mesh
	string directory;
	vector<MeshInfo> m_mesh_infos;

	map<string, uint> m_bone_mapping; // maps a bone name to its index
	uint m_num_bones = 0;
	vector<BoneMatrix> m_bone_matrices;
	aiMatrix4x4 m_global_inverse_transform;

	GLuint m_bone_location[MAX_BONES];

	void processNode(aiNode* node, const aiScene* scene);
	void Model::processMesh(aiMesh* mesh, int mesh_index, const aiScene* scene, vector<Vertex> &vertices, vector<GLuint> &indices, vector<Texture> &textures, vector<VertexBoneData> &bones);
	vector<Texture> LoadMaterialTexture(aiMaterial* mat, aiTextureType type, string type_name);

	uint findPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	uint findRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	uint findScaling(float p_animation_time, const aiNodeAnim* p_node_anim);
	const aiNodeAnim* findNodeAnim(const aiAnimation* p_animation, const string p_node_name);

	aiVector3D calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiQuaternion calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiVector3D calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim);


	void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform);
	void boneTransform(float time_in_sec, vector<aiMatrix4x4>& transforms);
};

