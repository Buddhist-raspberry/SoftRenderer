#include "SRApp.h"
#include <iostream>

void SRApp::Init(const char* title, int width, int height, int x, int y) {
	this->width = width;
	this->height = height;

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		std::cout << SDL_GetError() << std::endl;
		return;
	}
	window = SDL_CreateWindow(title, x,y, width, height, SDL_WINDOW_SHOWN);
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

}


void SRApp::Display(unsigned char * colorbuffer) {
	SDL_LockSurface(surface);
	Uint32* destPixels = (Uint32*)surface->pixels;
	for (int i = 0; i < width*height; i++) {
		//RGBA To ARGB
		Uint32 color = (Uint32)colorbuffer[i * 4 + 3] << 24 | (Uint32)colorbuffer[i * 4] << 16 |
			(Uint32)colorbuffer[i * 4 + 1] << 8 | (Uint32)colorbuffer[i * 4 + 2];
		memcpy(destPixels + i, &color, sizeof(Uint32));
	}
	SDL_UnlockSurface(surface);
	SDL_UpdateWindowSurface(window);
}

void SRApp::Quit() {
	SDL_DestroyRenderer(render);
	SDL_Quit();
	window = nullptr;
	render = nullptr;
	surface = nullptr;
}


void SRApp::Run(Pipeline* pipeline , std::vector<SRMesh*> meshs, unsigned char * colorbuffer) {

	Display(colorbuffer);

	SDL_Event event;

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
					//ÒÆ¶¯
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
					//Ðý×ª
				case SDL_KeyCode::SDLK_1:
					std::cout << "Key 1 Pressed!\n";
					update = true;
					pipeline->RotateX(-rotateSpeed);
					break;
				case SDL_KeyCode::SDLK_2:
					std::cout << "Key 2 Pressed!\n";
					update = true;
					pipeline->RotateX(rotateSpeed);
					break;
				case SDL_KeyCode::SDLK_3:
					std::cout << "Key 3 Pressed!\n";
					update = true;
					pipeline->RotateY(-rotateSpeed);
					break;
				case SDL_KeyCode::SDLK_4:
					std::cout << "Key 4 Pressed!\n";
					update = true;
					pipeline->RotateY(rotateSpeed);
					break;
				case SDL_KeyCode::SDLK_5:
					std::cout << "Key 5 Pressed!\n";
					update = true;
					pipeline->RotateZ(-rotateSpeed);
					break;
				case SDL_KeyCode::SDLK_6:
					std::cout << "Key 6 Pressed!\n";
					update = true;
					pipeline->RotateZ(rotateSpeed);
					break;

				case SDL_KeyCode::SDLK_z:
					std::cout << "Key z Pressed!\n";
					update = true;
					for (size_t i = 0; i < meshs.size(); ++i) {
						pipeline->ModelMoveX(modelMoveSpeed, meshs[i]);
					}
					break;
				case SDL_KeyCode::SDLK_x:
					std::cout << "Key x Pressed!\n";
					update = true;
					for (size_t i = 0; i < meshs.size(); ++i) {
						pipeline->ModelMoveX(-modelMoveSpeed, meshs[i]);
					}
					break;
				case SDL_KeyCode::SDLK_c:
					std::cout << "Key c Pressed!\n";
					update = true;
					for (size_t i = 0; i < meshs.size(); ++i) {
						pipeline->ModelMoveY(modelMoveSpeed, meshs[i]);
					}
					break;
				case SDL_KeyCode::SDLK_v:
					std::cout << "Key v Pressed!\n";
					update = true;
					for (size_t i = 0; i < meshs.size(); ++i) {
						pipeline->ModelMoveY(-modelMoveSpeed, meshs[i]);
					}
					break;
				case SDL_KeyCode::SDLK_b:
					std::cout << "Key b Pressed!\n";
					update = true;
					for (size_t i = 0; i < meshs.size(); ++i) {
						pipeline->ModelMoveZ(modelMoveSpeed, meshs[i]);
					}
					break;
				case SDL_KeyCode::SDLK_n:
					std::cout << "Key n Pressed!\n";
					update = true;
					for (size_t i = 0; i < meshs.size(); ++i) {
						pipeline->ModelMoveZ(-modelMoveSpeed, meshs[i]);
					}
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
			pipeline->Render(meshs, colorbuffer);
			Display(colorbuffer);
		}

	}
}

void SRApp::Run(Pipeline* pipeline, SRMesh* mesh, unsigned char * colorbuffer) {
	std::vector<SRMesh*> meshs;
	meshs.push_back(mesh);
	Run(pipeline, meshs, colorbuffer);
}