#ifndef __SR_SCENE_H__
#define __SR_SCENE_H__
#include <assimp/scene.h>
#include <vector>
#include "SRMesh.h"

//场景类
class SRScene
{
private:
	int numMeshes = 0;
	std::vector<SRMesh*> mMeshes;    /*场景中的模型*/

public:


public:
	bool ReadSceneFromFile(const std::string& pFile);   /*从文件读取场景*/
	SRMesh* GetMesh(int index);

public:
	void ShowMeshDetails(const aiMesh* mesh) const;     /*显示模型详细信息*/

private:
	SRMesh*  processMesh(const aiMesh* mesh);			/*处理单个网格，从assimp格式转为SRMesh*/
};

#endif //__SR_SCENE_H__
