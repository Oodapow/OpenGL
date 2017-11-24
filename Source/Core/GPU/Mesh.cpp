#include "Mesh.h"

#include <include/utils.h>

#include <Core/GPU/GPUBuffers.h>
#include <Core/GPU/Texture2D.h>
#include <Core/Managers/TextureManager.h>

using namespace std;

static_assert(sizeof(aiColor4D) == sizeof(glm::vec4), "WARNING! glm::vec4 and aiColor4D size differs!");

Mesh::Mesh(std::string meshID)
{
	this->meshID = std::move(meshID);

	glDrawMode = GL_TRIANGLES;
	buffers = new GPUBuffers();
}

Mesh::~Mesh()
{
	buffers->ReleaseMemory();
	SAFE_FREE(buffers);
}

const char * Mesh::GetMeshID() const
{
	return meshID.c_str();
}
bool Mesh::InitFromData(vector<glm::vec3>& positions,
						vector<glm::vec3>& normals,
						vector<glm::vec2>& texCoords,
						vector<unsigned short>& indices)
{
	nrIndices = indices.size();
	*buffers = UtilsGPU::UploadData(positions, normals, texCoords, indices);
	return buffers->VAO != 0;
}

GLenum Mesh::GetDrawMode() const
{
	return glDrawMode;
}

void Mesh::SetDrawMode(GLenum primitive)
{
	glDrawMode = primitive;
}

void Mesh::Render(Shader *shader) const
{
	glBindVertexArray(buffers->VAO);
	glDrawElementsBaseVertex(glDrawMode, nrIndices, GL_UNSIGNED_SHORT, 0, 0);
	glBindVertexArray(0);
}
