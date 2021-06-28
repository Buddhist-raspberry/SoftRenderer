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

	//char* model_name = "../../obj/african_head/african_head.obj";
	char* model_name = "../../obj/Models/model1/model_normalized.obj";
	

	std::vector<SRMesh*>meshs;
	/*读取模型*/
	scene = new SRScene();
	if (scene->ReadSceneFromFile(model_name)) {
		for (int i = 0; i < scene->numMeshes; i++) {
			SRMesh * mesh = scene->GetMesh(i);
			ShaderPhong * shader = new ShaderPhong();
			shader->diffuseColor = VecColor::White;
			shader->specularColor = VecColor::White;
			shader->gloss = 5.0f;
			mesh->mShader = shader;
			meshs.push_back(mesh);
		}
	}
	else {
		std::cerr << "Read scene from file failed!\n" << std::endl;
		return 1;
	}

	/*渲染*/
	unsigned char * colorbuffer = new unsigned char[4 * width*height];






	pipeline->ambient = new AmbientLight(VecColor::White, 0.4f);
	pipeline->AddLight(new DirectionalLight(VecColor::White, 0.5f, glm::vec3(0, 1, 0)));

	pipeline->SetBGColor(VecColor::LightSlateBlue);
	pipeline->Render(meshs, colorbuffer);

	/*保存为图片*/
	std::string result_name = "03_Phong_model1.png";
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);


	/*显示到窗口程序*/
	app = new SRApp();
	app->Init("03_Phong", width, height);
	app->SetMoveSpeed(MoveSpeed);
	app->Run(pipeline, meshs, colorbuffer);
	app->Quit();

	delete app;
	delete scene;
	stbi_image_free(colorbuffer);

	return 0;
}