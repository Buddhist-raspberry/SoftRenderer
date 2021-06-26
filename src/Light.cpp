#include "Light.h"


glm::vec3 AmbientLight::GetColorAtPos(const glm::vec3& pos) {
	return color * indensity;
}