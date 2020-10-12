#pragma once

#include "AABB.h"
#include "RenderObject.h"

#define TYPE_INNER 0
#define TYPE_LEAF 1
#define TYPE_TOSPLIT 2
#define TYPE_UNINITIALIZED 3

/**
 * Node of a bounding volume hierarchy.
 * When provided with list of objects, takes care of midpoint splitting and frustum culling
 */
class BVHNode
{
	union c
	{
		std::vector<RenderObject>* objectVec;	//type == 2, leaf, before splitting
		RenderObject* object;					//type == 1, leaf of a split BVH (can be empty with nullptr)
		std::array<BVHNode, 2>* children;		//type == 0, inner node
	};
	c contents;
	short type;
	glm::vec3 min_center, max_center;
public:
	/**
	 * Axis aligned bounding box of the node
	 */
	AABB bounds;
	
	/**
	 * Creates a unitialized BVH. Should be only used internally, since C++ doesn't allow array without default initializer
	 */
	BVHNode();
	
	/**
	 * Creates a single node containing a list of objects
	 * @param objects - a vector of objects to be included inside the node. Will be copied inside the node.
	 */
	BVHNode(std::vector<RenderObject>& objects);

	/**
	 * Creates a single node containing a list of objects
	 * @param objects a pointer to a vector of objects included inside the node. Node takes care of deleting the vector and the pointer shouldn't be used anymore!
	 */
	BVHNode(std::vector<RenderObject>* objects);

	/**
	 * Destructor
	 */
	~BVHNode();

	/**
	 * Recursively constructs a BVH using the midpoint split (midpoint calculated from AABB centers)
	 * @param sizetarget how many objects should be at most in a single node
	 */
	void MidpointSplit(unsigned int sizetarget = 1);
	
	/**
	 * Recursively draw the BVH with optional frustum culling
	 * @param cam a camera to use
	 * @param prog shaders to draw with
	 * @param f pointer to a frustum to cull with (no culling if nullptr)
	 * @param mask a mask of which frustum planes to cull with (default is all planes)
	 */
	void Draw(cCamera& cam, ShaderProgram& prog, Frustum *f = nullptr, short mask = 0b111111);

	/**
	 * Recursively traverse the structure and calculate how many drawcalls would be used
	 * @param f pointer to a frustum to cull with (no culling if nullptr)
	 * @param mask a mask of which frustum planes to cull with (default is all planes)
	 */
	unsigned int CountCalls(Frustum *f = nullptr, short mask = 0b111111);
	
	/**
	 * Recursively draw the BVH bounding boxes of a given level
	 * @param cam a camera to use
	 * @param prog shaders to draw with
	 * @param level level of the boxes in the tree drawn
	 */
	void DrawBounds(cCamera& cam, ShaderProgram& prog, int level = 0);
};
