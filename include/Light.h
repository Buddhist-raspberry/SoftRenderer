#ifndef __LIGHT_H__
#define __LIGHT_H__
#include "VecColor.h"

//�ƹ����
class Light {
protected:
	glm::vec3 color;
	float indensity;
public:
	Light(glm::vec3 _color, float _indensity) :color(_color), indensity(_indensity) {}
	virtual glm::vec3 GetColor(const glm::vec3& pos) = 0;
};

//������
class AmbientLight:public Light {
public:
	AmbientLight(glm::vec3 _color, float _indensity):Light(_color,_indensity) {}
	glm::vec3 GetColor(const glm::vec3& pos) override;
};

//ƽ�й�
class DirectionalLight :public Light {
private:
	glm::vec3 direction;
public:
	DirectionalLight(glm::vec3 _color, float _indensity,glm::vec3 _direction) :Light(_color, _indensity) 
	{
		direction = _direction;
	}
	glm::vec3 GetColor(const glm::vec3& pos) override;
	glm::vec3 GetDirection(const glm::vec3& pos);
};


#endif // !__LIGHT_H__
