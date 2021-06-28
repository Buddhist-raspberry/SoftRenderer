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

glm::vec3       eye(0, 4, 8);       /*摄像机位置*/
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

	/*渲染*/
	unsigned char * colorbuffer = new unsigned char[4 * width*height];

	std::vector<SRMesh*> meshs;

	{
		//地面
		char* model_name = "../../obj/floor.obj";

		scene = new SRScene();
		scene->ReadSceneFromFile(model_name);
		SRMesh* cube = scene->GetMesh(0);
		cube->modelMatrix = glm::scale(glm::mat4(1.0f),glm::vec3(2.0f, 1.0f, 2.0f));

		ShaderPhongMulti * shader = new ShaderPhongMulti();
		shader->diffuseColor = VecColor::White;
		shader->specularColor = VecColor::White;
		shader->gloss = 1.0f;
		shader->maxLightCount = 10;

		cube->SetShader(shader);
		meshs.push_back(cube);
	}
	//机器人
	{
	
		char* model_name = "../../obj/robot/robot.obj";
		char* mainTex_name = "../../obj/robot/Robot_Soldier_BaseColorW.png";
		char* normalTex_name = "../../obj/robot/Robot_Soldier_Normal.png";
		char* specularTex_name = "../../obj/robot/Robot_Soldier_Roughness.png";
		char* emissionTex_name = "../../obj/robot/Robot_Soldier_Emissive.png";

		Texture2D* mainTex = new Texture2D();
		mainTex->loadTexture(mainTex_name);
		Texture2D* normalTex = new Texture2D();
		normalTex->loadTexture(normalTex_name);
		Texture2D* specularTex = new Texture2D();
		specularTex->loadTexture(specularTex_name);
		Texture2D* emissionTex = new Texture2D();
		emissionTex->loadTexture(emissionTex_name);

		ShaderAll* shader = new ShaderAll();
		shader->mainTex = mainTex;
		shader->specularTex = specularTex;
		shader->normalTex = normalTex;
		shader->emissionTex = emissionTex;
		shader->mainColor = VecColor::White;
		shader->specularColor = VecColor::White;
		shader->gloss = 5.0f;

		scene = new SRScene();
		SRMesh* mesh;
		if (scene->ReadSceneFromFile(model_name)) {
			mesh = scene->GetMesh(0);
		}

		mesh->modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f),
			glm::vec3(2.0f, 2.0f, 2.0f)), glm::vec3(0, -0.5f, 0));
		mesh->SetShader(shader);
		meshs.push_back(mesh);
	}

	//文字
	{
		char* model_name = "../../obj/title.obj";

		ShaderPureColor* shader = new ShaderPureColor();
		shader->color = VecColor::LightSlateBlue;
		shader->color.a = 0.5f;
		scene = new SRScene();
		SRMesh* mesh;
		if (scene->ReadSceneFromFile(model_name)) {
			mesh = scene->GetMesh(0);
		}

		mesh->modelMatrix = glm::translate(glm::rotate( glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
			glm::vec3(1.0f, 0, 0)),glm::radians(180.0f)  ,glm::vec3(0,0,1.0f) ), glm::vec3(0, -3.0f,0) );
		mesh->SetShader(shader);
		meshs.push_back(mesh);
	}
	{
		//红色光源
		char* model_name = "../../obj/cube.obj";

		scene = new SRScene();
		scene->ReadSceneFromFile(model_name);
		SRMesh* cube = scene->GetMesh(0);
		cube->modelMatrix = glm::scale( glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 2.0f, 0)),
			glm::vec3(0.2f, 0.2f, 0.2f));
		ShaderPureColor* shader = new ShaderPureColor();
		shader->color = VecColor::Red;

		cube->SetShader(shader);
		meshs.push_back(cube);

	}
	{
		//蓝色光源
		char* model_name = "../../obj/cube.obj";

		scene = new SRScene();
		scene->ReadSceneFromFile(model_name);
		SRMesh* cube = scene->GetMesh(0);
		cube->modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 2.0f, 0)),
			glm::vec3(0.2f, 0.2f, 0.2f));
		ShaderPureColor* shader = new ShaderPureColor();
		shader->color = VecColor::Blue;

		cube->SetShader(shader);
		meshs.push_back(cube);

	}

	pipeline->ambient = new AmbientLight(VecColor::White, 0.2f);
	pipeline->AddLight(new DirectionalLight(VecColor::Red, 0.6f, glm::vec3(1, 1, 1)));
	pipeline->AddLight(new DirectionalLight(VecColor::Blue, 0.6f, glm::vec3(-1, 1, 1)));
	pipeline->AddLight(new SpotLight(VecColor::LightSkyBlue, 10.0f, glm::vec3(0, -2.0f, 0),
		glm::vec3(0, 1.0f, 0), 1.0f, 0.7f, 1.8f, 30.0f, 45.0f));

	pipeline->SetBGColor(VecColor::Black);
	pipeline->setAlphaBlend(true);
	pipeline->setBloom(true, 2.0f, 130.0f);

	pipeline->Render(meshs,colorbuffer);





	/*保存为图片*/
	std::string result_name = "13_GeneralScene.png";

	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);


	/*显示到窗口程序*/
	app = new SRApp();
	app->Init("13_GeneralScene", width, height);
	app->SetMoveSpeed(MoveSpeed);
	app->Run(pipeline, meshs, colorbuffer);
	app->Quit();

	delete app;
	delete scene;
	stbi_image_free(colorbuffer);

	return 0;
}