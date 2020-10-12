#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <glm/vec3.hpp>
#include <cctype>
#include "ObjLoader.h"


bool ObjLoader::Load(std::string objPath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3* m)
{
	//Needed since Linux adds some garbage character at the end
	if(!std::isalpha(objPath[objPath.size()-1]))
		objPath = objPath.substr(0, objPath.size()-1);

	bool fileloaded;
	if (std::filesystem::exists(objPath + ".bin"))
	{
		std::cout << "----------------" << std::endl;
		std::cout << "Loading binary representation" << std::endl;
		fileloaded = LoadBin(objPath + ".bin", vertices, indices, m);
	}
	else
	{
		if (!LoadObj(objPath, vertices, indices, m)){
			std::cout << "Failed loading" << std::endl;
			return false;
		}

		std::cout << "Saving to binary" << std::endl;
		SaveBin(objPath + ".bin", vertices, indices, m);
		m[0] = m[1] = glm::vec3();
		std::cout << "Testing load binary" << std::endl;
		fileloaded = LoadBin(objPath + ".bin", vertices, indices, m);
	}
	return fileloaded;
}

bool ObjLoader::LoadObj(std::string objPath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3* m)
{
	std::cout << "----------------" << std::endl;
	std::cout << "Loading OBJ file " << objPath << std::endl;

	float fmax = std::numeric_limits<float>::max();
	float fmin = std::numeric_limits<float>::min();
	glm::vec3 min(fmax, fmax, fmax), max(fmin, fmin, fmin);

	std::fstream file;
	std::cout << "File size " << std::filesystem::file_size(objPath) << " bytes. Large meshes can take a while to load." << std::endl;
	file.open(objPath, std::ios::in);
	if (!file.is_open())
	{
		std::cerr << "Failed opening file " << objPath << " for reading." << std::endl;
		return false;
	}

	std::string line;
	bool triangular = true;
	while (std::getline(file, line))
	{
		std::stringstream words(line);
		std::string firstWord;
		words >> firstWord;
		if (firstWord == "v")
		{
			glm::vec3 vertex;
			if (!(words >> vertex.x >> vertex.y >> vertex.z))
			{
				std::cerr << "failed reading vertex. Line with error:" << std::endl << line << std::endl;
				file.close();
				return false;
			}
			for (int i = 0; i < 3; i++)
			{
				min[i] = (vertex[i] < min[i]) ? vertex[i] : min[i];
				max[i] = (vertex[i] > max[i]) ? vertex[i] : max[i];
			}
			vertices.push_back(vertex);
		}
		else if (firstWord == "f")
		{
			unsigned int vertexIndex[3];
			std::string faceindices[3];

			if (!(words >> faceindices[0] >> faceindices[1] >> faceindices[2]))
			{
				std::cerr << "Face without three vertices!" << std::endl << line << std::endl;
				file.close();
				return false;
			}

			for (int i = 0; i < 3; i++)
			{
				std::stringstream ss(faceindices[i]);
				ss >> vertexIndex[i];
				indices.push_back(vertexIndex[i] - 1);
			}
			if (words >> faceindices[1]) // A quad is also supported
			{
				if (triangular)
				{
					triangular = false;
					std::cerr << "Face is not triangular, possible holes in the mesh!" << std::endl;
				}

				std::stringstream ss(faceindices[1]);
				ss >> vertexIndex[1];
				indices.push_back(vertexIndex[0] - 1);
				indices.push_back(vertexIndex[2] - 1);
				indices.push_back(vertexIndex[1] - 1);
			}

		}
		else
		{
			// Unsupported, treat as comment and do nothing
		}
	}
	file.close();

	m[0] = min;
	m[1] = max;
	std::cout << "Loaded, bounds: (" << min.x << "," << min.y << "," << min.z << ") (" << max.x << "," << max.y << "," << max.z << ")" << std::endl;
	return true;
}


void ObjLoader::SaveBin(std::string BinPath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3* m)
{
	std::ofstream fout(BinPath, std::ios::out | std::ios::binary);
	//fout << vertices.size();
	size_t vsize = vertices.size();
	fout.write((char*)&vsize, sizeof(size_t));
	std::cout << "Written vertices size " << vertices.size() << std::endl;
	fout.write((char*)&vertices[0], vertices.size() * sizeof(glm::vec3));

	//fout << indices.size();
	size_t isize = indices.size();
	fout.write((char*)&isize, sizeof(size_t));
	std::cout << "Written indices size " << indices.size() << std::endl;
	fout.write((char*)&indices[0], indices.size() * sizeof(unsigned int));
	std::cout << "saved min " << m[0].x << " " << m[0].y << " " << m[0].z << std::endl;
	std::cout << "saved max " << m[1].x << " " << m[1].y << " " << m[1].z << std::endl;
	fout.write((char*)&(m[0]), sizeof(glm::vec3) * 2);
	fout.close();
}
/*
saved min -1.36719 -0.984375 -0.851562
saved max 1.36719 0.984375 0.851562
*/


bool ObjLoader::LoadBin(std::string BinPath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3* m)
{
	std::cout << "reading bin file from: " << BinPath << std::endl;
	std::ifstream fin(BinPath, std::ios::in | std::ios::binary);
	size_t vsize;
	//fin >> vsize;
	fin.read((char*)&vsize, sizeof(size_t));
	std::cout << "vertices count: " << vsize << std::endl;
	vertices.resize(vsize);
	fin.read((char*)&vertices[0], vsize * sizeof(glm::vec3));
	size_t isize;
	//fin >> isize;
	fin.read((char*)&isize, sizeof(size_t));
	std::cout << "indices count: " << isize << std::endl;
	indices.resize(isize);
	fin.read((char*)&indices[0], isize * sizeof(unsigned int));
	fin.read((char*)&(m[0]), sizeof(glm::vec3) * 2);
	std::cout << "read bounds min " << m[0].x << " " << m[0].y << " " << m[0].z << std::endl;
	std::cout << "read bounds max " << m[1].x << " " << m[1].y << " " << m[1].z << std::endl;
	fin.close();
	return true;
}
