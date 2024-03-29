#pragma once
#include <random>
#include <iostream>

#include <Core/Engine.h>
#include <Core/GPU/Texture2D.h>
#include "mObject.h"

class mWater : public mObject{
public:
	void BuildReflectionBuffer(int width, int height) {
		glGenFramebuffers(1, &reflectionFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);
		// create a color attachment texture
		glGenTextures(1, &reflectionTextureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, reflectionTextureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTextureColorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers(1, &reflectionRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, reflectionRbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, reflectionRbo); // now actually attach it
																									  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		_shader->loc_textures[0] = glGetUniformLocation(_shader->program, "reflection");
	}

	void BuildRefractionBuffer(int width, int height) {
		glGenFramebuffers(1, &refractionFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, refractionFramebuffer);
		// create a color attachment texture
		glGenTextures(1, &refractionTextureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, refractionTextureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTextureColorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

		glGenTextures(1, &refractionRbo);
		glBindTexture(GL_TEXTURE_2D, refractionRbo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionRbo, 0);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		_shader->loc_textures[1] = glGetUniformLocation(_shader->program, "refraction");
		_shader->loc_textures[4] = glGetUniformLocation(_shader->program, "depthMap");
	}

	void ResizeTextureBuffers(int width, int height) {
		BuildReflectionBuffer(width, height);
		BuildRefractionBuffer(width, height);
	}

	void BindReflectionBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);
	}

	void BindRefractionBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, refractionFramebuffer);
	}

	void BindTextures() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, reflectionTextureColorbuffer);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, refractionTextureColorbuffer);

		normalMap->BindToTextureUnit(GL_TEXTURE2);
		dudvMap->BindToTextureUnit(GL_TEXTURE3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, refractionRbo);

		glActiveTexture(GL_TEXTURE0);
	}

	void InitTextureMaps() {
		normalMap = new Texture2D();
		normalMap->Load2D(RESOURCE_PATH::TEXTURES + "normal.png");
		_shader->loc_textures[2] = glGetUniformLocation(_shader->program, "normalMap");

		dudvMap = new Texture2D();
		dudvMap->Load2D(RESOURCE_PATH::TEXTURES + "waterDUDV.png");
		_shader->loc_textures[3] = glGetUniformLocation(_shader->program, "dudvMap");
	}

	Shader* InitShader() {
		_shader = new Shader(shader.c_str());
		_shader->AddShader(RESOURCE_PATH::SHADERS + "WVShader.glsl", GL_VERTEX_SHADER);
		_shader->AddShader(RESOURCE_PATH::SHADERS + "WFShader.glsl", GL_FRAGMENT_SHADER);
		_shader->CreateAndLink();
		return _shader;
	}

	Mesh* InitMesh(int dx, int dz, float y, float line) {
		double lineLength = 1;
		std::string name = mesh;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned short> indices;

		glm::vec3 p1 = glm::vec3(-dx / 2 * line, y, -dz / 2 * line);
		glm::vec3 p2 = glm::vec3(-dx / 2 * line, y, dz / 2 * line);
		glm::vec3 p3 = glm::vec3(dx / 2 * line, y, -dz / 2 * line);
		glm::vec3 p4 = glm::vec3(dx / 2  * line, y, dz / 2 * line);

		positions.push_back(p1);
		positions.push_back(p2);
		positions.push_back(p3);
		positions.push_back(p4);

		glm::vec2 t1 = glm::vec2(0, dz * line);
		glm::vec2 t2 = glm::vec2(0, 0);
		glm::vec2 t3 = glm::vec2(dx * line, dz * line);
		glm::vec2 t4 = glm::vec2(dx * line, 0);

		texCoords.push_back(t1);
		texCoords.push_back(t2);
		texCoords.push_back(t3);
		texCoords.push_back(t4);

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);

		_mesh = new Mesh(name);
		_mesh->InitFromData(positions, texCoords, indices);
		return _mesh;
	}
private:
	unsigned int reflectionFramebuffer = -1;
	unsigned int reflectionTextureColorbuffer = -1;
	unsigned int reflectionRbo = -1;

	unsigned int refractionFramebuffer = -1;
	unsigned int refractionTextureColorbuffer = -1;
	unsigned int refractionRbo = -1;

	Texture2D* normalMap = NULL;
	Texture2D* dudvMap = NULL;

	Mesh* _mesh;
	Shader* _shader;
};
