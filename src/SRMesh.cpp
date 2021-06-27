#include "SRMesh.h"

void SRMesh::SetShader(ShaderBase* shader) {
	if (mShader != nullptr) {
		delete mShader;
	}
	mShader = shader;
}

SRMesh::SRMesh(SRMesh* mesh) {
	this->mName = std::string(mesh->mName);
	this->mFaces = std::vector<glm::ivec3>(mesh->mFaces);
	this->mVertices = std::vector<glm::vec3>(mVertices);
	this->mUVs = std::vector<glm::vec3>(mUVs);
	this->mNormals = std::vector<glm::vec3>(mNormals);
	this->mTangents = std::vector<glm::vec3>(mTangents);
	this->mBitangents =  std::vector<glm::vec3>(mBitangents);
	this->mShader = mesh->mShader;
}