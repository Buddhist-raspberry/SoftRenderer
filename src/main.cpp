#include "VecColor.h"
#include "Pipeline.h"
#include "SRScene.h"

#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>
#include <glm/gtc/matrix_transform.hpp>

#include <SDL2/SDL.h>


const int width = 1024;
const int height = 768;

glm::vec3 light_dir(1, 1, 1);
glm::vec3       eye(1, 1, 2);
glm::vec3    center(0, 0, 0);
glm::vec3        up(0, 1, 0);
glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

const float moveSpeed = 0.5f;

Pipeline* pipeline = Pipeline::getInstance();


void runSDL(unsigned char * colorbuffer, SRMesh* mesh, ShaderBase* shader) {
	SDL_Window *window = nullptr;
	SDL_Renderer *render = nullptr;
	SDL_Surface * surface = nullptr;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		std::cout << SDL_GetError() << std::endl;
		return;
	}
	window = SDL_CreateWindow("SoftRenderer", 100, 50, width, height, SDL_WINDOW_SHOWN);
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
	for (int i = 0; i < width*height; i++) {
		//RGBA To ARGB
		Uint32 color = (Uint32)colorbuffer[i * 4 + 3] << 24 | (Uint32)colorbuffer[i * 4] << 16 |
			(Uint32)colorbuffer[i * 4 + 1] << 8 | (Uint32)colorbuffer[i * 4 + 2];
		memcpy(destPixels + i, &color, sizeof(Uint32));
	}
	//memcpy(destPixels , colorbuffer, sizeof(Uint32)*width*height);
	SDL_UnlockSurface(surface);
	SDL_UpdateWindowSurface(window);

	while (true) {
		bool quit = false;
		bool update = false;
		SDL_PollEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			switch (event.key.state) {
			case SDL_PRESSED:
				switch (event.key.keysym.sym) {
				case SDL_KeyCode::SDLK_w:
					std::cout << "Key W Pressed!\n";
					update = true;
					pipeline->MoveForward(moveSpeed);
					break;
				case SDL_KeyCode::SDLK_s:
					std::cout << "Key S Pressed!\n";
					update = true;
					pipeline->MoveForward(-moveSpeed);
					break;
				case SDL_KeyCode::SDLK_a:
					std::cout << "Key A Pressed!\n";
					update = true;
					pipeline->MoveRight(moveSpeed);
					break;
				case SDL_KeyCode::SDLK_d:
					std::cout << "Key D Pressed!\n";
					update = true;
					pipeline->MoveRight(-moveSpeed);
					break;
				case SDL_KeyCode::SDLK_q:
					std::cout << "Key Q Pressed!\n";
					update = true;
					pipeline->MoveUp(moveSpeed);
					break;
				case SDL_KeyCode::SDLK_e:
					std::cout << "Key E Pressed!\n";
					update = true;
					pipeline->MoveUp(-moveSpeed);
					break;
				case SDL_KeyCode::SDLK_ESCAPE:
					quit = true;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		if (quit)	break;
		if (update) {
			pipeline->Render(mesh, shader, colorbuffer);

			SDL_LockSurface(surface);
			Uint32* destPixels = (Uint32*)surface->pixels;
			for (int i = 0; i < width*height; i++) {
				//RGBA To ARGB
				Uint32 color = (Uint32)colorbuffer[i * 4 + 3] << 24 | (Uint32)colorbuffer[i * 4] << 16 |
					(Uint32)colorbuffer[i * 4 + 1] << 8 | (Uint32)colorbuffer[i * 4 + 2];
				memcpy(destPixels + i, &color, sizeof(Uint32));
			}
			//memcpy(destPixels , colorbuffer, sizeof(Uint32)*width*height);
			SDL_UnlockSurface(surface);
			SDL_UpdateWindowSurface(window);

		}

	}
	SDL_DestroyRenderer(render);
	//SDL_Quit();
}

int main(int argc, char** argv) {


	//if (2 > argc) {
	//	std::cerr << "Usage: " << argv[0] << "obj/model.obj" << std::endl;
	//	return 1;
	//}
	pipeline->SetModel(modelMatrix);
	pipeline->SetView(eye, center, up);
	pipeline->SetProjection(60.0f, width / height, 0.1f, 10.0f);
	pipeline->SetViewport(0, 0, width, height);

	//char* model_name = argv[1];
	char* model_name = "../obj/african_head/african_head.obj";
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

	//ShaderUV* shader = new ShaderUV();
	//ShaderPureColor* shader = new ShaderPureColor();
	//shader->color = VecColor::LightSkyBlue;
	ShaderTexture* shader = new ShaderTexture();
	Texture2D* tex = new Texture2D();
	tex->loadTexture("../obj/african_head/african_head_diffuse.tga");
	shader->mainTex = tex;

	pipeline->SetBGColor(VecColor::LightSlateBlue);
	//p->FillColor(colorbuffer,glm::vec4(0,0,1,0));
	pipeline->Render(mesh, shader, colorbuffer);
	std::string result_name = "result.png";

	if (argc >= 3) {
		result_name = argv[2];
	}
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);

	runSDL(colorbuffer, mesh, shader);

	delete scene;
	delete shader;
	stbi_image_free(colorbuffer);
	return 0;
}