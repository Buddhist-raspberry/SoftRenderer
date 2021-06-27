#ifndef __LIGHT_H__
#define __LIGHT_H__
#include "VecColor.h"

//灯光基类
class Light {
protected:
	glm::vec3 color;
	float indensity;
public:
	Light(glm::vec3 _color, float _indensity) :color(_color), indensity(_indensity) {}
	virtual glm::vec3 GetColor(const glm::vec3& pos) = 0;
	virtual glm::vec3 GetDirection(const glm::vec3& pos) { return glm::vec3(0); }
};

//环境光
class AmbientLight:public Light {
public:
	AmbientLight(glm::vec3 _color, float _indensity):Light(_color,_indensity) {}
	glm::vec3 GetColor(const glm::vec3& pos) override;
	glm::vec3 GetColor();
};

//平行光
class DirectionalLight :public Light {
private:
	glm::vec3 direction;
public:
	DirectionalLight(glm::vec3 _color, float _indensity,glm::vec3 _direction) :Light(_color, _indensity) 
	{
		direction = _direction;
	}
	glm::vec3 GetColor(const glm::vec3& pos) override;
	glm::vec3 GetDirection(const glm::vec3& pos) override;
};

//点光源
class PointLight :public Light {
protected:
	glm::vec3 position;
	float constant;
	float linear;
	float exp;
public:
	PointLight(glm::vec3 _color, float _indensity, glm::vec3 _position,
		float _constant,float _linear,float _exp) 
		:Light(_color, _indensity) {
		position = _position;
		constant = _constant;
		linear = _linear;
		exp = _exp;
	}
	glm::vec3 GetColor(const glm::vec3& pos) override;
	glm::vec3 GetDirection(const glm::vec3& pos) override;
};

//聚光灯
class SpotLight :public PointLight {
private:
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
	float epsilon;
public:
	SpotLight(glm::vec3 _color, float _indensity, glm::vec3 _position, glm::vec3 _direction,
		float _constant, float _linear, float _exp, float _cutOffAngle, float _outerCutOffAngle)
		:PointLight(_color, _indensity,_position,_constant,_linear,_exp) {
		direction = _direction;
		cutOff = glm::cos(glm::radians(_cutOffAngle));
		outerCutOff = glm::cos(glm::radians(_outerCutOffAngle));
		epsilon = cutOff - outerCutOff;
	}
	glm::vec3 GetColor(const glm::vec3& pos) override;

};

#endif // !__LIGHT_H__
