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

glm::vec3       eye(0, 2, 4);       /*�����λ��*/
glm::vec3    center(0, 0, 0);		/*������������ĵ�*/
glm::vec3        up(0, 1, 0);		/*��������Ϸ���*/
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



	/*��ȡģ��*/

	/*��Ⱦ*/
	unsigned char * colorbuffer = new unsigned char[4 * width*height];



	std::vector<SRMesh*> meshs;
	{
		char* model_name = "../../obj/cube.obj";
		char* main_texture_name = "../../obj/transparent_texture.png";
		ShaderAlphaTest * shader = new ShaderAlphaTest();
		Texture2D* texture = new Texture2D();
		texture->loadTexture(main_texture_name);
		shader->mainTex = texture;
		scene = new SRScene();
		if (scene->ReadSceneFromFile(model_name)) {
			SRMesh* mesh = scene->GetMesh(0);
			mesh->SetShader(shader);
			meshs.push_back(mesh);
		}
		delete scene;
	}
	{
		char* model_name = "../../obj/floor.obj";
		ShaderPureColor * shader = new ShaderPureColor();
		shader->color = glm::vec4(0, 0, 1.0f, 0.5f);
		scene = new SRScene();
		if (scene->ReadSceneFromFile(model_name)) {
			SRMesh* mesh = scene->GetMesh(0);
			mesh->SetShader(shader);
			mesh->modelMatrix = glm::translate(glm::rotate( glm::mat4(1.0f),glm::radians(-90.0f),
				glm::vec3(1.0f,0,0)), glm::vec3(0, -0.5f, 0));
			meshs.push_back(mesh);
		}
		delete scene;
	}



	pipeline->ambient = new AmbientLight(VecColor::White, 0.4f);
	pipeline->AddLight(new DirectionalLight(VecColor::White,0.8f,glm::vec3(1.0f,1.0f,1.0f)));

	pipeline->SetBGColor(VecColor::LightSlateBlue);
	pipeline->setAlphaBlend(true);
	pipeline->Render(meshs, colorbuffer);

	/*����ΪͼƬ*/
	std::string result_name = "11_Transparent_AlphaBlend_Multi.png";
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);


	/*��ʾ�����ڳ���*/
	app = new SRApp();
	app->Init("11_Transparent", width, height);
	app->SetMoveSpeed(MoveSpeed);
	app->Run(pipeline, meshs, colorbuffer);
	app->Quit();

	delete app;
	stbi_image_free(colorbuffer);

	return 0;
}