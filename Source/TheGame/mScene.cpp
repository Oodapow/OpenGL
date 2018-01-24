#include "mScene.h"
#include "mCamera.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

#include <stb/stb_image.h>

#include <Core/Engine.h>
#include <Core/Managers/ResourcePath.h>

#include <TheGame/mBuilder.h>
#include <TheGame/mWater.h>

mScene::mScene()
{

}

mScene::~mScene()
{

}

void mScene::Init()
{
	{
		light.direction = glm::normalize(glm::vec3(0, -1.0f, 0));
		light.ambient = glm::vec3(0.4f, 0.4f, 0.4f);
		light.diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
		light.specular = glm::vec3(0.3f, 0.3f, 0.3f);
	}

	{
		camera = new mCamera();
		camera->Pitch = 0;
		camera->Position = glm::vec3(2);
		camera->UpdateCameraVectors();
	}

	{
		Shader *shader = new Shader("light");
		shader->AddShader(RESOURCE_PATH::SHADERS + "VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader(RESOURCE_PATH::SHADERS + "FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		water = new mWater();
		water->mesh = "water";
		water->shader = "water";
		water->position = glm::vec3(0);
		auto wmesh = water->InitMesh(300, 300, 0.0f, 1);
		meshes[wmesh->GetMeshID()] = wmesh;
		auto wshader = water->InitShader();
		shaders[wshader->GetName()] = wshader;
		water->BuildReflectionBuffer(Engine::GetWindow()->GetResolution().x, Engine::GetWindow()->GetResolution().y);
		water->BuildRefractionBuffer(Engine::GetWindow()->GetResolution().x, Engine::GetWindow()->GetResolution().y);
		water->InitTextureMaps();
	}

	{
		int size = 100;
		float line = 0.5;
		Builder b(glm::vec2(size), glm::vec2(0), line, 11);
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				std::ostringstream ost;

				ost << "terrain" << i << j;
				b.position.x = line * size * i;
				b.position.y = line * size * j;
				auto mesh = b.GetMesh(ost.str());
				meshes[mesh->GetMeshID()] = mesh;

				mObject* obj = new mObject();
				obj->mesh = ost.str();
				obj->shader = "light";
				obj->position = glm::vec3(b.position.x, 0, b.position.y);
				objects.push_back(obj);
			}
		}
	}

	glEnable(GL_DEPTH_TEST);
}

void mScene::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.2, 0.3, 0.7, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void mScene::Update(float deltaTimeSeconds)
{
	windOffset += deltaTimeSeconds * windSpeed;
	camera->UpdateCameraVectors();

	// render
	// ------
	// bind to framebuffer and draw scene as we normally would to color texture 
	water->BindReflectionBuffer();
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

							 // make sure we clear the framebuffer's content
	glClearColor(0.2, 0.3, 0.7, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
	plane = glm::vec4(0, 1, 0, 0);

	float distance = 2 * (camera->Position.y) - water->position.y;
	camera->Position.y -= distance;
	camera->InvertPitch();

	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		RenderObject(*(*it));
	}

	camera->InvertPitch();
	camera->Position.y += distance;

	water->BindRefractionBuffer();

	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	glClearColor(0.2, 0.3, 0.7, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
	plane = glm::vec4(0, -1, 0, 0);

	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		RenderObject(*(*it));
	}

	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.2, 0.3, 0.7, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CLIP_DISTANCE0);

	water->BindTextures();
	RenderObject(*water);

	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		RenderObject(*(*it));
	}
}

void mScene::FrameEnd()
{
	
}

void mScene::RenderObject(const mObject& object)
{
	if (!meshes[object.mesh] || !shaders[object.shader] || !shaders[object.shader]->GetProgramID())
		return;

	if (!object.children.empty())
	{
		for (auto it = object.children.begin(); it != object.children.end(); it++)
		{
			RenderObject(**it);
		}
	}

	shaders[object.shader]->Use();
	shaders[object.shader]->BindTexturesUnits();

	shaders[object.shader]->setVec4("plane", plane);
	shaders[object.shader]->setFloat("windOffset", windOffset);

	shaders[object.shader]->setVec3("light.direction", light.direction);
	shaders[object.shader]->setVec3("light.ambient", light.ambient);
	shaders[object.shader]->setVec3("light.diffuse", light.diffuse);
	shaders[object.shader]->setVec3("light.specular", light.specular);

	shaders[object.shader]->setVec3("material.ambient", glm::vec3(0.3));
	shaders[object.shader]->setVec3("material.diffuse", glm::vec3(0.6));
	shaders[object.shader]->setVec3("material.specular", glm::vec3(0.2));
	shaders[object.shader]->setFloat("material.shininess", 4);

	shaders[object.shader]->setVec3("viewPos", camera->Position);
	
	shaders[object.shader]->setMat4("view", camera->GetViewMatrix());
	shaders[object.shader]->setMat4("projection", camera->GetPerspectiveMatrix());

	glm::mat4 modelMatrix(1);
	{
		modelMatrix = glm::translate(modelMatrix, -object.get_center());
		modelMatrix = glm::translate(modelMatrix, object.get_position());

		modelMatrix = glm::translate(modelMatrix, object.get_center());
		modelMatrix = glm::rotate(modelMatrix, object.get_r(), glm::vec3(0.0f, -1.0f, 0.0f));
		modelMatrix = glm::translate(modelMatrix, -object.get_center());

		modelMatrix = glm::translate(modelMatrix, object.get_center());
		modelMatrix = glm::rotate(modelMatrix, object.get_ry(), glm::vec3(0.0f, 0.0f, -1.0f));
		modelMatrix = glm::translate(modelMatrix, -object.get_center());
	}

	modelMatrix = glm::scale(modelMatrix, object.s);

	shaders[object.shader]->setMat4("model", modelMatrix);

	meshes[object.mesh]->Render(shaders[object.shader]);
}

void mScene::OnInputUpdate(float deltaTime, int mods)
{
	if (window->KeyHold(GLFW_KEY_W)) camera->Position = camera->Position + camera->GetFront() * deltaTime * cameraSpeed;
	if (window->KeyHold(GLFW_KEY_A)) camera->Position = camera->Position - camera->GetRight() * deltaTime * cameraSpeed;
	if (window->KeyHold(GLFW_KEY_S)) camera->Position = camera->Position - camera->GetFront() * deltaTime * cameraSpeed;
	if (window->KeyHold(GLFW_KEY_D)) camera->Position = camera->Position + camera->GetRight() * deltaTime * cameraSpeed;
	if (window->KeyHold(GLFW_KEY_SPACE)) camera->Position = camera->Position + camera->GetUp() * deltaTime * cameraSpeed;
	if (window->KeyHold(GLFW_KEY_SPACE) && (mods & GLFW_MOD_SHIFT) != 0) camera->Position = camera->Position - camera->GetUp() * deltaTime * cameraSpeed;

	if (window->KeyHold(GLFW_KEY_1))
	{
		light.direction = glm::vec3(glm::rotate(glm::mat4(1), deltaTime, glm::vec3(1,0,0)) * glm::vec4(light.direction, 1.0f));
	}

	if (window->KeyHold(GLFW_KEY_2))
	{
		light.direction = glm::vec3(glm::rotate(glm::mat4(1), -deltaTime, glm::vec3(1, 0, 0)) * glm::vec4(light.direction, 1.0f));
	}
}

void mScene::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_3)
	{
		float ncs = cameraSpeed + 1;
		if (ncs > 5) ncs = 5;
		cameraSpeed = ncs;
	}

	if (key == GLFW_KEY_4)
	{
		float ncs = cameraSpeed - 1;
		if (ncs < 0) ncs = 0;
		cameraSpeed = ncs;
	}

	if (key == GLFW_KEY_0)
	{
		if (!visible) {
			Engine::GetWindow()->ShowPointer();
		}
		else {
			Engine::GetWindow()->DisablePointer();
		}

		visible = !visible;
	}

	if (key == GLFW_KEY_9)
	{
		for (auto it = shaders.begin(); it != shaders.end(); it++)
		{
			(*it).second->Reload();
		}
	}
}

void mScene::OnKeyRelease(int key, int mods)
{
	
}

void mScene::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (deltaX < 100 && deltaY < 100) {
		float s = 0.003f;
		camera->Yaw += deltaX * s;
		camera->Pitch -= deltaY * s;
	}
}

void mScene::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void mScene::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void mScene::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	camera->Zoom -= offsetY*2;
}

void mScene::OnWindowResize(int width, int height)
{
	{
		water->ResizeTextureBuffers(width, height);
	}
}