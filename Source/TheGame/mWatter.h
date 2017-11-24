#pragma once
#include <random>

#include <Core/GPU/Mesh.h>
#include <glm/glm.hpp>

class mWatter {
public:

	static Mesh* GetMesh(int dx, int dz, float y, float line) {
		double lineLength = 1;
		std::string name = "watter";

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned short> indices;

		unsigned short index = 0;

		glm::vec3 p1 = glm::vec3(-dx / 2 * line, y, -dz / 2 * line);
		glm::vec3 p2 = glm::vec3(-dx / 2 * line, y, (dz / 2 + 1) * line);
		glm::vec3 p3 = glm::vec3((dx / 2 + 1) * line, y, -dz / 2 * line);
		glm::vec3 p4 = glm::vec3((dx / 2 + 1) * line, y, (dz / 2 + 1) * line);

		positions.push_back(p1);
		positions.push_back(p2);
		positions.push_back(p3);
		positions.push_back(p2);
		positions.push_back(p4);
		positions.push_back(p3);

		glm::vec3 n1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
		glm::vec3 n2 = glm::normalize(glm::cross(p3 - p4, p2 - p4));

		normals.push_back(n1);
		normals.push_back(n1);
		normals.push_back(n1);
		normals.push_back(n2);
		normals.push_back(n2);
		normals.push_back(n2);

		glm::vec2 t1 = glm::vec2(0, 1);
		glm::vec2 t2 = glm::vec2(0, 0);
		glm::vec2 t3 = glm::vec2(1, 1);
		glm::vec2 t4 = glm::vec2(1, 0);

		texCoords.push_back(t1);
		texCoords.push_back(t2);
		texCoords.push_back(t3);
		texCoords.push_back(t2);
		texCoords.push_back(t4);
		texCoords.push_back(t3);

		indices.push_back(index++);
		indices.push_back(index++);
		indices.push_back(index++);
		indices.push_back(index++);
		indices.push_back(index++);
		indices.push_back(index++);

		auto mesh = new Mesh(name);
		mesh->InitFromData(positions, normals, texCoords, indices);
		return mesh;
	}
};
