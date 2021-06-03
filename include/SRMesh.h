#ifndef __SR_MESH_H__
#define __SR_MESH_H__
#include <vector>
#include <glm/glm.hpp>
#include <string>

class SRMesh
{
public:
	std::string mName;
	std::vector<glm::ivec3> mFaces;
	std::vector<glm::vec3> mVertices;
	std::vector<glm::vec3> mUVs;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec3> mTangents;
	std::vector<glm::vec3> mBitangents;


};

#endif // __SR_MESH_H__
