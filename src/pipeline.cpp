#include "Pipeline.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>

Pipeline* Pipeline::instance = nullptr;

Pipeline::Pipeline() {
	ViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewportMatrix = glm::mat4(1.0f);
}

void Pipeline::SetModel(const glm::mat4& transformation) {
	ModelMatrix = transformation;
}


void Pipeline::SetViewport(int x, int y, int w, int h) {
	this->x_viewport = x;
	this->y_viewport = y;
	this->width_viewport = w;
	this->height_viewport = h;

	ViewportMatrix = glm::mat4(1.0f);
	ViewportMatrix[3][0] = x + w / 2.f;
	ViewportMatrix[3][1] = y + h / 2.f;
	ViewportMatrix[3][2] = 1.f;
	ViewportMatrix[0][0] = w / 2.f;
	ViewportMatrix[1][1] = h / 2.f;
	ViewportMatrix[2][2] = 0;

	if(zbuffer!=nullptr)
		delete []zbuffer;
	zbuffer = new float[w*h];
}

void Pipeline::SetProjection(float FoV, float aspect_ratio, float zNear, float zFar) {
	this->FoV = FoV;
	this->aspect_ratio = aspect_ratio;
	this->zNear = zNear;
	this->zFar = zFar;

	float zRange = zNear - zFar;
	float tanHalfFOV = glm::tan(glm::radians(FoV / 2.0));
	ProjectionMatrix = glm::mat4(0);
	ProjectionMatrix[0][0] = 1.0f / (tanHalfFOV * aspect_ratio);
	ProjectionMatrix[1][1] = 1.0f / tanHalfFOV;
	ProjectionMatrix[2][2] = -(zNear + zFar) / zRange;
	ProjectionMatrix[2][3] = 1.0f;
	ProjectionMatrix[3][2] = 2.0f * zFar * zNear / zRange;
}

void Pipeline::SetView(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
	this->eye = eye;
	this->center = center;

	glm::vec3 z = glm::normalize(center - eye);
	glm::vec3 x = glm::normalize(glm::cross(z, up));
	glm::vec3 y = glm::normalize(glm::cross(x, z));

	this->forward = z;
	this->right = x;
	this->up = y;


	glm::mat4x4 CameraMatrix = glm::mat4x4(1.0f);
	CameraMatrix[0] = glm::vec4(x, 0);
	CameraMatrix[1] = glm::vec4(y, 0);
	CameraMatrix[2] = -glm::vec4(z, 0);
	CameraMatrix = glm::transpose(CameraMatrix);

	glm::mat4x4 TranlateMatrix = glm::translate(glm::mat4(1.0f), -eye);

	ViewMatrix = CameraMatrix * TranlateMatrix;
}

glm::vec3 Pipeline::barycentric(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 P) {
	glm::vec3 s[2];
	for (int i = 2; i--; ) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	glm::vec3 u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return glm::vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return glm::vec3(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

bool Pipeline::culling(glm::mat3x3 &ndc) {
	//std::cout << glm::to_string(ndc) << std::endl;
	for (int i = 0; i < 3; i++) {
		int check = 0;
		if (ndc[i].z < 1.0f) check |= 1;
		if (ndc[i].z > 2.0f) check |= 2;
		if (ndc[i].x < -1.0f) check |= 4;
		if (ndc[i].x > 1.0f) check |= 8;
		if (ndc[i].y < -1.0f) check |= 16;
		if (ndc[i].y > 1.0f) check |= 32;
		if (check == 0) {
			return false;

		}
	}
	return true;
}


void Pipeline::triangle(struct vert_out* attributes, ShaderBase *shader, unsigned char *colorbuffer, float *zbuffer) {

	glm::mat3x2 uv_tri;   //三角形UV矩阵
	glm::mat3x4 normal_tri;  //三角形顶点法线矩阵 
	glm::mat3x4 tangent_tri;  //三角形顶点切线矩阵 
	glm::mat3x4 worldPos_tri; //三角形顶点世界空间坐标矩阵
	glm::mat3x4 clipPos_tri;  //三角形顶点裁剪空间坐标矩阵
	glm::mat3x3 ndc_tri;   //三角形顶点NDC矩阵

	//数组转化为矩阵
	for (int i = 0; i < 3; i++) {
		uv_tri[i] = attributes[i].uv;
		worldPos_tri[i] = glm::vec4(attributes[i].worldPos, 1.0f);
		normal_tri[i] = glm::vec4(attributes[i].worldNormal,0.f);
		tangent_tri[i] = glm::vec4(attributes[i].worldTangent, 0.f);
		glm::vec4 tmp = attributes[i].clipPos;
		clipPos_tri[i] = tmp;
		ndc_tri[i] = tmp / tmp[3]; //齐次除法
	}

	if(culling(ndc_tri)) return; //剔除完全不在视锥体内的三角形

	glm::mat3x4 viewclipPos_tri = ViewportMatrix*clipPos_tri; //视锥体内坐标
	glm::mat3x2 screenPos_tri;  //屏幕空间坐标


	for (int i = 0; i < 3; i++) {
		glm::vec4 tmp = viewclipPos_tri[i];
	    tmp /= tmp.w;
		screenPos_tri[i] = tmp;
	}

	//边界框
	glm::vec2 bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	glm::vec2 bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	glm::vec2 clamp(width_viewport - 1, height_viewport - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], screenPos_tri[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], screenPos_tri[i][j]));
		}
	}
	glm::ivec2 P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {

			//屏幕空间重心坐标
			glm::vec3 bc_screen = barycentric(screenPos_tri[0],
				screenPos_tri[1], screenPos_tri[2], P);
			//裁剪空间重心坐标
			glm::vec3 bc_clip = glm::vec3(bc_screen.x / viewclipPos_tri[0][3],
					bc_screen.y / viewclipPos_tri[1][3], bc_screen.z / viewclipPos_tri[2][3]);
			bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);

			//深度测试
			float depth_frag = glm::dot(glm::row(clipPos_tri, 2), bc_clip);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z<0 || zbuffer[P.x + P.y*width_viewport]>depth_frag) continue;
			
			//用重心坐标对顶点属性插值得到片元属性
			frag_in attri_frag;
			{
				attri_frag.uv = uv_tri*bc_clip;
				attri_frag.worldNormal = normal_tri * bc_clip;
				attri_frag.worldTangent = tangent_tri * bc_clip;
				attri_frag.worldPos = worldPos_tri * bc_clip;
			}

			//片元着色
			glm::vec4 color_frag = shader->fragment(attri_frag);
			color_frag =  glm::clamp(color_frag, glm::vec4(0) , glm::vec4(1.0f) );
			for (int i = 0; i < 4; i++) {
				setPixel(colorbuffer, P.x, P.y, color_frag);
			}
			zbuffer[P.x + P.y*width_viewport] = depth_frag;
		}
	}
}



void Pipeline::clearZbuffer() {
	for (int i = width_viewport * height_viewport; i--; zbuffer[i] = -std::numeric_limits<float>::max());
}



void Pipeline::clearColorbuffer(unsigned char *colorbuffer,const glm::vec4& color) {
	unsigned char c[4];
	for (int i = 0; i < 4; i++) {
		c[i] = (unsigned int)(color[i] * 255.0f) ;
	}
	for (int i = 0; i < width_viewport*height_viewport; i++) {
		memcpy(colorbuffer + (4 * i), c, sizeof(unsigned char) * 4);
	}
}

void Pipeline::setPixel(unsigned char* colorbuffer, int x, int y, const glm::vec4& color) {
	unsigned char c[4];
	for (int i = 0; i < 4; i++) {
		c[i] = (unsigned int)(color[i] * 255.0f);
	}
	memcpy(colorbuffer + (4 * (x+width_viewport*y-1)), c, sizeof(unsigned char) * 4);
}

void Pipeline::Render(SRMesh* mesh, unsigned char *colorbuffer) {
	std::cout << "\nStart rendering \"" << mesh->mName << "\"!\n";
	//清空深度缓冲区和颜色缓冲区
	clearZbuffer();
	clearColorbuffer(colorbuffer,backgroundColor);
	ShaderBase* shader = mesh->mShader;
	for (int i = 0; i < mesh->nFaces(); i++) {
		struct vert_out attributes[3];
		for (int j = 0; j < 3; j++) {
			struct vert_in v(mesh->GetVert(i,j),mesh->GetUV(i,j),mesh->GetNormal(i,j), mesh->GetTangent(i, j));
			attributes[j] = shader->vertex(v);
		}
		triangle(attributes,shader, colorbuffer, zbuffer);
	}

	std::cout << "Finish rendering \"" << mesh->mName << "\"!\n";
}

void Pipeline::Render(std::vector<SRMesh*> meshs, unsigned char *colorbuffer) {
	//清空深度缓冲区和颜色缓冲区
	clearZbuffer();
	clearColorbuffer(colorbuffer, backgroundColor);

	for (unsigned int i = 0; i < meshs.size(); i++) {
		SRMesh* mesh = meshs.at(i);
		SetModel(mesh->modelMatrix);
		std::cout << "\nStart rendering \"" << mesh->mName << "\"!\n";
		ShaderBase* shader = mesh->mShader;
		for (int i = 0; i < mesh->nFaces(); i++) {
			struct vert_out attributes[3];
			for (int j = 0; j < 3; j++) {
				struct vert_in v(mesh->GetVert(i, j), mesh->GetUV(i, j), mesh->GetNormal(i, j), mesh->GetTangent(i, j));
				attributes[j] = shader->vertex(v);
			}
			triangle(attributes, shader, colorbuffer, zbuffer);
		}
		std::cout << "Finish rendering \"" << mesh->mName << "\"!\n";
	}


}

glm::vec4 Pipeline::ObjectToClipPos(const glm::vec3& pos) {
	return ObjectToClipPos(glm::vec4(pos, 1.0f));
}

glm::vec4 Pipeline::ObjectToClipPos(const glm::vec4& pos) {
	return ProjectionMatrix*ViewMatrix*ModelMatrix*pos;
}

glm::vec4 Pipeline::ObjectToWorldPos(const glm::vec3& pos) {
	return ObjectToWorldPos(glm::vec4(pos, 1.0f));
}

glm::vec4 Pipeline::ObjectToWorldPos(const glm::vec4& pos) {
	return ModelMatrix*pos;
}

glm::vec4 Pipeline::ObjectToWorldDir(const glm::vec3& dir) {
	return ObjectToWorldDir(glm::vec4(dir, 0.0f));
}

glm::vec4 Pipeline::ObjectToWorldDir(const glm::vec4& dir) {
	return glm::transpose(glm::inverse(ModelMatrix))*dir;
}

void Pipeline::MoveForward(float distance) {
	//eye摄像机位置，center摄像机看向的地方，up摄像机向上的坐标轴
	this->SetView(eye + distance * forward, center, up);
	std::cout << "Camera: (" << eye.x << "," << eye.y << "," << eye.z << ")\n";
}

void Pipeline::MoveUp(float distance) {
	this->SetView(eye + distance * up, center + distance * up, up);
	std::cout << "Camera: (" << eye.x << "," << eye.y << "," << eye.z << ")\n";
}

void Pipeline::MoveRight(float distance) {
	this->SetView(eye + distance * right, center + distance * right, up);
	std::cout << "Camera: (" << eye.x << "," << eye.y << "," << eye.z << ")\n";
}
void Pipeline::RotateX(float distance) {
	glm::mat4 matrix = glm::rotate(glm::mat4(1), glm::radians(distance), glm::vec3(1, 0, 0));
	eye = matrix * glm::vec4(eye, 1.0f);
	this->SetView(eye, center, up);
	std::cout << "Camera: (" << eye.x << "," << eye.y << "," << eye.z << ")\n";
	std::cout << "matrix: " << matrix.length() << "\n";
}

void Pipeline::RotateY(float distance) {
	glm::mat4 matrix = glm::rotate(glm::mat4(1), glm::radians(distance), glm::vec3(0, 1, 0));
	eye = matrix * glm::vec4(eye, 1.0f);
	this->SetView(eye, center , up);
	std::cout << "Camera: (" << eye.x << "," << eye.y << "," << eye.z << ")\n";
	std::cout << "matrix: " << matrix.length() << "\n";
}

void Pipeline::RotateZ(float distance) {
	glm::mat4 matrix = glm::rotate(glm::mat4(1), glm::radians(distance), glm::vec3(0, 0, 1));
	eye = matrix * glm::vec4(eye, 1.0f);
	this->SetView(eye, center, up);
	std::cout << "Camera: (" << eye.x << "," << eye.y << "," << eye.z << ")\n";
	std::cout << "matrix: " << matrix.length() << "\n";
}

void Pipeline::AddLight(Light* light) {
	worldLights.push_back(light);
}
Light* Pipeline::GetLight(int index) {
	if (index >= 0 && index < worldLights.size())
		return worldLights.at(index);
	else
		return nullptr;
}

glm::vec3 Pipeline::GetCameraPos() {
	return eye;
}