#include "Shader.h"
#include "Pipeline.h"


struct vert_out ShaderBase::vertex(struct vert_in vert) {
	struct vert_out o;
	o.clipPos = Pipeline::getInstance()->ObjectToClipPos(vert.vert);
	o.worldPos = Pipeline::getInstance()->ObjectToWorldPos(vert.vert);
	o.worldNormal = Pipeline::getInstance()->ObjectToWorldDir(vert.normal);
	o.worldTangent = Pipeline::getInstance()->ObjectToWorldDir(vert.tangent);
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

glm::vec4 ShaderTexture::fragment(struct frag_in pixel) {
	glm::vec4 color = mainTex->sample(pixel.uv);
	return color;
}

glm::vec4 ShaderBumpedNormal::fragment(struct frag_in pixel) {
	glm::vec4 color = mainTex->sample(pixel.uv);
	return color;
}