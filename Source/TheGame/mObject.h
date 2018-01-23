#pragma once

#include <Core/World.h>
#include <Core/GPU/Mesh.h>
#include <glm/glm.hpp>

class mObject
{
public:
	std::string mesh;
	std::string shader;

	glm::vec3 position;

	glm::vec3 center = glm::vec3(0);

	std::vector<mObject*> children;
	mObject* parent = nullptr;

	glm::vec3 get_position() const {
		if (parent != nullptr) {
			glm::mat4 oPos(1);
			oPos = glm::rotate(oPos, -parent->get_r(), glm::vec3(0.0f, -1.0f, 0.0f));
			return glm::vec3(glm::vec4(position, 1.0f) * oPos) + parent->get_position();
		}
		return position;
	}

	glm::vec3 get_center() const {
		if (parent != nullptr) {
			return center + parent->get_center();
		}
		return center;
	}

	float get_r() const {
		if (parent != nullptr) {
			return r + parent->get_r();
		}
		return r;
	}

	float get_ry() const {
		if (parent != nullptr) {
			return ry + parent->get_ry();
		}
		return ry;
	}

	float r;
	float ry = 0;
	glm::vec3 s = glm::vec3(1.0f);
};