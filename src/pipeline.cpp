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

	glm::mat3x2 uv_tri;   //??????UV????
	glm::mat3x4 normal_tri;  //?????????????????? 
	glm::mat3x4 tangent_tri;  //?????????????????? 
	glm::mat3x4 worldPos_tri; //??????????????????????????
	glm::mat3x4 clipPos_tri;  //??????????????????????????
	glm::mat3x3 ndc_tri;   //??????????NDC????

	//??????????????
	for (int i = 0; i < 3; i++) {
		uv_tri[i] = attributes[i].uv;
		worldPos_tri[i] = glm::vec4(attributes[i].worldPos, 1.0f);
		normal_tri[i] = glm::vec4(attributes[i].worldNormal,0.f);
		tangent_tri[i] = glm::vec4(attributes[i].worldTangent, 0.f);
		glm::vec4 tmp = attributes[i].clipPos;
		clipPos_tri[i] = tmp;
		ndc_tri[i] = tmp / tmp[3]; //????????
	}

	if(culling(ndc_tri)) return; //????????????????????????????

	glm::mat3x4 viewclipPos_tri = ViewportMatrix*clipPos_tri; //????????????
	glm::mat3x2 screenPos_tri;  //????????????


	for (int i = 0; i < 3; i++) {
		glm::vec4 tmp = viewclipPos_tri[i];
	    tmp /= tmp.w;
		screenPos_tri[i] = tmp;
	}

	//??????
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

			//????????????????
			glm::vec3 bc_screen = barycentric(screenPos_tri[0],
				screenPos_tri[1], screenPos_tri[2], P);
			//????????????????
			glm::vec3 bc_clip = glm::vec3(bc_screen.x / viewclipPos_tri[0][3],
					bc_screen.y / viewclipPos_tri[1][3], bc_screen.z / viewclipPos_tri[2][3]);
			bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);

			//????????
			float depth_frag = glm::dot(glm::row(clipPos_tri, 2), bc_clip);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z<0 || zbuffer[P.x + P.y*width_viewport]>depth_frag) continue;
			
			//????????????????????????????????????
			frag_in attri_frag;
			{
				attri_frag.uv = uv_tri*bc_clip;
				attri_frag.worldNormal = normal_tri * bc_clip;
				attri_frag.worldTangent = tangent_tri * bc_clip;
				attri_frag.worldPos = worldPos_tri * bc_clip;
				attri_frag.clipPos = viewclipPos_tri * bc_clip;
			}

			//????????
			glm::vec4 color_frag = shader->fragment(attri_frag);
			color_frag = glm::clamp(color_frag, glm::vec4(0), glm::vec4(1.0f));
			if (alphaTest&&color_frag.a < alphaCutOff)
				continue;
			glm::vec4 color_result;
			if (alphaBlend) {
				glm::vec4 color_orig = getPixel(colorbuffer, P.x, P.y);
				color_result = color_frag * color_frag.a + color_orig * (1 - color_frag.a);
				color_result.a = color_orig.a + color_frag.a - color_orig.a * color_frag.a;
				//std::cout << color_orig.a << "," << color_frag.a << "\n";
			}
			else {
				color_result = color_frag;
			}
			for (int i = 0; i < 4; i++) {
				setPixel(colorbuffer, P.x, P.y, color_result);
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

glm::vec4 Pipeline::getPixel(unsigned char* colorbuffer, int x, int y) {
	glm::vec4 color;
	for (int i = 0; i < 4; i++) {
		color[i] = (float)colorbuffer[4 * (x + width_viewport * y - 1) + i]/255.0f;
	}
	return color;
}

void Pipeline::Render(SRMesh* mesh, unsigned char *colorbuffer) {
	std::cout << "\nStart rendering \"" << mesh->mName << "\"!\n";
	//??????????????????????????
	clearZbuffer();
	clearColorbuffer(colorbuffer,backgroundColor);
	ShaderBase* shader = mesh->mShader;
	SetModel(mesh->modelMatrix);
	for (int i = 0; i < mesh->nFaces(); i++) {
		struct vert_out attributes[3];
		for (int j = 0; j < 3; j++) {
			struct vert_in v(mesh->GetVert(i,j),mesh->GetUV(i,j),mesh->GetNormal(i,j), mesh->GetTangent(i, j));
			attributes[j] = shader->vertex(v);
		}
		triangle(attributes,shader, colorbuffer, zbuffer);
	}

	std::cout << "Finish rendering \"" << mesh->mName << "\"!\n";
	if (hasBloom) {
		bloom(colorbuffer, bloom_indensity, bloom_indensity);
	}

}

void Pipeline::Render(std::vector<SRMesh*> meshs, unsigned char *colorbuffer) {
	//??????????????????????????
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
	if(hasBloom){
		bloom(colorbuffer, bloom_indensity, bloom_threshold);
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
	//eye????????????center??????????????????up??????????????????
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
}

void Pipeline::RotateY(float distance) {
	glm::mat4 matrix = glm::rotate(glm::mat4(1), glm::radians(distance), glm::vec3(0, 1, 0));
	eye = matrix * glm::vec4(eye, 1.0f);
	this->SetView(eye, center , up);
	std::cout << "Camera: (" << eye.x << "," << eye.y << "," << eye.z << ")\n";
}

void Pipeline::RotateZ(float distance) {
	glm::mat4 matrix = glm::rotate(glm::mat4(1), glm::radians(distance), glm::vec3(0, 0, 1));
	eye = matrix * glm::vec4(eye, 1.0f);
	this->SetView(eye, center, up);
	std::cout << "Camera: (" << eye.x << "," << eye.y << "," << eye.z << ")\n";
}

void Pipeline::ModelMoveX(float distance, SRMesh* mesh) {
	mesh->modelMatrix = glm::translate(ModelMatrix, glm::vec3(distance, 0, 0));
}

void Pipeline::ModelMoveY(float distance, SRMesh* mesh) {
	mesh->modelMatrix = glm::translate(ModelMatrix, glm::vec3(0, distance, 0));
}

void Pipeline::ModelMoveZ(float distance, SRMesh* mesh) {
	mesh->modelMatrix  = glm::translate(ModelMatrix, glm::vec3(0, 0, distance));
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

float Pipeline::GetDepth() {
	return zFar - zNear;
}


unsigned char* Pipeline::threshold(unsigned char cb[], float T)
{
	const float weight[3] = { 0.2126f, 0.7152f, 0.0722f };

	const int size = 4 * width_viewport * height_viewport;
	unsigned char* tb = new unsigned char[size]();
	for (int i = 0; i < size; i += 4)
	{
		float r = (float)cb[i] * weight[0];
		float g = (float)cb[i + 1] * weight[1];
		float b = (float)cb[i + 2] * weight[2];
		if (r + g + b > T)
		{
			tb[i] = cb[i];
			tb[i + 1] = cb[i + 1];
			tb[i + 2] = cb[i + 2];
			tb[i + 3] = cb[i + 3];
		}
		else
		{
			/*printf("%f\n", r + b + g);*/
		}
	}
	return tb;
}

#define min(x,y) (((x)<(y))?(x):(y))


void Pipeline::bloom(unsigned char cb[], float indensity, float T )
{
	const float weight[5] = { 0.4026f, 0.2442f, 0.2442f,  0.0545f, 0.0545f };

	int width = width_viewport, height = height_viewport;
	unsigned char* tb = threshold(cb, T);
	unsigned int size = 4 * width * height;
	float* fcb = new float[size];
	for (unsigned int i = 0; i < size; ++i)
	{
		fcb[i] = (float)cb[i];
	}


	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			const int lines_before = -1 * min(4, i);
			const int lines_after = min(4, height - i - 1);
			const int col_before = -1 * min(4, j);
			const int col_after = min(4, width - j - 1);

			float increment_r = 0.0f;
			float increment_g = 0.0f;
			float increment_b = 0.0f;

			for (int k = lines_before; k <= lines_after; ++k)
			{
				for (int m = col_before; m <= col_after; ++m)
				{
					const int coordinate = ((i + k) * width + (j + m)) * 4;
					if (tb[coordinate] > 0 || tb[coordinate + 1] > 0 || tb[coordinate + 2] > 0)
					{
						//printf("%d  %f  %f  %d  %d\n", tb[coordinate], (float)tb[coordinate] * weight[abs(k)] * weight[abs(m)], weight[abs(k)] * weight[abs(m)], k, m);
						increment_r += (float)tb[coordinate] * weight[abs(k)] * weight[abs(m)];
						increment_g += (float)tb[coordinate + 1] * weight[abs(k)] * weight[abs(m)];
						increment_b += (float)tb[coordinate + 2] * weight[abs(k)] * weight[abs(m)];
					}
				}
			}
			const int coordinate = (i * width + j) * 4;
			fcb[coordinate] = cb[coordinate] + indensity * increment_r;
			fcb[coordinate + 1] = cb[coordinate + 1] + indensity * increment_g;
			fcb[coordinate + 2] = cb[coordinate + 2] + indensity * increment_b;
		}
	}
	for (unsigned int i = 0; i < size; ++i)
	{
		cb[i] = (unsigned char)min(255.0f, roundf(fcb[i]));
	}
	delete[]tb;
	delete[]fcb;
}