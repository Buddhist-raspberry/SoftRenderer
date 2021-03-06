#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>

#include <glm/gtc/matrix_transform.hpp>

#include "SRApp.h"


const int width = 800;
const int height = 800;

glm::vec3       eye(1, 1, 2);       /*摄像机位置*/
glm::vec3    center(0, 0, 0);		/*摄像机朝向中心点*/
glm::vec3        up(0, 1, 0);		/*摄像机向上方向*/
glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

const float MoveSpeed = 0.5f;

Pipeline* pipeline = Pipeline::getInstance();

SRScene* scene;
SRApp* app;


int main(int argc, char** argv) {



	pipeline->SetModel(modelMatrix);
	pipeline->SetView(eye, center, up);
	pipeline->SetProjection(60.0f, width / height, 0.1f, 10.0f);
	pipeline->SetViewport(0, 0, width, height);

	char* model_name = "../../obj/african_head/african_head.obj";
	char* main_texture_name = "../../obj/african_head/african_head_diffuse.tga";
	char* normal_texture_name = "../../obj/african_head/african_head_nm_tangent.tga";
	SRMesh * mesh;

	/*读取模型*/
	scene = new SRScene();
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

	/*渲染*/
	unsigned char * colorbuffer = new unsigned char[4 * width*height];




	ShaderBumpedNormal * shader = new ShaderBumpedNormal();
	shader->mainColor = VecColor::White;
	shader->specularColor = VecColor::White;
	shader->gloss = 10.0f;
	shader->mainTex = new Texture2D();
	shader->mainTex->loadTexture(main_texture_name);
	shader->normalTex = new Texture2D();
	shader->normalTex->loadTexture(normal_texture_name);
	mesh->mShader = shader;

	pipeline->ambient = new AmbientLight(VecColor::White, 0.3f);
	pipeline->AddLight(new DirectionalLight(VecColor::White, 1.0f, glm::vec3(1, 1, 1)));

	pipeline->SetBGColor(VecColor::LightSlateBlue);
	pipeline->Render(mesh,colorbuffer);

	/*保存为图片*/
	std::string result_name = "05_BumpedNormal.png";
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);


	/*显示到窗口程序*/
	app = new SRApp();
	app->Init("05_BumpedNormal", width, height);
	app->SetMoveSpeed(MoveSpeed);
	app->Run(pipeline, mesh, colorbuffer);
	app->Quit();

	delete app;
	delete scene;
	delete shader;
	stbi_image_free(colorbuffer);

	return 0;
}