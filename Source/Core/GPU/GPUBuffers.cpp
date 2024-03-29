#include "GPUBuffers.h"

using namespace std;

enum VERTEX_ATTRIBUTE_LOC
{
	POS,
	TEX_COORD,
	NORMAL,
};

GPUBuffers::GPUBuffers()
{
	size = 0;
	VAO = 0;
	memset(VBO, 0, 6 * sizeof(int));
}

void GPUBuffers::CreateBuffers(unsigned int size)
{
	this->size = size;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(size, VBO);
}

void GPUBuffers::ReleaseMemory()
{
	if (size) {
		size = 0;
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(size, VBO);
	}
}

namespace UtilsGPU
{
	GPUBuffers UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec3> &normals, 
					const vector<unsigned short>& indices)
	{
		GPUBuffers buffers;
		buffers.CreateBuffers(3);
		glBindVertexArray(buffers.VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::NORMAL);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.VBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		CheckOpenGLError();

		return buffers;
	}

	GPUBuffers UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec3> &normals,
					const vector<glm::vec2> &text_coords,
					const vector<unsigned short> &indices)
	{
		// Create the VAO
		GPUBuffers buffers;
		buffers.CreateBuffers(4);
		glBindVertexArray(buffers.VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);    

		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_coords[0]) * text_coords.size(), &text_coords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::NORMAL);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.VBO[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);
		CheckOpenGLError();

		return buffers;
	}

	GPUBuffers UploadData(const std::vector<glm::vec3> &positions, const std::vector<unsigned short>& indices)
	{
		// Create the VAO
		GPUBuffers buffers;
		buffers.CreateBuffers(2);
		glBindVertexArray(buffers.VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.VBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);
		CheckOpenGLError();

		return buffers;
	}

	GPUBuffers UploadData(const std::vector<glm::vec3> &positions,const vector<glm::vec2>& texCoords, const std::vector<unsigned short>& indices)
	{
		// Create the VAO
		GPUBuffers buffers;
		buffers.CreateBuffers(3);
		glBindVertexArray(buffers.VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords[0]) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.VBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);
		CheckOpenGLError();

		return buffers;
	}
}
