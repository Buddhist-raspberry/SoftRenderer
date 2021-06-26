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
	virtual glm::vec3 GetColorAtPos(const glm::vec3& pos) = 0;
};

//环境光
class AmbientLight:public Light {
public:
	AmbientLight(glm::vec3 _color, float _indensity):Light(_color,_indensity) {}
	glm::vec3 GetColorAtPos(const glm::vec3& pos) override;
};


#endif // !__LIGHT_H__
