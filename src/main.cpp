#include "pipeline.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>

#include "veccolor.h"

const int width = 800;
const int height = 600;

glm::vec3 light_dir(1, 1, 1);
glm::vec3       eye(1, 1, 2);
glm::vec3    center(0, 0, 0);
glm::vec3        up(0, 1, 0);
glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

Pipeline* p = Pipeline::getInstance();

int main(int argc, char** argv) {
	if (2 > argc) {
		std::cerr << "Usage: " << argv[0] << "obj/model.obj" << std::endl;
		return 1;
	}
	p->SetModel(modelMatrix);
	p->SetView(eye, center, up);
	p->SetProjection(60.0f, width / height, 0.1f, 10.0f);
	p->SetViewport(0, 0, width, height);

	char* model_name = argv[1];
	//char* model_name = "../obj/boggie/body.obj";
	Model* model = new Model(model_name);

	//TGAImage *frame = new TGAImage(width, height, TGAImage::RGB);
	unsigned char * colorbuffer = new unsigned char[4 * width*height];

	ShaderUV* shader=new ShaderUV();

	p->SetBGColor(VecColor::LightSlateBlue);
	p->Render(model, shader, colorbuffer);
	std::string result_name = "uv.png";

	stbi_write_png(result_name.c_str(), width, height, 4, colorbuffer, 0);

	delete model;
	delete shader;
	//delete frame;
	stbi_image_free(colorbuffer);
	return 0;
}