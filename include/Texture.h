#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include <glm/glm.hpp>

//2D������
class Texture2D {
private:
	unsigned char* data;
	int width;
	int height;
public:
	void loadTexture(const char* filaname);
	glm::vec4 sample(const glm::vec2& uv);   /*�������*/
	glm::vec4 sample(float u,float v);
};

class CubemapTexture {
private:
	unsigned char* data[6];
	const char* fileNames[6];
	int width;
	int height;
public:
	void loadTextures(const char* posXFilename, const char* negXFilename,
		const char* posYFilename, const char* negYFilename,
		const char* posZFilename, const char* negZFilename);
	void loadTexture(unsigned int index);
	glm::vec4 sample(float u, float v, unsigned index);
	glm::vec4 sample(const glm::vec2& uv, unsigned index);
	glm::vec4 sample3D(const glm::vec3& dir);
};

//���ͼ
class DepthMap {
private:
	unsigned char* data;
	int width;
	int height;
public:
	DepthMap(unsigned char* _data, int _width, int _height) :data(_data), width(_width), height(_height) {}
	glm::vec4 sample(glm::vec2& xy);
};
#endif //__TEXTURE_H__
