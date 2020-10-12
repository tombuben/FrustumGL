#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "SceneLoader.h"


SceneLoader::SceneLoader(const std::string path)
{
	std::fstream file;
	file.open(path, std::ios::in);
	if (!file.is_open())
	{
		std::cerr << "Failed opening file " << path << " for reading." << std::endl;
		return;
	}

	namespace fs = std::filesystem;
	auto parent = fs::absolute(fs::path(path).parent_path());
	std::string line;
	while (std::getline(file, line))
	{
		std::stringstream words(line);
		std::string firstWord;
		words >> firstWord;
		if (firstWord == "M" || firstWord == "m" || firstWord == "Mesh" || firstWord == "mesh")
		{
			words >> std::ws;
			std::getline(words, line);
			std::string meshpath = parent.string() + "/" + line;
			//std::cout << "Mesh path is: " << meshpath << std::endl;
			Meshes.push_back(Mesh(meshpath));
		}
		else if (firstWord == "SM" || firstWord == "sm" || firstWord == "SplitMesh" || firstWord == "splitmesh")
		{
			int trisInLeaf;
			words >> trisInLeaf;
			words >> std::ws;
			std::getline(words, line);
			std::string meshpath = parent.string() + "/" + line;
			SplitMeshes.push_back(SplitMesh(trisInLeaf, meshpath));
		}

		else if (firstWord == "O" || firstWord == "o" || firstWord == "Obj" || firstWord == "obj")
		{
			int meshID;
			float posX = 0, posY = 0, posZ = 0;
			float scale = 1;
			float angle = 0;
			float rotX = 0, rotY = 1, rotZ = 0;
			words >> meshID >> posX >> posY >> posZ >> scale >> angle >> rotX >> rotY >> rotZ;
			//std::cout << "Object: ID " << meshID << ", " << posX << "x " << posY << "y " << posZ << "z, Scale: " << scale
			//		  << ", Angle: " << angle << ", " << rotX << "x " << rotY << "y " << rotZ << "z" << std::endl;
			Objects.push_back(RenderObject(Meshes.at(meshID), glm::vec3(posX, posY, posZ), scale, angle, glm::vec3(rotX, rotY, rotZ)));
		}

		else if (firstWord == "SO" || firstWord == "so" || firstWord == "SplitObj" || firstWord == "splitobj")
		{
			int splitmeshID;
			float posX = 0, posY = 0, posZ = 0;
			float scale = 1;
			float angle = 0;
			float rotX = 0, rotY = 1, rotZ = 0;
			words >> splitmeshID >> posX >> posY >> posZ >> scale >> angle >> rotX >> rotY >> rotZ;

			for (auto& mesh : SplitMeshes.at(splitmeshID).Meshes)
			{
				Objects.push_back(RenderObject(mesh, glm::vec3(posX, posY, posZ), scale, angle, glm::vec3(rotX, rotY, rotZ)));
			}
		}

		else if (firstWord == "b" || firstWord == "B" || firstWord == "Bezier" || firstWord == "bezier")
		{
			std::cout << "Animation in scene" << std::endl;
			words >> animlen >> animcount;
			std::cout << "Animcount: " << animcount << std::endl;
			for (int i = 0; i < 4; i++)
			{
				float p1, p2, p3;
				words >> p1 >> p2 >> p3;
				bezier[i] = glm::vec3(p1, p2, p3);
			}
		}

		else
		{
			//Every other line ignored as a comment
		}
	}

}

SceneLoader::~SceneLoader()
{
	for(auto mesh : Meshes)
	{
		mesh.DeleteFromGPU();
	}

	for (auto splitmesh : SplitMeshes)
	{
		splitmesh.DeleteFromGPU();
	}
}

