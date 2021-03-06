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

glm::vec3       eye(0, 2, 3);       /*摄像机位置*/
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

	char* model_name = "../../obj/cube/cube.obj";

	/*读取模型*/

	/*渲染*/
	unsigned char * colorbuffer = new unsigned char[4 * width*height];

	ShaderPhong * shader = new ShaderPhong();
	shader->diffuseColor = VecColor::White;
	shader->specularColor = VecColor::White;
	shader->gloss = 5.0f;

	std::vector<SRMesh*> meshs;
	//float pad = 1.0f;
	//for (int x = -1; x <=1; x++) {
	//	for (int y = -1; y <=1; y++) {
	//		scene = new SRScene();
	//		if (scene->ReadSceneFromFile(model_name)) {
	//			SRMesh* mesh = scene->GetMesh(0);
	//			mesh->modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x*pad, y*pad, 0));
	//			mesh->SetShader(shader);
	//			meshs.push_back(mesh);
	//		}
	//	}
	//}
	scene = new SRScene();
	if (scene->ReadSceneFromFile(model_name)) {
		SRMesh* mesh = scene->GetMesh(0);
		mesh->modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
		mesh->SetShader(shader);
		meshs.push_back(mesh);
	}



	pipeline->ambient = new AmbientLight(VecColor::White, 0.1f);
	pipeline->AddLight(new SpotLight(VecColor::LightSkyBlue, 10.0f, glm::vec3(0, -0.5f,0),
		glm::vec3(0,1.0f,0), 1.0f, 0.7f, 1.8f,30.0f,45.0f));

	pipeline->SetBGColor(VecColor::Black);
	pipeline->Render(meshs, colorbuffer);

	/*保存为图片*/
	std::string result_name = "09_SpotLight.png";
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);


	/*显示到窗口程序*/
	app = new SRApp();
	app->Init("09_SpotLight", width, height);
	app->SetMoveSpeed(MoveSpeed);
	app->Run(pipeline, meshs, colorbuffer);
	app->Quit();

	delete app;
	delete scene;
	delete shader;
	stbi_image_free(colorbuffer);

	return 0;
}