#pragma once
#include <string>
#include <GL/glew.h>


/**
 * Extremely simple representation of a OpenGL shader program, only with MVP uniform
 */
class ShaderProgram
{
public:
	GLuint programID; ///< Shader program the object represents
	GLuint matrixID;  ///< MVP matrix the program has
	bool loaded = false; ///< Whether or not the program loaded fine

	/**
	 * Creates and compiles an OpenGL shader program
	 * @param vertexfile path to vertex shader file
	 * @param fragmentfile path to fragment shader file
	 */
	ShaderProgram(std::string vertexfile, std::string fragmentfile);

	/**
	 * Unloads the shader program from the GPU
	 */
	~ShaderProgram();
};

