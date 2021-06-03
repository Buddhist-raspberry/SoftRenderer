#ifndef __SHADER_H__
#define __SHADER_H__
#include <glm/glm.hpp>


//顶点着色器的输入
struct vert_in {
	glm::vec3 vert;		//顶点坐标
	glm::vec2 uv;		//顶点UV
	glm::vec3 normal;	//顶点法线
	vert_in() {}
	vert_in(glm::vec3 _vert, glm::vec2 _uv, glm::vec3 _normal) {
		vert = _vert;
		uv = _uv;
		normal = _normal;
	}
};

//顶点着色器的输出
struct vert_out {
	glm::vec4 clipPos;	//裁剪空间坐标
	glm::vec3 worldNormal; //世界空间法线
	glm::vec3 worldPos;		//世界空间坐标
	glm::vec2 uv;
	vert_out() {}
	vert_out(glm::vec3 _worldNormal, glm::vec3 _worldPos, glm::vec2 _uv) {
		worldNormal = _worldNormal;
		worldPos = _worldPos;
		uv = _uv;
	}
};

//片元着色器的输入
struct frag_in {
	glm::vec3 worldNormal; //世界空间法线
	glm::vec3 worldPos;		//世界空间坐标
	glm::vec2 uv;
	frag_in() {}
	frag_in(glm::vec3 _worldNormal, glm::vec3 _worldPos, glm::vec2 _uv) {
		worldNormal = _worldNormal;
		worldPos = _worldPos;
		uv = _uv;
	}
};


//着色器接口
class IShader {
public:
	virtual struct vert_out vertex(struct vert_in vert) = 0;
	virtual glm::vec4 fragment(struct frag_in pixel) = 0;
};

//着色器基类
class ShaderBase :public IShader {
public:
	struct vert_out vertex(struct vert_in vert) override;
};

//纯色着色器
class ShaderPureColor :public ShaderBase {
public:
	glm::vec4 color;
	glm::vec4 fragment(struct frag_in pixel) override;
};

//UV着色器
class ShaderUV:public ShaderBase {
public:
	glm::vec4 fragment(struct frag_in pixel) override;
};

#endif /*__SHADER_H__*/