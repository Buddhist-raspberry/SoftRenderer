#ifndef __PIPELINE_H__
#define __PIPELINE_H__
#include <glm/glm.hpp>
#include "Shader.h"
#include "SRMesh.h"
#include "Light.h"

class Pipeline //��Ⱦ������
{
public:
	static Pipeline* getInstance() {
		if (instance == nullptr)
			instance = new Pipeline();
		return instance;
	}
	const glm::mat4& getModel() {
		return ModelMatrix;
	}
	const glm::mat4& getView() {
		return ViewMatrix;
	}
	const glm::mat4& getProjection() {
		return ProjectionMatrix;
	}
	const glm::mat4& getViewport() {
		return ViewportMatrix;
	}
	const glm::mat4& getMVP() {
		return ProjectionMatrix * ViewMatrix*ModelMatrix;
	}
	const glm::mat4& getVPV() {
		return ViewportMatrix * ProjectionMatrix * ViewMatrix;
	}

public:

	//���ñ任����
	void SetModel(const glm::mat4& transformation);
	void SetViewport(int x, int y, int w, int h);
	void SetProjection(float FoV, float aspect_ratio, float zNear, float zFar);
	void SetView(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

public:  //����������任��غ���

	/*���ģ�Ϳռ�任������ռ�*/
	glm::vec4 ObjectToWorldPos(const glm::vec3& pos);
	glm::vec4 ObjectToWorldPos(const glm::vec4& pos);

	/*���ģ�Ϳռ�任���ü��ռ�*/
	glm::vec4 ObjectToClipPos(const glm::vec3& pos);
	glm::vec4 ObjectToClipPos(const glm::vec4& pos);

	/*������ģ�Ϳռ�任������ռ�*/
	glm::vec4 ObjectToWorldDir(const glm::vec3& dir);
	glm::vec4 ObjectToWorldDir(const glm::vec4& dir);

public:
	void Render(SRMesh* mesh, unsigned char *colorbuffer); //��Ⱦһ��ģ��
	void Render(std::vector<SRMesh*> meshs, unsigned char *colorbuffer); //��Ⱦ���ģ��
	void MoveForward(float distance); /*ǰ���ƶ�*/
	void MoveUp(float distance); /*�����ƶ�*/
	void MoveRight(float distance); /*�����ƶ�*/

	void RotateX(float distance);/* �����x����ת */
	void RotateY(float distance);/* �����y����ת */
	void RotateZ(float distance);/* �����z����ת */

	void ModelMoveX(float distance, SRMesh* mesh);/* ģ��x����ת */
	void ModelMoveY(float distance, SRMesh* mesh);/* ģ��y����ת */
	void ModelMoveZ(float distance, SRMesh* mesh);/* ģ��z����ת */

public:
	//���ñ�����ɫ
	void SetBGColor(glm::vec4 color) {
		backgroundColor = color;
	}
	void FillColor(unsigned char *colorbuffer, const glm::vec4 & color) {
		clearColorbuffer(colorbuffer, color);
	}
private:
	static Pipeline* instance;
	Pipeline();


	//�任����
	glm::mat4 ModelMatrix;	/*ģ�ͱ任����*/
	glm::mat4 ViewMatrix;	/*�۲�任����*/
	glm::mat4 ProjectionMatrix; /*ͶӰ�任����*/
	glm::mat4 ViewportMatrix; /*��Ļӳ�����*/

	//�任����
	int x_viewport, y_viewport, width_viewport, height_viewport;
	float FoV, aspect_ratio, zNear, zFar;
	glm::vec3 eye, center, up, right, forward;

	//��Ȼ�����
	float *zbuffer = nullptr;

	//��ɫ
	glm::vec4 backgroundColor; //������ɫ

	glm::vec3 barycentric(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 P); //����������ε���������
	bool culling(glm::mat3x3 &ndc);		//�޳�
	void triangle(struct vert_out* attributes, ShaderBase *shader, unsigned char *colorbuffer, float *zbuffer); //����һ��������
	void clearZbuffer();  //�����Ȼ�����
	void clearColorbuffer(unsigned char *colorbuffer, const glm::vec4& color); //�����ɫ������
	void setPixel(unsigned char* colorbuffer, int x, int y, const glm::vec4& color);


public:
	AmbientLight* ambient;
private:
	std::vector<Light*> worldLights;
public:
	void AddLight(Light* light);
	Light* GetLight(int index);
	glm::vec3 GetCameraPos();
	float GetDepth();
	unsigned int GetLightCount() { return worldLights.size(); }
private:
	bool alphaTest = false;
	bool alphaBlend = false;
	float alphaCutOff = 0.5f;
public:
	void setAlphaTest(bool flag, float cutOff=0.5f) { alphaTest = flag; alphaCutOff = cutOff; }
	void setAlphaBlend(bool flag) { alphaBlend = flag; }
};


#endif /*__PIPELINE_H__*/