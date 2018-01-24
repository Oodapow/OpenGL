#pragma once

#include <glm/glm.hpp>

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -45.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVTY = 0.1f;
const float ZOOM = 45.0f;

class mCamera
{
public:
	glm::vec3 Position;

	float Yaw;
	float Pitch;

	float Zoom;

	mCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

	glm::vec3 GetFront() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetRight() const;

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetPerspectiveMatrix(float viewDistance) const;

	void UpdateCameraVectors();
	void InvertPitch();
private:
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	glm::vec3 front;
};