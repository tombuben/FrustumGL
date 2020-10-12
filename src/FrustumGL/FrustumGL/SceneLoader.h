#pragma once
#include <string>

#include "Mesh.h"
#include "SplitMesh.h"
#include "RenderObject.h"

/**
 * Simple loader for a text file with a scene representation
 * The text file format contains one line per record, with three possible record types:
 * - Mesh record: Represented by keywords "m", "M", "mesh" or "Mesh"
 *   After the keyword, a relative path from the text file to an obj file containing the mesh is located
 *
 * - Object record: Represented by keywords "o", "O", "obj" or "Obj"
 *   After the keyword, a set of values is given in order:
 *   [Mesh ID] [Triplet of X Y Z world coordinates] [Scale (default 1)] [rotation in radians (default 0)] [Triplet representing the axis vector of rotation (default up axis)]
 *
 * - Animation: Represented by keyword "b", "B", "bezier" or "Bezier"
 *   After the keyword, a set of values is given in order:
 *   [Duration in seconds] [Triplet of first control point] [Triplet second point] [Triplet third point] [Triplet last point]
 * 
 * - SplitMesh record: Represented by keywords "sm", "SM", "splitmesh" or "SplitMesh"
 *   Loads a mesh and splits it into smaller chunks via the midpoint method, maximum triangles in chunk is defined by user
 *   After the keyword, a set of values is given in order:
 *   [max tris in submesh] [relative path to obj file]
 *
 * - SplitObject record: Represented by keywords "so", "SO", "splitobj" or "SplitObj"
 *   SplitMeshes are treated separately from meshes
 *   After the keyword, a set of values is given in order:
 *   [SplitMesh ID] [Triplet of X Y Z world coordinates] [Scale (default 1)] [rotation in radians (default 0)] [Triplet representing the axis vector of rotation (default up axis)]
 */
class SceneLoader
{
	std::vector<SplitMesh> SplitMeshes;
	std::vector<Mesh> Meshes;
public:
	std::array<glm::vec3, 4> bezier; ///< A bezier curve of the camera animation
	float animlen = 0; ///< The duration of the camera animation
	int animcount = -1; ///< How many times the animation will run

	std::vector<RenderObject> Objects; ///< List of objects in scene

	/**
	 * Loads the scene and populates the object file
	 * @param path Path to file describing the scene
	 */
	SceneLoader(const std::string path);

	/**
	 * Cleans up loaded data from the GPU
	 */
	~SceneLoader();
};

