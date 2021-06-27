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
	glm::vec3& lightDir = glm::normalize(light->GetDirection(pos));
	glm::vec3& lightColor = light->GetColor(pos);

	glm::vec3 diffuse = lightColor * diffuseColor * glm::clamp(glm::dot(normal, lightDir), 0.0f,1.0f);

	glm::vec3& cameraPos = Pipeline::getInstance()->GetCameraPos();
	glm::vec3 viewDir = glm::normalize( cameraPos - pos );
	glm::vec3 reflectDir = glm::normalize( glm::reflect(-lightDir,normal) );
	glm::vec3 specular = lightColor * specularColor * glm::pow(glm::clamp(glm::dot(reflectDir, viewDir), 0.0f,1.0f), gloss);
	return glm::vec4(ambient+diffuse+ specular, 1.0f);
}

glm::vec4 ShaderTexture::fragment(struct frag_in pixel) {
	glm::vec3& pos = pixel.worldPos;
	glm::vec3& normal = pixel.worldNormal;

	glm::vec3 ambient = Pipeline::getInstance()->ambient->GetColor();

	Light* light = Pipeline::getInstance()->GetLight(0);
	glm::vec3& lightDir = glm::normalize(light->GetDirection(pos));
	glm::vec3& lightColor = light->GetColor(pos);

	glm::vec3 albedo = glm::vec3(mainTex->sample(pixel.uv))*mainColor;
	glm::vec3 diffuse = lightColor * albedo * glm::clamp(glm::dot(normal, lightDir), 0.0f, 1.0f);

	glm::vec3& cameraPos = Pipeline::getInstance()->GetCameraPos();
	glm::vec3 viewDir = glm::normalize(cameraPos - pos);
	glm::vec3 reflectDir = glm::normalize(glm::reflect(-lightDir, normal));
	glm::vec3 specular = lightColor * specularColor * glm::pow(glm::clamp(glm::dot(reflectDir, viewDir), 0.0f, 1.0f), gloss);

	return glm::vec4(ambient + diffuse + specular, 1.0f);
}

glm::vec4 ShaderBumpedNormal::fragment(struct frag_in pixel) {
	glm::vec3& pos = pixel.worldPos;
	glm::vec3& normalSample = glm::vec3(normalTex->sample(pixel.uv.x, pixel.uv.y));
	glm::vec3& normal = calcBumpedNormal(pixel.worldNormal, pixel.worldTangent, normalSample);

	glm::vec3 ambient = Pipeline::getInstance()->ambient->GetColor();

	Light* light = Pipeline::getInstance()->GetLight(0);
	glm::vec3& lightDir = glm::normalize(light->GetDirection(pos));
	glm::vec3& lightColor = light->GetColor(pos);

	glm::vec3 albedo = glm::vec3(mainTex->sample(pixel.uv))*mainColor;
	glm::vec3 diffuse = lightColor * albedo * glm::clamp(glm::dot(normal, lightDir), 0.0f, 1.0f);

	glm::vec3& cameraPos = Pipeline::getInstance()->GetCameraPos();
	glm::vec3 viewDir = glm::normalize(cameraPos - pos);
	glm::vec3 reflectDir = glm::normalize(glm::reflect(-lightDir, normal));
	glm::vec3 specular = lightColor * specularColor * glm::pow(glm::clamp(glm::dot(reflectDir, viewDir), 0.0f, 1.0f), gloss);

	return glm::vec4(ambient + diffuse + specular, 1.0f);
}

glm::vec3 ShaderBumpedNormal::calcBumpedNormal(glm::vec3& normal, glm::vec3& tangent, glm::vec3& sample) {
	glm::vec3 tangent_new = glm::normalize(tangent - glm::dot(tangent, normal)*normal);
	glm::vec3 bitangent_new = glm::cross(tangent_new, normal);
	glm::vec3 bumpMapNormal = 2.0f * sample - glm::vec3(1.0f, 1.0f, 1.0f);
	glm::mat3 TBN = glm::mat3(tangent_new, bitangent_new, normal);

	glm::vec3 worldNormal = glm::normalize(TBN * bumpMapNormal);

	return worldNormal;
}