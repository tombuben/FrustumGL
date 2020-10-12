#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"
#include "ObjLoader.h"

Mesh::Mesh()
{
	loaded = false;
}

Mesh::Mesh(std::string objPath)
{
	loaded = Load(objPath);
}


Mesh::Mesh(GLuint VBO, std::vector<unsigned int> indices, AABB b)
{
	verticesVBO = VBO;
	isShared = true;
	loaded = true;
	sendIndicesToGPU(indices);
	bounds.m[0] = b.m[0];
	bounds.m[1] = b.m[1];
	bounds.center = b.center;
}

Mesh::Mesh(std::vector<glm::vec3>& vertices, std::vector<unsigned int> indices, AABB b)
{
	sendVerticesToGPU(vertices);
	sendIndicesToGPU(indices);
	bounds.m[0] = b.m[0];
	bounds.m[1] = b.m[1];
	bounds.center = b.center;
}

void Mesh::DeleteFromGPU()
{
	std::cout << "Deleting GPU data of " << this << std::endl;
	if (!isShared)
		glDeleteBuffers(1, &verticesVBO);
	glDeleteBuffers(1, &indicesVBO);
}

bool Mesh::Load(std::string objPath)
{
	if (loaded == true)
	{
		std::cerr << "Object already loaded!" << std::endl;
		return true;
	}
	// We do not keep vertices in RAM, only GPU memory.
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	loaded = ObjLoader::Load(objPath, vertices, indices, bounds.m);
	if (!loaded)
	{
		std::cout << "Mesh was not loaded!" << std::endl;
		return false;
	}
	bounds.center = (bounds.m[0] + bounds.m[1]) / 2.0f;

	return sendVerticesToGPU(vertices) && sendIndicesToGPU(indices);
}

bool Mesh::sendVerticesToGPU(std::vector<glm::vec3> const& vertices)
{
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0].x, GL_STATIC_DRAW);
	isShared = false;

	GLenum err;
	loaded = (err = glGetError()) == GL_NO_ERROR;
	if (err != GL_NO_ERROR)
	{
		while (err != GL_NO_ERROR)
		{
			std::cerr << "Error while loading to gpu" << std::endl;
			err = glGetError();
		}
		return false;
	}
	return true;
}

bool Mesh::sendIndicesToGPU(std::vector<unsigned int> const& indices)
{
	glGenBuffers(1, &indicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	indicesVBO_size = indices.size();

	GLenum err;
	loaded = (err = glGetError()) == GL_NO_ERROR;
	if (err != GL_NO_ERROR)
	{
		while (err != GL_NO_ERROR)
		{
			std::cerr << "Error while loading to gpu" << std::endl;
			err = glGetError();
		}
		return false;
	}
	return true;
}

void Mesh::Draw(cCamera& cam, ShaderProgram& prog, const glm::mat4 model) const
{
	if (!loaded) return;
	glUseProgram(prog.programID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);

	// Set MVP matrix
	GLuint MatrixID = glGetUniformLocation(prog.programID, "MVP");
	glm::mat4 MVP = cam.getVP() * model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indicesVBO_size,   // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
}