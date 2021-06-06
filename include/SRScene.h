#ifndef __SR_SCENE_H__
#define __SR_SCENE_H__
#include <assimp/scene.h>
#include <vector>
#include "SRMesh.h"

//������
class SRScene
{
private:
	int numMeshes = 0;
	std::vector<SRMesh*> mMeshes;    /*�����е�ģ��*/

public:


public:
	bool ReadSceneFromFile(const std::string& pFile);   /*���ļ���ȡ����*/
	SRMesh* GetMesh(int index);

public:
	void ShowMeshDetails(const aiMesh* mesh) const;     /*��ʾģ����ϸ��Ϣ*/

private:
	SRMesh*  processMesh(const aiMesh* mesh);			/*���������񣬴�assimp��ʽתΪSRMesh*/
};

#endif //__SR_SCENE_H__
