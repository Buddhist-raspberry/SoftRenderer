#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>

#include <glm/gtc/matrix_transform.hpp>

#include "SRApp.h"

#define min(x,y) (((x)<(y))?(x):(y))

const int width = 800;
const int height = 600;

glm::vec3       eye(0, 4, 8);       /*摄像机位置*/
glm::vec3    center(0, 0, 0);		/*摄像机朝向中心点*/
glm::vec3        up(0, 1, 0);		/*摄像机向上方向*/
glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

const float MoveSpeed = 0.5f;

Pipeline* pipeline = Pipeline::getInstance();

SRScene* scene;
SRApp* app;


unsigned char* threshold(unsigned char cb[], float T)
{
	const float weight[3] = { 0.2126f, 0.7152f, 0.0722f };

	const int size = 4 * width * height;
	unsigned char* tb = new unsigned char[size]();
	for (int i = 0; i < size; i += 4)
	{
		float r = (float)cb[i] * weight[0];
		float g = (float)cb[i + 1] * weight[1];
		float b = (float)cb[i + 2] * weight[2];
		if (r + g + b > T)
		{
			tb[i] = cb[i];
			tb[i + 1] = cb[i + 1];
			tb[i + 2] = cb[i + 2];
			tb[i + 3] = cb[i + 3];
		}
		else
		{
			/*printf("%f\n", r + b + g);*/
		}
	}
	return tb;
}

void bloom(unsigned char cb[],float indensity=1.0f,float T=10.0f)
{
	const float weight[5] = { 0.4026f, 0.2442f, 0.2442f,  0.0545f, 0.0545f };

	unsigned char* tb = threshold(cb,T);
	unsigned int size = 4 * width * height;
	float* fcb = new float[size];
	for (unsigned int i = 0; i < size; ++i)
	{
		fcb[i] = (float)cb[i];
	}


	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			const int lines_before = -1 * min(4, i);
			const int lines_after = min(4, height - i - 1);
			const int col_before = -1 * min(4, j);
			const int col_after = min(4, width - j - 1);

			float increment_r = 0.0f;
			float increment_g = 0.0f;
			float increment_b = 0.0f;

			for (int k = lines_before; k <= lines_after; ++k)
			{
				for (int m = col_before; m <= col_after; ++m)
				{
					const int coordinate = ((i + k) * width + (j + m)) * 4;
					if (tb[coordinate] > 0 || tb[coordinate + 1] > 0 || tb[coordinate + 2] > 0)
					{
						//printf("%d  %f  %f  %d  %d\n", tb[coordinate], (float)tb[coordinate] * weight[abs(k)] * weight[abs(m)], weight[abs(k)] * weight[abs(m)], k, m);
						increment_r += (float)tb[coordinate] * weight[abs(k)] * weight[abs(m)];
						increment_g += (float)tb[coordinate + 1] * weight[abs(k)] * weight[abs(m)];
						increment_b += (float)tb[coordinate + 2] * weight[abs(k)] * weight[abs(m)];
					}
				}
			}
			const int coordinate = (i * width + j) * 4;
			fcb[coordinate] = cb[coordinate] + indensity* increment_r;
			fcb[coordinate + 1] = cb[coordinate+1] + indensity * increment_g;
			fcb[coordinate + 2] = cb[coordinate+2] + indensity * increment_b;
		}
	}
	for (unsigned int i = 0; i < size; ++i)
	{
		cb[i] = (unsigned char)min(255.0f, roundf(fcb[i]));
	}
	delete[]tb;
	delete[]fcb;
}


int main(int argc, char** argv) {



	pipeline->SetModel(modelMatrix);
	pipeline->SetView(eye, center, up);
	pipeline->SetProjection(60.0f, width / height, 0.1f, 10.0f);
	pipeline->SetViewport(0, 0, width, height);



	/*渲染*/
	unsigned char * colorbuffer = new unsigned char[4 * width*height];
	char* model_name = "../../obj/cube.obj";

	std::vector<SRMesh*> meshs;
	{
		scene = new SRScene();
		SRMesh* mesh;
		if (scene->ReadSceneFromFile(model_name)) {
			mesh = scene->GetMesh(0);
		}


		ShaderPureColor* shader = new ShaderPureColor();
		shader->color = VecColor::Green*0.7f;
		mesh->mShader = shader;
		mesh->modelMatrix = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),
			glm::vec3(1.0f, 0, 0)), glm::vec3(0, 0, -5.0f));
		meshs.push_back(mesh);
	}
	{
		scene = new SRScene();
		SRMesh* mesh;
		if (scene->ReadSceneFromFile(model_name)) {
			mesh = scene->GetMesh(0);
		}

		ShaderPureColor* shader = new ShaderPureColor();
		shader->color = VecColor::Blue*0.3f;
		mesh->mShader = shader;
		mesh->modelMatrix = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),
			glm::vec3(1.0f, 0, 0)), glm::vec3(3.0f, 0, -3.0f));
		meshs.push_back(mesh);
	}
	{
		scene = new SRScene();
		SRMesh* mesh;
		if (scene->ReadSceneFromFile(model_name)) {
			mesh = scene->GetMesh(0);
		}

		ShaderPureColor* shader = new ShaderPureColor();
		shader->color = VecColor::Red*0.8f;
		mesh->mShader = shader;
		mesh->modelMatrix = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),
			glm::vec3(1.0f, 0, 0)), glm::vec3(-3.0f, 0, -4.0f));
		meshs.push_back(mesh);
	}



	pipeline->SetBGColor(VecColor::LightSlateBlue);

	pipeline->ambient = new AmbientLight(VecColor::White, 0.1f);
	pipeline->AddLight(new DirectionalLight(VecColor::White, 0.2f, glm::vec3(1, 1, 1)));

	pipeline->SetBGColor(VecColor::Black);
	pipeline->Render(meshs,colorbuffer);


	std::string result_name = "07_Bloom_Before.png";
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);

	bloom(colorbuffer,4.0f,10.0f);


	/*保存为图片*/
	result_name = "07_Bloom_After.png";
	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);


	/*显示到窗口程序*/
	app = new SRApp();
	app->Init("07_Bloom", width, height);
	app->SetMoveSpeed(MoveSpeed);
	app->Run(pipeline, meshs, colorbuffer);
	app->Quit();

	delete app;
	delete scene;
	stbi_image_free(colorbuffer);

	return 0;
}