#include "SRScene.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>




bool SRScene::ReadSceneFromFile(const std::string& pFile) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	

	if (!scene) {
		std::cerr << "Read scene error!\n";
		std::cerr << importer.GetErrorString()<<"\n";
		return false;
	}
	if (!scene->HasMeshes()) {
		std::cerr << "No mesh detected in this scene!\n";
		return false;
	}

	std::cout << "Read " << scene->mNumMeshes << " meshes from \"" << pFile << "\" successfully!\n";
	//for (int i = 0; i < scene->mNumMeshes; i++) {
	//	std::cout << "Mesh#" << i << ": " << scene->mMeshes[i]->mName.C_Str() << std::endl;
	//}

	
	numMeshes = scene->mNumMeshes;
	for (int i = 0; i < numMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		std::cout << "\nStart Processing Mesh#" << i << "\n";
		ShowMeshDetails(mesh);
		mMeshes.push_back(processMesh(mesh));
		std::cout << "Finish Processing Mesh#" << i << "\n\n";
	}


	return true;

}

void SRScene::ShowMeshDetails(const aiMesh* mesh) const {
	std::cout << "Name: " << mesh->mName.C_Str() << std::endl;
	std::cout << "NumColorChannels: " << mesh->GetNumColorChannels() << std::endl;
	std::cout << "NumUVChannels: " << mesh->GetNumUVChannels() << std::endl;
	std::cout << "NumFaces: " << mesh->mNumFaces << std::endl;
	std::cout << "NumVertices: " << mesh->mNumVertices << std::endl;
	std::cout << "HasNormal:" << (mesh->HasNormals()?"True":"False") << std::endl;
}

glm::vec3 vec3_ai2glm(const aiVector3D& vec_ai ) {
	return glm::vec3(vec_ai.x, vec_ai.y, vec_ai.z);
}


SRMesh*  SRScene::processMesh(const aiMesh* mesh) {
	SRMesh* srMesh = new SRMesh();
	srMesh->mName = std::string(mesh->mName.C_Str());
	/******************************Face*********************************/
	aiFace* faces = mesh->mFaces;
	for (int i = 0; i < mesh->mNumFaces; i++) {
		glm::ivec3 face = glm::ivec3(faces[i].mIndices[0], faces[i].mIndices[1], faces[i].mIndices[2]);
		srMesh->mFaces.push_back(face);
	}

	/***********************Vertex,UV,Normal,Tagent,Bitagent******************************/
	
	aiVector3D * vertices = mesh->mVertices;
	aiVector3D * uvs = mesh->mTextureCoords[0];
	aiVector3D * normals = mesh->mNormals;
	aiVector3D * tagents = mesh->mTangents;
	aiVector3D * bitagents = mesh->mBitangents;
	
	for (int i = 0; i < mesh->mNumVertices; i++) {
		srMesh->mVertices.push_back(vec3_ai2glm(vertices[i]));
		if (mesh->HasTextureCoords(0)) {
			srMesh->mUVs.push_back(vec3_ai2glm(uvs[i]));
		}
		if (mesh->HasNormals()) {
			srMesh->mNormals.push_back(vec3_ai2glm(normals[i]));
		}
		if (mesh->HasTangentsAndBitangents()) {
			srMesh->mTangents.push_back(vec3_ai2glm(tagents[i]));
			srMesh->mBitangents.push_back(vec3_ai2glm(bitagents[i]));
		}
	}
	return srMesh;
}




SRMesh* SRScene::GetMesh(int index) {
	if (index<0 || index>=numMeshes) {
		std::cerr << "Error! Mesh index out of range!\n";
		return nullptr;
	}
	std::cout << "Get Mesh#" << index << "!\n";
	return mMeshes.at(index);
}