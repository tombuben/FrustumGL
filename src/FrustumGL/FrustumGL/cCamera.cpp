#include <iostream>
#include "cCamera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

cCamera::cCamera(glm::vec3 i_position,
				 glm::vec3 i_direction,
				 glm::vec3 i_upVector,
				 float i_FoV,
				 float i_aspect,
				 float i_near,
				 float i_far)
{
	position = i_position;
	direction = i_direction;
	upVector = i_upVector;
	FoV = i_FoV;
	aspect = i_aspect;
	near = i_near;
	far = i_far;
	rotate(0, 0);
}

glm::mat4 cCamera::getViewMat()
{
	return glm::lookAt(position, position + direction, upVector);
}

glm::mat4 cCamera::getProjectionMat()
{
	return glm::perspective(
		glm::radians(FoV),
		aspect,
		near,
		far
	);
}

glm::mat4 cCamera::getVP()
{
	return getProjectionMat() * getViewMat();
}

void cCamera::move(glm::vec3 movedirection)
{
	position += strafe * movedirection.x + up * movedirection.y - forward * movedirection.z;
}

void cCamera::rotate(float verticalAngle, float horizontalAngle)
{
	direction = glm::rotateY(direction, verticalAngle);
	strafe = glm::normalize(glm::cross(direction, upVector));
	direction = glm::rotate(direction, horizontalAngle, strafe);
	direction = glm::normalize(direction);

	forward = direction;
	up = glm::normalize(glm::cross(forward, strafe));
}
