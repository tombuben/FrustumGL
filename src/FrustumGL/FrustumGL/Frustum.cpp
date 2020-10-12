#include <glm/gtc/matrix_transform.hpp>
#include "Frustum.h"
#include "Mesh.h"

Mesh Frustum::box = Mesh();


Frustum::Frustum(cCamera cam) : Frustum(cam.getVP())
{}


Frustum::Frustum(glm::mat4 VP) : VP(VP)
{
	invVP = glm::inverse(VP);

	for (int i = 4; i--; ) planes[LEFTPLANE][i] = VP[i][3] + VP[i][0]; // left
	for (int i = 4; i--; ) planes[RIGHTPLANE][i] = VP[i][3] - VP[i][0]; //right
	for (int i = 4; i--; ) planes[BOTTOMPLANE][i] = VP[i][3] + VP[i][1]; //bottom
	for (int i = 4; i--; ) planes[TOPPLANE][i] = VP[i][3] - VP[i][1]; //top
	for (int i = 4; i--; ) planes[NEARPLANE][i] = VP[i][3] + VP[i][2]; //near
	for (int i = 4; i--; ) planes[FARPLANE][i] = VP[i][3] - VP[i][2]; //far

	//Normalize
	for (int i = 0; i < 6; i++)
	{
		planes[i] /= -glm::length(glm::vec3(planes[i].x, planes[i].y, planes[i].z));
	}
}

void Frustum::Draw(cCamera& cam, ShaderProgram& prog, glm::mat4 model) const
{
	if (!box.loaded)
	{
		box.Load("../data/Cube.obj");
	}

	model = invVP * model;

	glEnable(GL_BLEND); //Enable blending.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function

	////Uncomment for wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	Frustum::box.Draw(cam, prog, model);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_BLEND); //Enable blending.
}

