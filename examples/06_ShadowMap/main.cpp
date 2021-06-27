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

glm::vec3       eye(2, 1, 4);       /*摄像机位置*/
glm::vec3    center(0, 0, 0);		/*摄像机朝向中心点*/
glm::vec3        up(0, 1, 0);		/*摄像机向上方向*/

const float MoveSpeed = 0.5f;

Pipeline* pipeline = Pipeline::getInstance();

SRApp* app;

SRMesh* GetHead() {
	char* model_name = "../../obj/african_head/african_head.obj";
	char* main_texture_name = "../../obj/african_head/african_head_diffuse.tga";
	char* normal_texture_name = "../../obj/african_head/african_head_nm_tangent.tga";



	SRScene* scene = new SRScene();
	scene->ReadSceneFromFile(model_name);
	SRMesh* head = scene->GetMesh(0);

	ShaderBumpedNormal * shader = new ShaderBumpedNormal();
	shader->mainColor = VecColor::White;
	shader->specularColor = VecColor::White;
	shader->gloss = 2.0f;
	shader->mainTex = new Texture2D();
	shader->mainTex->loadTexture(main_texture_name);
	shader->normalTex = new Texture2D();
	shader->normalTex->loadTexture(normal_texture_name);

	head->mShader = shader;
	//head->mShader = new ShaderUV();

	delete scene;
	return head;

}


SRMesh* GetCube() {
	char* model_name = "../../obj/cube/cube.obj";


	SRScene* scene = new SRScene();
	scene->ReadSceneFromFile(model_name);
	SRMesh* cube = scene->GetMesh(0);

	ShaderPhong * shader = new ShaderPhong();
	shader->diffuseColor = VecColor::White*0.2f;
	shader->specularColor = VecColor::White;
	shader->gloss = 1.0f;

	cube->mShader = shader;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f,0.2f,20.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -5.0f,0));
	cube->modelMatrix = modelMatrix;
	delete scene;
	return cube;
}

int main(int argc, char** argv) {

	std::vector<SRMesh*> meshs;
	meshs.push_back(GetCube());
	meshs.push_back(GetHead());

	pipeline->SetView(eye, center, up);
	pipeline->SetProjection(60.0f, width / height, 0.001f, 10.0f);
	pipeline->SetViewport(0, 0, width, height);


	/*渲染*/
	unsigned char * colorbuffer = new unsigned char[4 * width*height];



	pipeline->ambient = new AmbientLight(VecColor::White, 0.2f);
	pipeline->AddLight(new DirectionalLight(VecColor::White, 0.5f, glm::vec3(1, 1, 1)));

	pipeline->SetBGColor(VecColor::LightSlateBlue);

	pipeline->Render(meshs, colorbuffer);


	/*保存为图片*/
	std::string result_name = "06_ShadowMap.png";
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);



	/*显示到窗口程序*/
	app = new SRApp();
	app->Init("06_ShadowMap", width, height);
	app->SetMoveSpeed(MoveSpeed);
	app->Run(pipeline, meshs,  colorbuffer);
	app->Quit();

	delete app;
	stbi_image_free(colorbuffer);

	return 0;
}