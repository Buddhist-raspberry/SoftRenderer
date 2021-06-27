#ifndef __SHADER_H__
#define __SHADER_H__
#include "Texture.h"


//������ɫ��������
struct vert_in {
	glm::vec3 vert;		//��������
	glm::vec2 uv;		//����UV
	glm::vec3 normal;	//���㷨��
	glm::vec3 tangent;  //��������
	vert_in() {}
	vert_in(glm::vec3 _vert, glm::vec2 _uv, glm::vec3 _normal, glm::vec3 _tangent) {
		vert = _vert;
		uv = _uv;
		normal = _normal;
		tangent = _tangent;
	}
};

//������ɫ�������
struct vert_out {
	glm::vec4 clipPos;	//�ü��ռ�����
	glm::vec3 worldNormal; //����ռ䷨��
	glm::vec3 worldTangent; //����ռ�����
	glm::vec3 worldPos;		//����ռ�����
	glm::vec2 uv;
	vert_out() {}
	vert_out(glm::vec3 _worldNormal, glm::vec3 _worldTangent , glm::vec3 _worldPos, glm::vec2 _uv) {
		worldNormal = _worldNormal;
		worldTangent = _worldTangent;
		worldPos = _worldPos;
		uv = _uv;
	}
};

//ƬԪ��ɫ��������
struct frag_in {
	glm::vec3 worldNormal; //����ռ䷨��
	glm::vec3 worldTangent; //����ռ�����
	glm::vec3 worldPos;		//����ռ�����
	glm::vec2 uv;
	frag_in() {}
	frag_in(glm::vec3 _worldNormal, glm::vec3 _worldTangent , glm::vec3 _worldPos, glm::vec2 _uv) {
		worldNormal = _worldNormal;
		worldNormal = _worldTangent;
		worldPos = _worldPos;
		uv = _uv;
	}
};


//��ɫ���ӿ�
class IShader {
public:
	virtual struct vert_out vertex(struct vert_in vert) = 0;
	virtual glm::vec4 fragment(struct frag_in pixel) = 0;
};

//��ɫ������
class ShaderBase :public IShader {
public:
	struct vert_out vertex(struct vert_in vert) override;
};

//��ɫ��ɫ��
class ShaderPureColor :public ShaderBase {
public:
	glm::vec4 color;
	glm::vec4 fragment(struct frag_in pixel) override;
};

//UV��ɫ��
class ShaderUV:public ShaderBase {
public:
	glm::vec4 fragment(struct frag_in pixel) override;
};

//Phong����ģ����ɫ��
class ShaderPhong :public ShaderBase {
public:
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float gloss;
	glm::vec4 fragment(struct frag_in pixel) override;
};

//����������ɫ��
class ShaderTexture:public ShaderBase {
public:
	Texture2D* mainTex;
	glm::vec3 mainColor;
	glm::vec3 specularColor;
	float gloss;
	glm::vec4 fragment(struct frag_in pixel) override;
};

//����+������ɫ��
class ShaderBumpedNormal:public ShaderBase {
public:
	Texture2D* mainTex;
	Texture2D* normalTex;
	glm::vec4 fragment(struct frag_in pixel) override;
};

#endif /*__SHADER_H__*/