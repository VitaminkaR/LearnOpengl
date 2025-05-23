#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

namespace LOGL
{
	std::string Shader::readFile(const char* filePath)
	{
		std::ifstream file(filePath);
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			std::string str;
			std::string file_contents;
			while (!file.eof())
			{
				std::getline(file, str);
				file_contents += str;
				file_contents.push_back('\n');
			}
			file.close();
			return file_contents;
		}
		catch (std::ifstream::failure& e)
		{
			LOGL::error("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
		}
	}

	void Shader::checkCompileErrors(GLuint shader, const std::string& type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				LOGL::error("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s", type.c_str(), infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				LOGL::error("ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s", type.c_str(), infoLog);
			}
		}
	}

	Shader::Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vShaderStr;
		std::string fShaderStr;
		try
		{
			vShaderStr = readFile(vertexPath);
			fShaderStr = readFile(fragmentPath);
		}
		catch (std::ifstream::failure& e)
		{
			LOGL::error("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: ");
		}
		const char* vShaderCode = vShaderStr.c_str();
		const char* fShaderCode = fShaderStr.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void Shader::use()
	{
		glUseProgram(ID);
	}

	void Shader::setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void Shader::setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::setMat4(const std::string& name, const glm::mat4& value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void Shader::setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(glm::vec3(x, y, z)));
	}

}