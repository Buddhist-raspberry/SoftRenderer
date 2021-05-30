#include <algorithm>
#include <limits>
#include <cstdlib>
#include <iostream>
#include "our_gl.h"

glm::mat4 ModelView;
glm::mat4 Viewport;
glm::mat4 Projection;

IShader::~IShader() {}



void viewport(int x, int y, int w, int h) {
    Viewport = glm::mat4(1.0f);
    Viewport[3][0] = x+w/2.f;
    Viewport[3][1] = y+h/2.f;
    Viewport[3][2] = 1.f;
    Viewport[0][0] = w/2.f;
    Viewport[1][1] = h/2.f;
    Viewport[2][2] = 0;
	std::cout<< "Viewport:\n" << glm::to_string(Viewport)<<std::endl;
}

void projection(float coeff) {
    Projection = glm::mat4(1.0f);
    Projection[2][3] = coeff;
	std::cout << "Projection:\n" << glm::to_string(Projection) << std::endl;
}

void lookat(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
	glm::vec3 z = glm::normalize(eye-center);
	glm::vec3 x = glm::normalize(glm::cross(up,z));
	glm::vec3 y = glm::normalize(glm::cross(z,x));
    glm::mat4x4 Minv = glm::mat4x4(1.0f);
	glm::mat4x4 Tr = glm::mat4x4(1.0f);
	Minv[0] = glm::vec4(x,0);
	Minv[1] = glm::vec4(y, 0);
	Minv[2] = glm::vec4(z, 0);
	Tr[3] = glm::vec4(-center,1.0f);
    ModelView = Minv*Tr;
	std::cout<<"ModelView:\n" << glm::to_string(ModelView) << std::endl;
}

glm::vec3 barycentric(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 P) {
	glm::vec3 s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
	glm::vec3 u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return glm::vec3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return glm::vec3(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(glm::mat3x4 &clipc, IShader &shader, TGAImage &image, float *zbuffer) {
	glm::mat4x3 pts  = glm::transpose(Viewport*clipc); // transposed to ease access to each of the points
	glm::mat2x3 pts2;


	for (int i = 0; i < 3; i++) {
		glm::vec4 pt = glm::row(pts, i);
		pt /= pt.w;
		pts2[0][i] = pt.x;
		pts2[1][i] = pt.y;
	}


	glm::vec2 bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
	glm::vec2 bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	glm::vec2 clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts2[j][i]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[j][i]));
        }
    }
	glm::ivec2 P;
    TGAColor color;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
			glm::vec3 bc_screen  = barycentric(glm::row(pts2,0),
				glm::row(pts2, 1), glm::row(pts2, 2), P);
			glm::vec3 bc_clip    = glm::vec3(bc_screen.x/pts[3][0], bc_screen.y/pts[3][1], bc_screen.z/pts[3][1]);
            bc_clip = bc_clip/(bc_clip.x+bc_clip.y+bc_clip.z);
            float frag_depth = glm::dot(glm::row(clipc,2),bc_clip);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0 || zbuffer[P.x+P.y*image.get_width()]>frag_depth) continue;
            bool discard = shader.fragment(bc_clip, color);
            if (!discard) {
                zbuffer[P.x+P.y*image.get_width()] = frag_depth;
                image.set(P.x, P.y, color);
            }
        }
    }
}

