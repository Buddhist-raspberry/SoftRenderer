#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>

#include <glm/gtc/matrix_transform.hpp>

#include "SRApp.h"


const int width = 800;
const int height = 600;

glm::vec3       eye(1, 1, 2);       /*摄像机位置*/
glm::vec3    center(0, 0, 0);		/*摄像机朝向中心点*/
glm::vec3        up(0, 1, 0);		/*摄像机向上方向*/
glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

const float MoveSpeed = 0.5f;

Pipeline* pipeline = Pipeline::getInstance();

SRScene* scene;
SRApp* app;


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
	char* main_texture_name = "../obj/african_head/african_head_diffuse.tga";
	char* normal_texture_name = "../obj/african_head/african_head_nm_tangent.tga";
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


	ShaderBase* shader;
	ShaderBumpedNormal * s= new ShaderBumpedNormal();
	Texture2D* mainTex = new Texture2D();
	mainTex->loadTexture(main_texture_name);
	s->mainTex = mainTex;
	Texture2D* normalTex = new Texture2D();
	normalTex->loadTexture(normal_texture_name);
	s->normalTex = normalTex;
	shader = s;

	pipeline->ambient = new AmbientLight(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
	pipeline->AddLight(new DirectionalLight(glm::vec3(0,0, 1.0f), 4.0f, glm::vec3(0, -1, 0)));

	pipeline->SetBGColor(VecColor::LightSlateBlue);
	pipeline->Render(mesh, shader, colorbuffer);

	/*保存为图片*/
	std::string result_name = "result.png";
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);


	/*显示到窗口程序*/
	app = new SRApp();
	app->Init("SoftRenderer", width, height);
	app->SetMoveSpeed(MoveSpeed);
	app->Run(pipeline, mesh, shader, colorbuffer);
	app->Quit();

	delete app;
	delete scene;
	delete shader;
	stbi_image_free(colorbuffer);

	return 0;
}