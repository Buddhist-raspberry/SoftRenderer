#ifndef __SHADER_H__
#define __SHADER_H__
#include <glm/glm.hpp>


//������ɫ��������
struct vert_in {
	glm::vec3 vert;		//��������
	glm::vec2 uv;		//����UV
	glm::vec3 normal;	//���㷨��
	vert_in() {}
	vert_in(glm::vec3 _vert, glm::vec2 _uv, glm::vec3 _normal) {
		vert = _vert;
		uv = _uv;
		normal = _normal;
	}
};

//������ɫ�������
struct vert_out {
	glm::vec4 clipPos;	//�ü��ռ�����
	glm::vec3 worldNormal; //����ռ䷨��
	glm::vec3 worldPos;		//����ռ�����
	glm::vec2 uv;
	vert_out() {}
	vert_out(glm::vec3 _worldNormal, glm::vec3 _worldPos, glm::vec2 _uv) {
		worldNormal = _worldNormal;
		worldPos = _worldPos;
		uv = _uv;
	}
};

//ƬԪ��ɫ��������
struct frag_in {
	glm::vec3 worldNormal; //����ռ䷨��
	glm::vec3 worldPos;		//����ռ�����
	glm::vec2 uv;
	frag_in() {}
	frag_in(glm::vec3 _worldNormal, glm::vec3 _worldPos, glm::vec2 _uv) {
		worldNormal = _worldNormal;
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

#endif /*__SHADER_H__*/