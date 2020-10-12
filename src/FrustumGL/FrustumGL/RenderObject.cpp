#include <iostream>
#include "RenderObject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

RenderObject::RenderObject(
	Mesh& mesh,
	glm::vec3 position,
	float scale,
	float angle,
	glm::vec3 rotationAxis)
	:mesh(mesh), position(position), scale(scale), angle(angle), rotationAxis(rotationAxis)
{
	if (angle == 0)
	{
		//if no rotation, bounds are same as bounds for the mesh
		this->bounds.m[0] = mesh.bounds.m[0];
		this->bounds.m[1] = mesh.bounds.m[1];
		this->bounds.center = mesh.bounds.center;
	}
	else
	{
		//calculating actual bounds of rotated mesh is way too expensive,
		//use rotated bounds of the mesh as base for final bounds
		glm::vec3 min, max;
		min = max = glm::rotate(mesh.bounds.getCorner(0), angle, rotationAxis);
		for (int i = 1; i < 8; i++)
		{
			glm::vec3 vertex = glm::rotate(mesh.bounds.getCorner(i), angle, rotationAxis);

			for (int i = 0; i < 3; i++)
			{
				min[i] = (vertex[i] < min[i]) ? vertex[i] : min[i];
				max[i] = (vertex[i] > max[i]) ? vertex[i] : max[i];
			}
		}
		this->bounds.m[0] = min;
		this->bounds.m[1] = max;
		this->bounds.center = glm::rotate(mesh.bounds.center, angle, rotationAxis);
	}
	this->bounds.m[0] *= scale;
	this->bounds.m[1] *= scale;
	this->bounds.center *= scale;

	this->bounds.m[0] += position;
	this->bounds.m[1] += position;
	this->bounds.center += position;
	//std::cout << "Created object with bounds centered at " << bounds.center.x << " " << bounds.center.y << " " << bounds.center.z << std::endl;

	if (rotationAxis == glm::vec3(0, 0, 0))
	{
		throw "Zero lenght rotation axis vector!";
	}

	model = glm::mat4(1.0);
	model = glm::translate(model, position);
	model = glm::rotate(model, angle, rotationAxis);
	model = glm::scale(model, glm::vec3(scale, scale, scale));
}

void RenderObject::Draw(cCamera& cam, ShaderProgram& prog) const
{
	mesh.Draw(cam, prog, model);
}
