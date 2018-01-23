#pragma once
#include <string>
#include <vector>

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>			// Output data structure
#include <assimp/postprocess.h>		// Post processing flags

#include <include/glm.h>

#include "Shader.h"

class GPUBuffers;
class Texture2D;

class Mesh
{
	typedef unsigned int GLenum;

	public:
		Mesh(std::string meshID);
		virtual ~Mesh();

		bool InitFromData(std::vector<glm::vec3>& positions,
						std::vector<glm::vec3>& normals,
						std::vector<glm::vec2>& texCoords,
						std::vector<unsigned short>& indices);

		bool InitFromData(std::vector<glm::vec3>& positions, std::vector<unsigned short>& indices);

		// GL_POINTS, GL_TRIANGLES, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY,
		// GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY
		void SetDrawMode(GLenum primitive);
		GLenum GetDrawMode() const;

		void Render(Shader *shader = NULL) const;

		const char* GetMeshID() const;

	private:
		std::string meshID;
		glm::vec3 halfSize;
		glm::vec3 meshCenter;

	protected:
		std::string fileLocation;

		GLenum glDrawMode;
		GPUBuffers *buffers;

		int nrIndices;
};