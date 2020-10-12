#pragma once
#include <glm/vec3.hpp>
#include "Mesh.h"


/**
 * Simple class taking care of objects
 */
class RenderObject
{
public:
	Mesh& mesh; ///< Reference to a mesh the object is using
private:
	glm::vec3 position;
	float scale;
	float angle;
	glm::vec3 rotationAxis;
	glm::mat4 model;
public:
	AABB bounds; ///< Bounds of the object

	/**
	 * Creates a RenderObject of a mesh with given position, scale and (overly simplistic) rotation around an axis
	 * @param mesh which mesh to use
	 * @param position world space position
	 * @param scale scale of the object
	 * @param angle angle to rotate the object in radians
	 * @param rotationAxis vector representing an axis to rotate around
	 */
	RenderObject(Mesh& mesh, glm::vec3 position = glm::vec3(), float scale = 1.0, float angle = 0, glm::vec3 rotationAxis = glm::vec3(0.0, 1.0, 0.0));
	
	/**
	 * Draw the object
	 * @param cam a camera
	 * @param prog shaders to draw with
	 */
	void Draw(cCamera& cam, ShaderProgram& prog) const;
};

