#ifndef __SR_SCENE_H__
#define __SR_SCENE_H__
#include <assimp/scene.h>
#include <vector>
#include "SRMesh.h"

class SRScene
{
private:
	int numMeshes = 0;
	std::vector<SRMesh*> mMeshes;

public:


public:
	bool ReadSceneFromFile(const std::string& pFile);
	SRMesh* GetMesh(int index);

public:
	void ShowMeshDetails(const aiMesh* mesh) const;

private:
	SRMesh*  processMesh(const aiMesh* mesh);
};

#endif //__SR_SCENE_H__
