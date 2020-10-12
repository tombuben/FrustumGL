#include "ShaderProgram.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

ShaderProgram::ShaderProgram(std::string vertexfile, std::string fragmentfile)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexfile, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		std::cerr << "Could not open vertex shader file! " << vertexfile << std::endl;
		throw "Could not open vertex shader file!";
		return;
	}

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentfile, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else {
		std::cerr << "Could not open fragment shader file! " << fragmentfile << std::endl;
		throw "Could not open fragment shader file!";
		return;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;
	
	//Compile vertex shader
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);
	
	//Check compilation
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> v(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, v.data());
		throw std::string(v.begin(), v.end());
	}

	//Compile fragment shader
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check compilation
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> v(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, v.data());
		throw std::string(v.begin(), v.end());
	}

	programID = glCreateProgram();
	glAttachShader(programID, VertexShaderID);
	glAttachShader(programID, FragmentShaderID);
	glLinkProgram(programID);

	// Check linking
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> v(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, v.data());
		throw std::string(v.begin(), v.end());
	}

	glDetachShader(programID, VertexShaderID);
	glDetachShader(programID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}


ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(programID);
}
