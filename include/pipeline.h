#ifndef __PIPELINE_H__
#define __PIPELINE_H__
#include <glm/glm.hpp>
#include "Shader.h"
#include "SRMesh.h"
#include "Light.h"

class Pipeline //渲染管线类
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

	//设置变换矩阵
	void SetModel(const glm::mat4& transformation);
	void SetViewport(int x, int y, int w, int h);
	void SetProjection(float FoV, float aspect_ratio, float zNear, float zFar);
	void SetView(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

public:  //顶点或向量变换相关函数

	/*点从模型空间变换到世界空间*/
	glm::vec4 ObjectToWorldPos(const glm::vec3& pos);
	glm::vec4 ObjectToWorldPos(const glm::vec4& pos);

	/*点从模型空间变换到裁剪空间*/
	glm::vec4 ObjectToClipPos(const glm::vec3& pos);
	glm::vec4 ObjectToClipPos(const glm::vec4& pos);

	/*向量从模型空间变换到世界空间*/
	glm::vec4 ObjectToWorldDir(const glm::vec3& dir);
	glm::vec4 ObjectToWorldDir(const glm::vec4& dir);

public:
	void Render(SRMesh* mesh, unsigned char *colorbuffer); //渲染一个模型
	void Render(std::vector<SRMesh*> meshs, unsigned char *colorbuffer); //渲染多个模型
	void MoveForward(float distance); /*前后移动*/
	void MoveUp(float distance); /*上下移动*/
	void MoveRight(float distance); /*左右移动*/

	void RotateX(float distance);/* 摄像机x轴旋转 */
	void RotateY(float distance);/* 摄像机y轴旋转 */
	void RotateZ(float distance);/* 摄像机z轴旋转 */

	void ModelMoveX(float distance, SRMesh* mesh);/* 模型x轴旋转 */
	void ModelMoveY(float distance, SRMesh* mesh);/* 模型y轴旋转 */
	void ModelMoveZ(float distance, SRMesh* mesh);/* 模型z轴旋转 */

public:
	//设置背景颜色
	void SetBGColor(glm::vec4 color) {
		backgroundColor = color;
	}
	void FillColor(unsigned char *colorbuffer, const glm::vec4 & color) {
		clearColorbuffer(colorbuffer, color);
	}
private:
	static Pipeline* instance;
	Pipeline();


	//变换矩阵
	glm::mat4 ModelMatrix;	/*模型变换矩阵*/
	glm::mat4 ViewMatrix;	/*观察变换矩阵*/
	glm::mat4 ProjectionMatrix; /*投影变换矩阵*/
	glm::mat4 ViewportMatrix; /*屏幕映射矩阵*/

	//变换参数
	int x_viewport, y_viewport, width_viewport, height_viewport;
	float FoV, aspect_ratio, zNear, zFar;
	glm::vec3 eye, center, up, right, forward;

	//深度缓冲区
	float *zbuffer = nullptr;

	//颜色
	glm::vec4 backgroundColor; //背景颜色

	glm::vec3 barycentric(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 P); //求点在三角形的重心坐标
	bool culling(glm::mat3x3 &ndc);		//剔除
	void triangle(struct vert_out* attributes, ShaderBase *shader, unsigned char *colorbuffer, float *zbuffer); //绘制一个三角形
	void clearZbuffer();  //清空深度缓冲区
	void clearColorbuffer(unsigned char *colorbuffer, const glm::vec4& color); //清空颜色缓冲区
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