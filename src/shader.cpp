#include "shader.h"
#include "pipeline.h"


struct vert_out ShaderBase::vertex(struct vert_in vert) {
	struct vert_out o;
	o.clipPos = Pipeline::getInstance()->ObjectToClipPos(vert.vert);
	o.worldPos = Pipeline::getInstance()->ObjectToWorldPos(vert.vert);
	o.worldNormal = Pipeline::getInstance()->ObjectToWorldDir(vert.normal);
	o.uv = vert.uv;
	return o;
}
glm::vec4 ShaderPureColor::fragment(struct frag_in pixel) {
	return color;
}

glm::vec4 ShaderUV::fragment(struct frag_in pixel) {
	glm::vec4 color = glm::vec4(pixel.uv.x, pixel.uv.y, 0, 1.0f);
	return color;
}
