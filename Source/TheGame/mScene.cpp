#include "mScene.h"
#include "mCamera.h"

#include <vector>
#include <string>
#include <iostream>
#include <random>

#include <stb/stb_image.h>

#include <Core/Engine.h>
#include <Core/Managers/ResourcePath.h>

#include <TheGame/mBuilder.h>
#include <TheGame/mWatter.h>

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
		light.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	}

	{
		camera = new mCamera();
		camera->Pitch = -M_PI_4;
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
		Shader *shader = new Shader("watter");
		shader->AddShader(RESOURCE_PATH::SHADERS + "WVShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader(RESOURCE_PATH::SHADERS + "WFShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Builder b(size, size, 10);

		b.AddOffset(2);
		b.AddNoise(count);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.25);
		b.Blur();

		auto mesh = b.GetMesh();
		meshes[mesh->GetMeshID()] = mesh;

		auto mesh2 = mWatter::GetMesh(b.Getdx(), b.Getdz(), 0.0f,  1.0f);
		meshes[mesh2->GetMeshID()] = mesh2;
	}

	{
		mObject* obj = new mObject();
		obj->mesh = "terrain";
		obj->shader = "light";
		obj->position = glm::vec3(0);
		objects.push_back(obj);
	}

	{
		watter = new mObject();
		watter->mesh = "watter";
		watter->shader = "watter";
		watter->position = glm::vec3(0);
	}

	{
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// create a color attachment texture
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Engine::GetWindow()->GetResolution().x, Engine::GetWindow()->GetResolution().y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Engine::GetWindow()->GetResolution().x, Engine::GetWindow()->GetResolution().y); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
																									  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		shaders["watter"]->loc_textures[0] = textureColorbuffer;
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
	camera->UpdateCameraVectors();

	// render
	// ------
	// bind to framebuffer and draw scene as we normally would to color texture 
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

							 // make sure we clear the framebuffer's content
	glClearColor(0.2, 0.3, 0.7, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
	plane = glm::vec4(0, 1, 0, 0);

	float distance = 2 * (camera->Position.y);
	camera->Position.y -= distance;
	camera->InvertPitch();

	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		RenderObject(*(*it));
	}

	camera->InvertPitch();
	camera->Position.y += distance;

	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.2, 0.3, 0.7, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CLIP_DISTANCE0);

	RenderObject(*watter);

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
	if (window->KeyHold(GLFW_KEY_W)) camera->Position = camera->Position + camera->GetFront() * deltaTime * 4.0f;
	if (window->KeyHold(GLFW_KEY_A)) camera->Position = camera->Position - camera->GetRight() * deltaTime * 4.0f;
	if (window->KeyHold(GLFW_KEY_S)) camera->Position = camera->Position - camera->GetFront() * deltaTime * 4.0f;
	if (window->KeyHold(GLFW_KEY_D)) camera->Position = camera->Position + camera->GetRight() * deltaTime * 4.0f;
	if (window->KeyHold(GLFW_KEY_SPACE)) camera->Position = camera->Position + camera->GetUp() * deltaTime * 4.0f;
	if (window->KeyHold(GLFW_KEY_SPACE) && (mods & GLFW_MOD_SHIFT) != 0) camera->Position = camera->Position - camera->GetUp() * deltaTime * 4.0f;

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
		delete meshes["terrain"];
		meshes.erase("terrain");
		Builder b(size, size, 10);

		b.AddOffset(2);
		b.AddNoise(--count);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.25);
		b.Blur();

		auto mesh = b.GetMesh();
		meshes[mesh->GetMeshID()] = mesh;
	}

	if (key == GLFW_KEY_4)
	{
		delete meshes["terrain"];
		meshes.erase("terrain");
		Builder b(size, size, 10);

		b.AddOffset(2);
		b.AddNoise(++count);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.25);
		b.Blur();

		auto mesh = b.GetMesh();
		meshes[mesh->GetMeshID()] = mesh;
	}

	if (key == GLFW_KEY_5 && size > 3)
	{
		delete meshes["terrain"];
		meshes.erase("terrain");
		size--;
		Builder b(size, size, 10);

		b.AddOffset(2);
		b.AddNoise(count);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.25);
		b.Blur();

		auto mesh = b.GetMesh();
		meshes[mesh->GetMeshID()] = mesh;
	}

	if (key == GLFW_KEY_6 && size < 14)
	{
		delete meshes["terrain"];
		meshes.erase("terrain");
		size++;
		Builder b(size, size, 10);

		b.AddOffset(2);
		b.AddNoise(count);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.5);
		b.AddMiddlePoint(0.25);
		b.Blur();

		auto mesh = b.GetMesh();
		meshes[mesh->GetMeshID()] = mesh;
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

}