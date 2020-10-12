#include <iostream>
#include <chrono>
#include <math.h>
#include "SplitMesh.h"
#include "ObjLoader.h"

SplitMesh::SplitMesh(unsigned int trisInLeaf, std::string objPath)
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	if (!ObjLoader::Load(objPath, vertices, indices, bounds.m))
		return;
	sendVerticesToGPU(vertices);

	bounds.center = (bounds.m[0] + bounds.m[1]) / 2.0f;

	std::cout << "-- Splitting Mesh --" << std::endl;
	auto begin = std::chrono::high_resolution_clock::now();
	midpointSplit(trisInLeaf, vertices, indices, bounds, AABB());
	auto end = std::chrono::high_resolution_clock::now();
	auto dur = end - begin;
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
	std::cout << "-- Finished Mesh split in " << ms << " ms --" << std::endl;
}

void SplitMesh::DeleteFromGPU()
{
	std::cout << "Deleting shared vertex GPU data of " << this << std::endl;
	glDeleteBuffers(1, &verticesVBO);
}

void SplitMesh::midpointSplit(unsigned int trisInLeaf,
							  std::vector<glm::vec3>& vertices,
							  std::vector<unsigned int>& indices,
							  AABB b, AABB centerbounds)
{
	unsigned int trisCount = indices.size() / 3;
	if (trisCount <= trisInLeaf)
	{
		Meshes.push_back(Mesh(verticesVBO, indices, b));
		return;
	}

	if (isnan(centerbounds.center.x))
	{
		glm::vec3 center = (vertices[indices[0]] + vertices[indices[1]] + vertices[indices[2]]) / 3.0f;
		centerbounds.m[0] = centerbounds.m[1] = center;
		for (long unsigned int i = 0; i < indices.size(); i += 3)
		{
			glm::vec3 center = (vertices[indices[i]] + vertices[indices[i + 1]] + vertices[indices[i + 2]]) / 3.0f;
			centerbounds.insert(center);
		}
		centerbounds.center = (centerbounds.m[0] + centerbounds.m[1]) / 2.0f;
	}

	//Calculate midpoint
	glm::vec3 diff = glm::abs(centerbounds.m[1] - centerbounds.m[0]);
	short splitaxis = diff.x > diff.y ? (diff.x > diff.z ? 0 : 2) : (diff.y > diff.z ? 1 : 2);
	float midpoint = (centerbounds.m[0][splitaxis] + centerbounds.m[1][splitaxis]) / 2;

	AABB leftbounds, rightbounds;
	AABB leftcenter, rightcenter;
	std::vector<unsigned int> leftIndices, rightIndices;
	//For each triangle
	for (long unsigned int i = 0; i < indices.size(); i += 3)
	{
		glm::vec3 center = (vertices[indices[i]] + vertices[indices[i + 1]] + vertices[indices[i + 2]]) / 3.0f;
		if (center[splitaxis] < midpoint)
		{
			leftIndices.push_back(indices[i]);
			leftIndices.push_back(indices[i + 1]);
			leftIndices.push_back(indices[i + 2]);
			leftbounds.insert(vertices[indices[i]]);
			leftbounds.insert(vertices[indices[i + 1]]);
			leftbounds.insert(vertices[indices[i + 2]]);
			leftcenter.insert(center);
		}
		else
		{
			rightIndices.push_back(indices[i]);
			rightIndices.push_back(indices[i + 1]);
			rightIndices.push_back(indices[i + 2]);
			rightbounds.insert(vertices[indices[i]]);
			rightbounds.insert(vertices[indices[i + 1]]);
			rightbounds.insert(vertices[indices[i + 2]]);
			rightcenter.insert(center);
		}
	}

	leftbounds.center = (leftbounds.m[0] + leftbounds.m[1]) / 2.0f;
	rightbounds.center = (rightbounds.m[0] + rightbounds.m[1]) / 2.0f;
	leftcenter.center = (leftcenter.m[0] + leftcenter.m[1]) / 2.0f;
	rightcenter.center = (rightcenter.m[0] + rightcenter.m[1]) / 2.0f;

	midpointSplit(trisInLeaf, vertices, leftIndices, leftbounds, leftcenter);
	midpointSplit(trisInLeaf, vertices, rightIndices, rightbounds, rightcenter);
}

bool SplitMesh::sendVerticesToGPU(std::vector<glm::vec3>& vertices)
{

	std::cout << "Loading shared vertex GPU data of " << this << std::endl;
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0].x, GL_STATIC_DRAW);

	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		while (err != GL_NO_ERROR)
		{
			std::cerr << "Error while loading SplitMesh vertices to GPU" << std::endl;
			err = glGetError();
		}
		return false;
	}
	return true;
}
