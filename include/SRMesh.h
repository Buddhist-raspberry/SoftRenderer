#ifndef __SR_MESH_H__
#define __SR_MESH_H__
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "Shader.h"

//模型网格类
class SRMesh
{
public:
	ShaderBase* mShader=nullptr;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
public:
	void SetShader(ShaderBase* shader);
public:
	std::string mName;
	std::vector<glm::ivec3> mFaces;		
	std::vector<glm::vec3> mVertices;
	std::vector<glm::vec3> mUVs;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec3> mTangents;
	std::vector<glm::vec3> mBitangents;

	unsigned int nFaces() const {
		return mFaces.size();
	}
	unsigned int nVertices() const {
		return mVertices.size();
	}
	unsigned int nUVs() const {
		return mUVs.size();
	}
	unsigned int nNormals() const {
		return mNormals.size();
	}
	unsigned int nTangents() const {
		return mTangents.size();
	}
	unsigned int nBitangents() const {
		return mBitangents.size();
	}
	glm::vec3 GetVert(unsigned int faceIndex, unsigned int vertIndex) {
		//printf("Get Vert(%d,%d)\n", faceIndex, vertIndex);
		return mVertices.at(mFaces.at(faceIndex)[vertIndex]);
	}
	glm::vec3 GetUV(unsigned int faceIndex, unsigned int vertIndex) {
		//printf("Get UV(%d,%d)\n", faceIndex, vertIndex);
		return mUVs.at(mFaces.at(faceIndex)[vertIndex]);
	}
	glm::vec3 GetNormal(unsigned int faceIndex, unsigned int vertIndex) {
		//printf("Get Normal(%d,%d)\n", faceIndex, vertIndex);
		return mNormals.at(mFaces.at(faceIndex)[vertIndex]);
	}
	glm::vec3 GetTangent(unsigned int faceIndex, unsigned int vertIndex) {
		//printf("Get Tangent(%d,%d)\n", faceIndex, vertIndex);
		return mTangents.at(mFaces.at(faceIndex)[vertIndex]);
	}
	glm::vec3 GetBitangent(unsigned int faceIndex, unsigned int vertIndex) {
		//printf("Get Bitangent(%d,%d)\n", faceIndex, vertIndex);
		return mBitangents.at(mFaces.at(faceIndex)[vertIndex]);
	}

};

#endif // __SR_MESH_H__
