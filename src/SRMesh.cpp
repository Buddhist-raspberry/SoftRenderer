#include "SRMesh.h"

void SRMesh::SetShader(ShaderBase* shader) {
	if (mShader != nullptr) {
		delete mShader;
	}
	mShader = shader;
}