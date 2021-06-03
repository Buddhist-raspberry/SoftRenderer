#ifndef __VEC_COLOR_H__
#define __VEC_COLOR_H__
#include <glm/glm.hpp>
namespace VecColor {
	const glm::vec4 White(1.0f, 1.0f, 1.0f, 1.0f);
	const glm::vec4 Black(0, 0, 0, 1.0f);
	const glm::vec4 Gray(0.5f, 0.5f, 0.5f, 1.0f);
	const glm::vec4 Red(1.0f, 0, 0, 1.0f);
	const glm::vec4 Green(0, 1.0f, 0, 1.0f);
	const glm::vec4 Blue(0, 0, 1.0f, 1.0f);
	const glm::vec4 LightSkyBlue(0.6f, 0.9f, 1.0f, 1.0f);
	const glm::vec4 LightSlateBlue(0.5f, 0.5f, 1.0f, 1.0f);
};
//TODO:1.颜色混合模式 2.十六进制颜色 

#endif //__VEC_COLOR_H__
