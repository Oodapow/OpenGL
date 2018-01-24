#pragma once
#include "mCamera.h"
#include "mWater.h"
#include "mObject.h"
#include "mSkyBox.h"
#include <Core/GPU/Texture2D.h>
struct Light {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
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

	void RenderObjects(glm::vec4 plane = glm::vec4(0));
	void RenderBox(glm::vec4 plane = glm::vec4(0));
	void RenderObjectsWithNormals();
	void RenderWater();

	std::unordered_map<std::string, Mesh*> meshes;
	std::unordered_map<std::string, Shader*> shaders;

	Light light;
	mCamera *camera;

	bool visible = false;
	bool useNvs = false;

	float windOffset = 0;
	float windSpeed = 0.02;
	float cameraSpeed = 2;
	float viewDistance = 150.0f;

	std::vector<mObject*> objects;
	mWater* water;
	mSkyBox* skybox;

	Texture2D* texture_d;
	Texture2D* texture_n;
	Texture2D* texture_s;

	Texture2D* texture_d2;
	Texture2D* texture_n2;
	Texture2D* texture_s2;

	Texture2D* texture_d3;
	Texture2D* texture_n3;
	Texture2D* texture_s3;

	Texture2D* texture_d4;
	Texture2D* texture_n4;
	Texture2D* texture_s4;

	glm::vec4 plane;
};

