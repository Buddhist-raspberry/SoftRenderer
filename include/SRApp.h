#ifndef __SRAPP_H__
#define __SRAPP_H__
#include "VecColor.h"
#include "Pipeline.h"
#include "SRScene.h"
#include <SDL2/SDL.h>

//´°¿Ú³ÌÐòÀà
class SRApp
{
private:
	SDL_Window *window = nullptr;
	SDL_Renderer *render = nullptr;
	SDL_Surface * surface = nullptr;
private:
	int width;
	int height;
private:
	float moveSpeed=2.0f;
	float rotateSpeed=10.0f;
	float modelMoveSpeed=2.0f;
public:
	void Init(const char* title, int width,int height, int x=100, int y=50);
	void Display(unsigned char * colorbuffer);
	void Run(Pipeline* pipeline, SRMesh* mesh, unsigned char * colorbuffer);
	void Run(Pipeline* pipeline ,std::vector<SRMesh*> meshs, unsigned char * colorbuffer);
	void Quit();

public:
	void SetMoveSpeed( float _moveSpeed) { moveSpeed = _moveSpeed; }
	void SetRotateSpeed(float _rotateSpeed) { rotateSpeed = _rotateSpeed; }
	void SetModelMoveSpeed(float _modelMoveSpeed) { modelMoveSpeed = _modelMoveSpeed; }


};

#endif // !__SRAPP_H__
