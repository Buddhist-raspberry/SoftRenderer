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

glm::vec4 ShaderPhong::fragment(struct frag_in pixel) {
	glm::vec3& pos = pixel.worldPos;
	glm::vec3& normal = pixel.worldNormal;

	glm::vec3 ambient = Pipeline::getInstance()->ambient->GetColor();

	Light* light = Pipeline::getInstance()->GetLight(0);
	glm::vec3& lightDir = light->GetDirection(pos);
	glm::vec3& lightColor = light->GetColor(pos);

	glm::vec3 diffuse = lightColor * diffuseColor * glm::max(glm::dot(normal, lightDir), 0.0f);

	glm::vec3& cameraPos = Pipeline::getInstance()->GetCameraPos();
	glm::vec3 viewDir = glm::normalize( cameraPos - pos );
	glm::vec3 reflectDir = lightDir - normal * glm::dot(normal, lightDir)*glm::vec3(2.0f);
	glm::vec3 specular = lightColor * specularColor * glm::pow(glm::max(glm::dot(reflectDir, viewDir), 0.0f), gloss);

	return glm::vec4(ambient+diffuse+ specular, 1.0f);
}

glm::vec4 ShaderTexture::fragment(struct frag_in pixel) {
	glm::vec3& pos = pixel.worldPos;
	glm::vec3& normal = pixel.worldNormal;

	glm::vec3 ambient = Pipeline::getInstance()->ambient->GetColor();

	Light* light = Pipeline::getInstance()->GetLight(0);
	glm::vec3& lightDir = light->GetDirection(pos);
	glm::vec3& lightColor = light->GetColor(pos);

	glm::vec3 albedo = glm::vec3(mainTex->sample(pixel.uv))*mainColor;
	glm::vec3 diffuse = lightColor * albedo * glm::max(glm::dot(normal, lightDir), 0.0f);

	glm::vec3& cameraPos = Pipeline::getInstance()->GetCameraPos();
	glm::vec3 viewDir = glm::normalize(cameraPos - pos);
	glm::vec3 reflectDir = lightDir - normal * glm::dot(normal, lightDir)*glm::vec3(2.0f);
	glm::vec3 specular = lightColor * specularColor * glm::pow(glm::max(glm::dot(reflectDir, viewDir), 0.0f), gloss);

	return glm::vec4(ambient + diffuse + specular, 1.0f);
}

glm::vec4 ShaderBumpedNormal::fragment(struct frag_in pixel) {
	glm::vec4 color = mainTex->sample(pixel.uv);
	return color;
}