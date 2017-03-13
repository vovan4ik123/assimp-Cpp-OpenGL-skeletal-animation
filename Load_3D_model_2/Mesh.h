#pragma once

#include <iostream>

#include "GL\glew.h"
#include "GL\freeglut.h"
#include "SDL\SDL_opengl.h"
#include "SDL\SDL.h"
#include "ForShader.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "InputHandler.h"
#include "Vector2D.h"
#include "Camera.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


using namespace std;
typedef unsigned int uint;
#define NUM_BONES_PER_VEREX 4

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 text_coords;
};

struct Texture
{
	GLuint id;
	string type;
	aiString path;
};

struct BoneMatrix
{
	aiMatrix4x4 offset_matrix;
	aiMatrix4x4 final_world_transform;

	//glm::mat4 offset_matrix;
	//glm::mat4 final_world_transform;
};

struct VertexBoneData
{
	uint ids[NUM_BONES_PER_VEREX];   // we be have 4 bone ids & 4 weights for EACH !!! vertex
	float weights[NUM_BONES_PER_VEREX];

	VertexBoneData()
	{
		memset(ids, 0, sizeof(ids));    // init all values in array = 0
		memset(weights, 0, sizeof(weights));
	}

	void addBoneData(uint bone_id, float weight);
};

struct MeshInfo {
	MeshInfo()
	{					// accumulate all vertices from many scene mesh in one vector
		base_index = 0; // offset in our one mesh after adding vertices from next scene->mesh[i]
	}

	uint base_index; // base_index == base_vertex
};

class Mesh
{
public:
	Mesh(vector<Vertex> vertic, vector<GLuint> ind, vector<Texture> textur, vector<VertexBoneData> bone_id_weights);
	Mesh() {};
	~Mesh();

	//Mesh data
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	vector<VertexBoneData> bones_id_weights_for_each_vertex;

	// Render mesh
	void Draw(GLuint shaders_program);

private:
	//buffers
	GLuint VAO;
	GLuint VBO_vertices;
	GLuint VBO_bones;
	GLuint EBO_indices;
	//inititalize buffers
	void SetupMesh();
};
