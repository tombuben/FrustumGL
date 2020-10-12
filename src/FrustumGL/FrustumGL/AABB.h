#pragma once
#include <glm/vec3.hpp>
#include <vector>

#include "cCamera.h"
#include "ShaderProgram.h"
#include "Frustum.h"

#ifndef MESH_OBJ
class Mesh;
#endif // !MESH_OBJ

#define INSIDE  1
#define OUTSIDE -1
#define INTERSECTS 0

/**
 * Axis aligned bounding box
 */
class AABB
{
	static Mesh box;

	short lastFrustumPlane = 0;
public:
	glm::vec3 m[2]; ///< Array with the minimum and maximum corner
	glm::vec3 center = glm::vec3(0); ///< the center of the AABB

	AABB(); ///< Default constructor, only used in special cases

	/**
	 * A bounding box is defined by the minimum and maximum corner
	 * @param min minimum corner vertex
	 * @param max maximum corner vertex
	 */
	AABB(glm::vec3 min, glm::vec3 max);

	/**
	 * Draw the bounding box, with the minimum and maximum points highlighted
	 * @param cam a camera to use
	 * @param prog shaders to draw with
	 * @param model model matrix to use
	 */
	void Draw(cCamera& cam, ShaderProgram& prog, glm::mat4 model = glm::mat4(1.0f)) const;

	/**
	 * Grants access to all 8 corners of the box
	 * @param i which corner to use (0 is min, 1 is max)
	 */
	glm::vec3 getCorner(short i);

	/**
	 * Fast frustum culling based on [1]. Returns if box is INSIDE, INTERSECTS, or is OUTSIDE the frustum
	 * Using:
	 *  fast plane box intersection test
	 *  temporal coherency (box OUTSIDE returns faster if OUTSIDE in predious test)
	 *  plane masking (masks of inner BVH nodes do not need to be tested against all frustum planes)
	 * [1]:Sykora, Daniel, and Josef Jelinek. "Efficient View Frustum Culling." Central European Seminar on Computer Graphics. 2002.
	 * @param f frustum to chech against
	 * @param in_mask bit mask of which planes to use
	 * @param[out] out_mask a bit mask to use if entering child nodes. Only valid if INTERSECTS
	 * 
	 * @return whether the box is INSIDE, INTERSECTS, or is OUTSIDE the frustum
	 */
	short checkFrustum(Frustum& f, const short in_mask, short& out_mask);

	/**
	 * insert a point to the AABB, either expanding or keeping the current bounds
	 * @param point newly inserted point
	 */
	void insert(glm::vec3 point);
};

