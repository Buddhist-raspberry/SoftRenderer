#ifndef __SHADER_H__
#define __SHADER_H__
#include "Texture.h"


//顶点着色器的输入
struct vert_in {
	glm::vec3 vert;		//顶点坐标
	glm::vec2 uv;		//顶点UV
	glm::vec3 normal;	//顶点法线
	glm::vec3 tangent;  //顶点切线
	vert_in() {}
	vert_in(glm::vec3 _vert, glm::vec2 _uv, glm::vec3 _normal, glm::vec3 _tangent) {
		vert = _vert;
		uv = _uv;
		normal = _normal;
		tangent = _tangent;
	}
};

//顶点着色器的输出
struct vert_out {
	glm::vec4 clipPos;	//裁剪空间坐标
	glm::vec3 worldNormal; //世界空间法线
	glm::vec3 worldTangent; //世界空间切线
	glm::vec3 worldPos;		//世界空间坐标
	glm::vec2 uv;
	vert_out() {}
	vert_out(glm::vec3 _worldNormal, glm::vec3 _worldTangent , glm::vec3 _worldPos, glm::vec2 _uv) {
		worldNormal = _worldNormal;
		worldTangent = _worldTangent;
		worldPos = _worldPos;
		uv = _uv;
	}
};

//片元着色器的输入
struct frag_in {
	glm::vec3 worldNormal; //世界空间法线
	glm::vec3 worldTangent; //世界空间切线
	glm::vec3 worldPos;		//世界空间坐标
	glm::vec2 uv;
	frag_in() {}
	frag_in(glm::vec3 _worldNormal, glm::vec3 _worldTangent , glm::vec3 _worldPos, glm::vec2 _uv) {
		worldNormal = _worldNormal;
		worldNormal = _worldTangent;
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

//Phong光照模型着色器
class ShaderPhong :public ShaderBase {
public:
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float gloss;
	glm::vec4 fragment(struct frag_in pixel) override;
};

//基本纹理着色器
class ShaderTexture:public ShaderBase {
public:
	Texture2D* mainTex;
	glm::vec3 mainColor;
	glm::vec3 specularColor;
	float gloss;
	glm::vec4 fragment(struct frag_in pixel) override;
};

//纹理+法线着色器
class ShaderBumpedNormal:public ShaderBase {
public:
	Texture2D* mainTex;
	Texture2D* normalTex;
	glm::vec4 fragment(struct frag_in pixel) override;
};

#endif /*__SHADER_H__*/