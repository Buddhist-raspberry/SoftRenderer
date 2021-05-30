#ifndef __OUR_GL_H__
#define __OUR_GL_H__
#include "tgaimage.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

extern glm::mat4x4 ModelView;
extern glm::mat4x4 Projection;

void viewport(int x, int y, int w, int h);
void projection(float coeff=0.f); // coeff = -1/c
void lookat(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

struct IShader {
    virtual ~IShader();
    virtual glm::vec4 vertex(int iface, int nthvert) = 0;
    virtual bool fragment(glm::vec3 bar, TGAColor &color) = 0;
};

//void triangle(Vec4f *pts, IShader &shader, TGAImage &image, float *zbuffer);
void triangle(glm::mat3x4 &pts, IShader &shader, TGAImage &image, float *zbuffer);
#endif //__OUR_GL_H__

