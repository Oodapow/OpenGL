#pragma once
#include <random>
#include <iostream>

#include <Core/Engine.h>
#include <Core/GPU/Texture2D.h>
#include "mObject.h"

class mSkyBox : public mObject {
public:
	void BindTextures() {
		cubeMap->BindToTextureUnit(GL_TEXTURE0);
	}

	void InitTextureMaps() {
		std::vector<std::string> faces
		{
			RESOURCE_PATH::TEXTURES + "Daylight Box_Right.bmp",
			RESOURCE_PATH::TEXTURES + "Daylight Box_Left.bmp",
			RESOURCE_PATH::TEXTURES + "Daylight Box_Top.bmp",
			RESOURCE_PATH::TEXTURES + "Daylight Box_Bottom.bmp",
			RESOURCE_PATH::TEXTURES + "Daylight Box_Front.bmp",
			RESOURCE_PATH::TEXTURES + "Daylight Box_Back.bmp"
		};
		cubeMap = new Texture2D();
		cubeMap->LoadCubeMap(faces);
	}

	Shader* InitShader() {
		_shader = new Shader(shader.c_str());
		_shader->AddShader(RESOURCE_PATH::SHADERS + "BoxVertexShader.glsl", GL_VERTEX_SHADER);
		_shader->AddShader(RESOURCE_PATH::SHADERS + "BoxFragmentShader.glsl", GL_FRAGMENT_SHADER);
		_shader->CreateAndLink();
		_shader->loc_textures[0] = glGetUniformLocation(_shader->program, "skybox");
		return _shader;
	}

	Mesh* InitMesh(float side) {
		side = side / 2;
		std::vector<glm::vec3> positions = {
			{ 0.0, 0.0, 0.0 }, { side, 0.0, 0.0 }, { side, side, 0.0 }, { 0.0, side, 0.0 },
			{ 0.0, 0.0, side }, { side, 0.0, side }, { side, side, side }, { 0.0, side, side },
		};

		for (auto it = positions.begin(); it != positions.end(); it++)
		{
			it->x = it->x * 2 - side;
			it->y = it->y * 2 - side;
			it->z = it->z * 2 - side;
		}

		std::vector<unsigned short> indices = {
			0, 1, 2, 0, 2, 3,
			1, 5, 6, 1, 2, 6,
			5, 4, 7, 5, 7, 6,
			4, 0, 3, 4, 3, 7,
			0, 1, 5, 0, 4, 5,
			3, 2, 6, 3, 6, 7
		};

		_mesh = new Mesh(mesh);
		_mesh->InitFromData(positions, indices);
		return _mesh;
	}
private:
	Texture2D* cubeMap = NULL;

	Mesh* _mesh;
	Shader* _shader;
};
