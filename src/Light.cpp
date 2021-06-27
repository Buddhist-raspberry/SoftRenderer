#include "Light.h"


glm::vec3 AmbientLight::GetColor(const glm::vec3& pos) {
	return GetColor();
}

glm::vec3 AmbientLight::GetColor() {
	return color * indensity;
}


glm::vec3 DirectionalLight::GetColor(const glm::vec3& pos) {
	return color * indensity;
}

glm::vec3 DirectionalLight::GetDirection(const glm::vec3& pos) {
	return direction;
}

glm::vec3 PointLight::GetColor(const glm::vec3& pos) {
	float distance = (position - pos).length();
	float I = indensity / (constant + linear * distance + exp * glm::pow(distance, 2));
	return color * I;
}

glm::vec3 PointLight::GetDirection(const glm::vec3& pos) {
	return position-pos;
}

glm::vec3 SpotLight::GetColor(const glm::vec3& pos) {
	glm::vec3 lightDir = glm::normalize(position - pos);
	float theta = glm::dot(lightDir, glm::normalize(-direction));
	float epsilon = cutOff - outerCutOff;
	float I = glm::clamp( (theta - outerCutOff) / epsilon, 0.0f, 1.0f);
	glm::vec3 c = PointLight::GetColor(pos);
	return I * c;
}
