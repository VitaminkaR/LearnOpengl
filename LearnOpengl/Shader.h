#pragma once

#include "glad/glad.h"
#include <string>
#include <iostream>
#include <fstream>
#include "logger.h"
#include "glm/glm.hpp"

namespace LOGL
{
	class Shader
	{
	public:
		GLuint ID;

		Shader(const char* vertexPath, const char* fragmentPath);

		void use();

		void setBool(const std::string& name, bool value) const;

		void setInt(const std::string& name, int value) const;

		void setFloat(const std::string& name, float value) const;

		void setMat4(const std::string& name, const glm::mat4& value) const;

		void setVec3(const std::string& name, const glm::vec3& value) const;

		void setVec3(const std::string& name, float x, float y, float z) const;
	private:
		std::string readFile(const char* filePath);
		void checkCompileErrors(GLuint shader, const std::string& type);
	};
}

