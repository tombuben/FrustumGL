#pragma once
#include <string>
#include <vector>


/**
 * Loader for OBJ files. Handles very simple obj files (Tris and quads only)
 * Saves them to a binary representation for faster loading next time (provided .obj files can be loaded for several minutes, binary caching speeds up stuff)
 */
class ObjLoader
{
	static void SaveBin(std::string BinPath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3* m);
	static bool LoadBin(std::string BinPath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3* m);
	static bool LoadObj(std::string objPath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3* m);
public:
	/**
	 * function to load an obj file
	 * @param objPath path to the .obj file
	 * @param vertices reference to where should vertices be written
	 * @param indices reference to where should vertices be written
	 * @param m pointer where bounds of the loaded mesh will be written
	 */
	static bool Load(std::string objPath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3* m);
};

