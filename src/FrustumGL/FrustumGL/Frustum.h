#pragma once
#include <glm/mat4x4.hpp>
#include <array>
#include "cCamera.h"
#include "ShaderProgram.h"

#ifndef MESH_OBJ
class Mesh;
#endif // !MESH_OBJ


#define LEFTPLANE 0
#define RIGHTPLANE 1
#define BOTTOMPLANE 2
#define TOPPLANE 3
#define NEARPLANE 4
#define FARPLANE 5

/**
 * represents the frustum and its planes
 */
class Frustum
{
	static Mesh box;
	glm::mat4 VP;
	glm::mat4 invVP;
public:
	std::array<glm::vec4, 6> planes; ///< Array of implicit planes forming the frustum

	/**
	 * Create a frustum from a camera
	 * @param cam camera to create frustum from
	 */
	Frustum(cCamera cam);

	/**
	 * Create a frustum from a View Projection matrix
	 * @param VP matrix to create camera from
	 */
	Frustum(glm::mat4 VP);

	/**
	 * Draw a wireframe of the frustum
	 * @param cam a camera to draw with
	 * @param prog shaders to draw with
	 * @param model model matrix to use (position of the frustum)
	 */
	void Draw(cCamera& cam, ShaderProgram& prog, glm::mat4 model = glm::mat4(1.0f)) const;
};

