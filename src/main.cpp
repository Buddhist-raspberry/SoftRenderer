#include <vector>
#include <limits>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <string>
#include "tgaimage.h"
#include "model.h"
#include "our_gl.h"
Model *model        = NULL;

const int width  = 800;
const int height = 800;

glm::vec3 light_dir(1,1,1);
glm::vec3       eye(1, 1, 3);
glm::vec3    center(0,0,0);
glm::vec3        up(0,1,0);

struct Shader : public IShader {
    glm::mat3x2 varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
	glm::mat3x4 varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
	glm::mat3x3 varying_nrm; // normal per vertex to be interpolated by FS
	glm::mat3x3 ndc_tri;     // triangle in normalized device coordinates

    virtual glm::vec4 vertex(int iface, int nthvert) {
        varying_uv[nthvert] = model->uv(iface, nthvert);
        varying_nrm[nthvert] =glm::vec3(glm::transpose(glm::inverse(Projection*ModelView))*glm::vec4(model->normal(iface, nthvert), 0.f));
		glm::vec4 gl_Vertex = Projection*ModelView*glm::vec4(model->vert(iface, nthvert),1.0f);
        varying_tri[nthvert] = gl_Vertex;
        ndc_tri[nthvert] = glm::vec3(gl_Vertex/gl_Vertex[3]);
        return gl_Vertex;
    }

    virtual bool fragment(glm::vec3 bar, TGAColor &color) {
		glm::vec3 bn = glm::normalize(varying_nrm*bar);
		glm::vec2 uv = varying_uv*bar;

		glm::vec3 n = glm::normalize(model->normal(uv));

        float diff = std::max(0.f, glm::dot(n,light_dir));
        //color = model->diffuse(uv)*diff*5;
		color = TGAColor( (int)(uv.x*255), (int)(uv.y*255), 0, 255);

        return false;
    }
};

void render(Model* model,Shader& shader, float* zbuffer, TGAImage& frame) {
	for (int i = 0; i < model->nfaces(); i++) {
		for (int j = 0; j < 3; j++) {
			shader.vertex(i, j);
		}
		triangle(shader.varying_tri, shader, frame, zbuffer);
	}

}

void clear(float* zbuffer,TGAImage& frame) {
	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
	frame.clear();
}

int main(int argc, char** argv) {
	if (2 > argc) {
		std::cerr << "Usage: " << argv[0] << "obj/model.obj" << std::endl;
		return 1;
	}

	//char* model_name = "../obj/african_head/african_head.obj";
	char* model_name = argv[1];

    float *zbuffer = new float[width*height];
	TGAImage frame(width, height, TGAImage::RGB);


    lookat(eye, center, up);
    viewport(width/8, height/8, width*3/4, height*3/4);
    projection(-1.f/(eye-center).length());
    light_dir = glm::normalize( glm::vec3((Projection*ModelView*glm::vec4(light_dir, 0.f))) );

    model = new Model(model_name);
    Shader shader;

	for (int i = 0; i < 4; i++) {
		clear(zbuffer, frame);
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, glm::radians(90.0f),up);
		eye = trans * glm::vec4(eye,1.0f);
		lookat(eye, center, up);

		render(model, shader,zbuffer,frame);
		frame.flip_vertically();
		std::string result_name = "diffuse_" + std::to_string(i) +".tga";
		frame.write_tga_file(result_name.c_str());

	}


	delete model;
    delete [] zbuffer;
    return 0;
}

