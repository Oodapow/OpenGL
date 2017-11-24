#include <TheGame/mCamera.h>

#include <glm/gtc/matrix_transform.hpp>
#include <Core/Engine.h>

mCamera::mCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), Zoom(ZOOM)
{
	Position = position;
	worldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	UpdateCameraVectors();
}

glm::vec3 mCamera::GetFront() const
{
	return front;
}

glm::vec3 mCamera::GetUp() const
{
	return up;
}

glm::vec3 mCamera::GetRight() const
{
	return right;
}

glm::mat4 mCamera::GetViewMatrix() const
{
	return glm::lookAt(Position , Position + front, up);
}

glm::mat4 mCamera::GetPerspectiveMatrix() const
{
	auto wnd = Engine::GetWindow();
	return glm::perspective(glm::radians(Zoom), (float)wnd->GetResolution().x / (float)wnd->GetResolution().y, 0.1f, 100.0f);
}

void mCamera::UpdateCameraVectors()
{
	if (Zoom < 52.0f) 
	{
		Zoom = 52.0f;
	}
	else if (Zoom > 97.0f)
	{
		Zoom = 97.0f;
	}

	glm::vec3 front;
	front.x = cos(Yaw) * cos(Pitch);
	front.y = sin(Pitch);
	front.z = sin(Yaw) * cos(Pitch);
	this->front = glm::normalize(front);
	right = glm::normalize(glm::cross(this->front, worldUp));
	up = glm::normalize(glm::cross(right, this->front));
}

void mCamera::InvertPitch()
{
	Pitch = -Pitch;
	UpdateCameraVectors();
}