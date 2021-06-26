#include "Light.h"


glm::vec3 AmbientLight::GetColor(const glm::vec3& pos) {
	return color * indensity;
}

glm::vec3 DirectionalLight::GetColor(const glm::vec3& pos) {
	return color * indensity;
}

glm::vec3 DirectionalLight::GetDirection(const glm::vec3& pos) {
	return direction;
}