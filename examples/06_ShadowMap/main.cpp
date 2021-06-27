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

glm::vec3       eye(2, 1, 4);       /*�����λ��*/
glm::vec3    center(0, 0, 0);		/*������������ĵ�*/
glm::vec3        up(0, 1, 0);		/*��������Ϸ���*/

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

	delete scene;
	return head;

}

SRMesh* GetCube() {
	char* model_name = "../../obj/cube/cube.obj";

	SRScene* scene = new SRScene();
	scene->ReadSceneFromFile(model_name);
	SRMesh* cube = scene->GetMesh(0);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f, 0.2f, 20.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -8.0f, 0));
	cube->modelMatrix = modelMatrix;

	delete scene;
	return cube;
}

int main(int argc, char** argv) {

	std::vector<SRMesh*> meshs;
	//meshs.push_back(GetCube());
	meshs.push_back(GetHead());

	pipeline->SetProjection(60.0f, width / height, 0.01f, 10.0f);
	pipeline->SetViewport(0, 0, width, height);

	unsigned char * shadowMap = new unsigned char[4 * width*height];
	unsigned char * colorbuffer = new unsigned char[4 * width*height];


	pipeline->ambient = new AmbientLight(VecColor::White, 0);
	pipeline->AddLight(new DirectionalLight(VecColor::White, 1.0f, glm::vec3(-1, 1, 0)));
	pipeline->SetBGColor(VecColor::White);

	//��ȾShadowMap
	{
		for (int i = 0; i < meshs.size(); i++) {
			ShaderDepth* shader = new ShaderDepth();
			meshs.at(i)->SetShader(shader);
		}

		glm::vec3 lightPos = pipeline->GetLight(0)->GetDirection(glm::vec3());

		pipeline->SetView(lightPos*5.0f, center, up);

		pipeline->Render(meshs, shadowMap);

		std::string result_name = "06_ShadowMap_Depth.png";
		stbi_write_png(result_name.c_str(), width, height, 4, shadowMap, 0);
	}
	glm::mat4 shadowVP = pipeline->getVP();
	DepthMap* shadowMapTexture = new DepthMap(shadowMap, width, height);
	{
		for (int i = 0; i < meshs.size(); i++) {
			ShaderPhongWithShadow* shader = new ShaderPhongWithShadow();
			shader->diffuseColor = VecColor::White;
			shader->specularColor = VecColor::White;
			shader->gloss = 5.0f;
			shader->shadowMap = shadowMapTexture;
			shader->shadowVP = shadowVP;
			meshs.at(i)->SetShader(shader);
		}
	}


	pipeline->SetView(eye, center, up);
	pipeline->SetBGColor(VecColor::LightSlateBlue);
	pipeline->Render(meshs, colorbuffer);


	/*����ΪͼƬ*/
	std::string result_name = "06_ShadowMap.png";
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);



	/*��ʾ�����ڳ���*/
	app = new SRApp();
	app->Init("06_ShadowMap", width, height);
	app->SetMoveSpeed(MoveSpeed);
	app->Run(pipeline, meshs,  colorbuffer);
	app->Quit();

	delete app;
	stbi_image_free(colorbuffer);

	return 0;
}