#include "VecColor.h"
#include <iostream>

namespace VecColor {
	glm::vec4 color_256_to_vec(unsigned char* color) {
		glm::vec4 c=glm::vec4(0);
		for (int i = 0; i < 4; i++) {
			c[i] = color[i] / 256.0f;
		}
		return c;
	}
	unsigned char* color_vec_to_256(const glm::vec4& color) {
		unsigned char* c;
		for (int i = 0; i < 4; i++) {
			c[i] = (unsigned int)(color[i] * 255.0f);
		}
		return c;
	}
};