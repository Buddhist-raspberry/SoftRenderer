#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include <glm/glm.hpp>

//2D纹理类
class Texture2D {
private:
	unsigned char* data;
	int width;
	int height;
public:
	void loadTexture(const char* filaname);
	glm::vec4 sample(const glm::vec2& uv);   /*纹理采样*/
	glm::vec4 sample(float u,float v);
};

#endif //__TEXTURE_H__
