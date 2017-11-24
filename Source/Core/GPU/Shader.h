#pragma once
#include <string>
#include <vector>
#include <list>
#include <functional>
#include <glm/glm.hpp>
#include <include/gl.h>

#define MAX_2D_TEXTURES		16
#define INVALID_LOC			-1

class Shader
{
	public:
		Shader(const char *name);
		~Shader();

		const char *GetName() const;
		GLuint GetProgramID() const;

		void Use() const;
		unsigned int Reload();

		void AddShader(const std::string &shaderFile, GLenum shaderType);
		void ClearShaders();
		unsigned int CreateAndLink();

		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setMat4(const std::string& name, glm::mat4 value) const;
		void setVec3(const std::string& name, glm::vec3 value) const;
		void setVec4(const std::string& name, glm::vec4 value) const;

		void BindTexturesUnits();
	private:
		static unsigned int CreateShader(const std::string &shaderFile, GLenum shaderType);
		static unsigned int CreateProgram(const std::vector<unsigned int> &shaderObjects);

	public:
		GLuint program;

		GLint loc_textures[MAX_2D_TEXTURES];
	private:

		bool compileErrors;

		struct ShaderFile
		{
			std::string file;
			GLenum type;
		};

		std::string shaderName;
		std::vector<ShaderFile> shaderFiles;
};
