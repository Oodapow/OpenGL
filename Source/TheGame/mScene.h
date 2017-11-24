#pragma once
#include <Core/World.h>
#include <Core/GPU/Mesh.h>
#include <glm/glm.hpp>
#include "mCamera.h"

struct Light {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct mObject
{
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

class mScene :
	public World
{
public:
	mScene();
	~mScene();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderObject(const mObject& object);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	std::unordered_map<std::string, Mesh*> meshes;
	std::unordered_map<std::string, Shader*> shaders;

	Light light;
	mCamera *camera;
	int count = 4;
	int size = 14;
	bool visible = false;

	std::vector<mObject*> objects;
	mObject* watter;

	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;

	glm::vec4 plane;
};

