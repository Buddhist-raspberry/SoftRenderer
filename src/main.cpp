#include "pipeline.h"
#include "veccolor.h"
#include "SRScene.h"

#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>
#include <glm/gtc/matrix_transform.hpp>

#include <SDL2/SDL.h>


const int width = 800;
const int height = 600;

glm::vec3 light_dir(1, 1, 1);
glm::vec3       eye(1, 1, 2);
glm::vec3    center(0, 0, 0);
glm::vec3        up(0, 1, 0);
glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

Pipeline* p = Pipeline::getInstance();

void runSDL(unsigned char * colorbuffer) {
	SDL_Window *window = nullptr;
	SDL_Renderer *render = nullptr;
	SDL_Surface * surface = nullptr;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		std::cout << SDL_GetError() << std::endl;
		return;
	}
	window = SDL_CreateWindow("SoftRenderer", 100, 100,width, height, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cout << SDL_GetError() << std::endl;
		return;
	}
	render = SDL_CreateRenderer(window, -1, 0);
	if (render == nullptr) {
		std::cout << SDL_GetError() << std::endl;
		return;
	}
	surface = SDL_GetWindowSurface(window);

	SDL_LockSurface(surface);
	Uint32* destPixels = (Uint32*)surface->pixels;
	memcpy(destPixels, colorbuffer, sizeof(Uint32)*width*height);
	SDL_UnlockSurface(surface);
	SDL_UpdateWindowSurface(window);

	while (true) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;
		
	}
	SDL_DestroyRenderer(render);
	SDL_Quit();
}

int main(int argc, char** argv) {

	if (2 > argc) {
		std::cerr << "Usage: " << argv[0] << "obj/model.obj" << std::endl;
		return 1;
	}
	p->SetModel(modelMatrix);
	p->SetView(eye, center, up);
	p->SetProjection(60.0f, width / height, 0.1f, 10.0f);
	p->SetViewport(0, 0, width, height);

	char* model_name = argv[1];
	//char* model_name = "../obj/boggie/body.obj";
	SRMesh * mesh;

	SRScene* scene = new SRScene();
	if (scene->ReadSceneFromFile(model_name)) {
		mesh = scene->GetMesh(0);
		if (mesh) {
			std::cout << "Name: " << mesh->mName << std::endl;
		}
		else {
			std::cerr << "No mesh found in file!\n" << std::endl;
			return 1;
		}
	}
	else {
		std::cerr << "Read scene from file failed!\n" << std::endl;
		return 1;
	}

	unsigned char * colorbuffer = new unsigned char[4 * width*height];

	ShaderUV* shader=new ShaderUV();

	p->SetBGColor(VecColor::LightSlateBlue);
	p->Render(mesh, shader, colorbuffer);
	std::string result_name = "result.png";

	if (argc >= 3) {
		result_name = argv[2];
	}
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);

	runSDL(colorbuffer);

	delete scene;
	delete shader;
	stbi_image_free(colorbuffer);
	return 0;
}