#include "Texture.h"
#include "VecColor.h"

#include <stb_image/stb_image.h>
#include <string.h>
#include <iostream>

void Texture2D::loadTexture(const char* filename) {
	int n;
	unsigned char* tmp = stbi_load(filename, &this->width, &this->height, &n, 0);
	data = new unsigned char[4 * width*height];
	for (int i = 0; i < width*height; i++) {
		if (n == 1) {
			memset(data + 4 * i, tmp[i], sizeof(unsigned char) * 4);
		}
		else if (n == 3) {
			memcpy(data + 4 * i, tmp + 3 * i, sizeof(unsigned char)*3);
			data[4*i+3] = 255;
		}
		else if (n == 4) {
			memcpy(data + 4 * i, tmp + 4 * i, sizeof(unsigned char)*4);
		}
	}
	stbi_image_free(tmp);
	std::cout << "Load Texture From \"" << filename << "\" Successfully!\n";
}

glm::vec4 Texture2D::sample(float u, float v){
	int x = (int)(u * width - 0.5f) % width;
	int y = (int)(v * height - 0.5f) % height;
	x = x < 0 ? width + x : x;
	y = y < 0 ? height + y : y;
	int index =x + (height - 1 - y) * width - 1;
	if (index >= width * height|| index<0)
		return VecColor::Black;
	return VecColor::color_256_to_vec(data + 4 * index);
}

glm::vec4 Texture2D::sample(const glm::vec2& uv) {
   return sample(uv.x, uv.y);
}


void CubemapTexture::loadTextures(const char* posXFilename, const char* negXFilename,
	const char* posYFilename, const char* negYFilename,
	const char* posZFilename, const char* negZFilename) {

	fileNames[0] = posXFilename;
	fileNames[1] = negXFilename;
	fileNames[2] = posYFilename;
	fileNames[3] = negYFilename;
	fileNames[4] = posZFilename;
	fileNames[5] = negZFilename;

	for (unsigned int i = 0; i < 6; i++) {
		loadTexture(i);
	}


}

void CubemapTexture::loadTexture(unsigned int index) {
	int n;
	unsigned char* tmp = stbi_load(fileNames[index], &this->width, &this->height, &n, 0);
	data[index] = new unsigned char[4 * width*height];
	for (int i = 0; i < width*height; i++) {
		if (n == 1) {
			memset(data[index] + 4 * i, tmp[i], sizeof(unsigned char) * 4);
		}
		else if (n == 3) {
			memcpy(data[index] + 4 * i, tmp + 3 * i, sizeof(unsigned char) * 3);
			data[index][4 * i + 3] = 255;
		}
		else if (n == 4) {
			memcpy(data[index] + 4 * i, tmp + 4 * i, sizeof(unsigned char) * 4);
		}
	}
	stbi_image_free(tmp);
	std::cout << "Load Texture From \"" << fileNames[index] << "\" Successfully!\n";
}


glm::vec4 CubemapTexture::sample(float u, float v, unsigned index) {
	int x = (int)(u * width - 0.5f) % width;
	int y = (int)(v * height - 0.5f) % height;
	x = x < 0 ? width + x : x;
	y = y < 0 ? height + y : y;
	return VecColor::color_256_to_vec(data[index] + 4 * (x + (height - 1 - y) * width - 1));
}
glm::vec4 CubemapTexture::sample(const glm::vec2& uv, unsigned index) {
	return sample(uv.x, uv.y, index);

}

glm::vec4 CubemapTexture::sample3D(const glm::vec3& dir) {
	//TODO: cube映射为球体->球体求交->采样
	return glm::vec4(0);
}

glm::vec4 DepthMap::sample(glm::vec2& xy) {
	int index = int(xy.y) + int(xy.x)*width;
	if (index >= width * height||index < 0)
		return VecColor::White;
	return VecColor::color_256_to_vec(data + 4 *index);
}