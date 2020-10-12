#pragma once
#include <string>
#include "Mesh.h"
#include "AABB.h"

/**
 * Class representing a splittable mesh. Loads an object and given a maximum triangles in Mesh creates a set of submeshes using midpoint split.
 * The split meshes Share a vertices VBO in the GPU memory, but each has their own indices VBO
 */
class SplitMesh
{
	void midpointSplit(unsigned int trisInLeaf,
					   std::vector<glm::vec3>& vertices,
					   std::vector<unsigned int>& indices,
					   AABB b, AABB centerbounds);
	bool sendVerticesToGPU(std::vector<glm::vec3>& vertices);
public:
	GLuint verticesVBO; ///< Index of the shared vertices Vertex Buffer Object

	AABB bounds; ///< Bounds of the total object
	std::vector<Mesh> Meshes; ///< List of meshes with the maximum triangles goal managed

	/**
	 * Create a split mesh - a collection of submeshes with a maximum number of triangles in Leaf
	 * @param trisInLeaf maximum number of triangles in a submesh
	 * @param meshpath a path to the .obj file to load
	 */
	SplitMesh(unsigned int trisInLeaf, std::string meshpath);

	/**
	 * Deletes the mesh data from the GPU, separate from the destructor (because of copying),
	 * called via the SceneLoader destructor.
	 */
	void DeleteFromGPU();
};

