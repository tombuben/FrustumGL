#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "AABB.h"
#include "cCamera.h"
#include "ShaderProgram.h"

#define MESH_OBJ

/**
 * Simple class taking care of meshes
 * Supports loading simple obj files and drawing them.
 */
class Mesh
{
	bool sendVerticesToGPU(std::vector<glm::vec3> const& vertices);
	bool sendIndicesToGPU(std::vector<unsigned int> const& indices);
public:
	bool loaded = false; ///< Flag whether or not the mesh is already loaded
	AABB bounds; ///< Bounding box of the mesh

	GLuint verticesVBO; ///< Index of the vertices Vertex Buffer Object
	bool isShared = false; ///< Is the vertices data shared or not (when deleting from GPU, only SharedMesh can delete shared vertices VBO)
	GLuint indicesVBO; ///< Index of the indices Vertex Buffer Object
	unsigned int indicesVBO_size; ///< number of unsigned int elements in the indices buffer

	/**
	 * Initializes a unloaded object
	 */
	Mesh();


	/**
	 * Initializes a mesh with an obj mesh
	 * @param objPath path to an obj file containing the mesh
	 */
	Mesh(std::string objPath);

	/**
	 * Initializes a mesh with given vertices, triangles and bounds
	 * @param vertices a list of vertices
	 * @param indices a list of indices creating triangles
	 * @param bounds bounds of the point cloud in vertices
	 */
	Mesh(std::vector<glm::vec3>& vertices, std::vector<unsigned int> indices, AABB bounds);

	/**
	 * Initializes a mesh with given vertices, triangles and bounds
	 * @param verticesVBO VBO object with vertices to use (not managed by this)
	 * @param indices a list of indices creating triangles
	 * @param bounds bounds of the point cloud in vertices
	 */
	Mesh(GLuint verticesVBO, std::vector<unsigned int> indices, AABB bounds);

	/**
	 * Deletes the mesh data from the GPU, separate from the destructor (because of copying),
	 * called via the SceneLoader destructor.
	 */
	void DeleteFromGPU();

	/**
	 * Loads an obj file into an uninitialized mesh
	 * @param objPath path to an obj file containing the mesh
	 */
	bool Load(std::string objPath);

	/**
	 * Draw the mesh
	 * @param cam a camera
	 * @param prog shaders to draw with
	 * @param model model matrix to use
	 */
	void Draw(cCamera& cam, ShaderProgram& prog, const glm::mat4 model = glm::mat4(1.0f)) const;
};

