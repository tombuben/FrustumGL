#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


/**
 * takes care of a perspective camera in scene, can return V and P matrices
 */
class cCamera
{

	glm::vec3 forward;
	glm::vec3 strafe;
	glm::vec3 up;

public:
	glm::vec3 position; ///< position of the camera
	glm::vec3 direction; ///< direction of the camera (not target, literal direction)
	glm::vec3 upVector; ///< up direction for the camera

	float FoV; ///< Field of view of the camera
	float aspect; ///< screen aspect ratio
	float near; ///< near plane distance
	float far; ///< far plane distance

	/** 
	 * Creates a basic camera.
	 * Basic camera has by default a 4/3 aspect and is looking to the center of coordinate system
	 * @param i_position position of the camera
	 * @param i_direction where the camera is looking towards
	 * @param i_upVector up direction for the camera
	 * @param i_FoV Field of view of the camera
	 * @param i_aspect screen aspect ratio
	 * @param i_near near plane distance
	 * @param i_far far plane distance
	 */
	cCamera(
		glm::vec3 i_position = glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3 i_direction = glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3 i_upVector = glm::vec3(0.0f, 1.0f, 0.0f),
		float i_FoV = 70, float i_aspect = 4.0f / 3.0f, float i_near = 0.1f, float i_far = 100.0f);

	/**
	 * Returns the view matrix
	 * @return view matrix
	 */
	glm::mat4 getViewMat();

	/**
	 * Returns the projection matrix
	 * @return projection matrix
	 */
	glm::mat4 getProjectionMat();

	/**
	 * Returns the VP matrix created by projection*view
	 * @return VP matrix
	 */
	glm::mat4 getVP();

	/**
	 * Moves the camera in the direction relative to the camera rotation
	 * @param direction direction to move towards
	 */
	void move(glm::vec3 direction);

	/**
	 * Rotates the camera around the vertical axis or up and down
	 * @param vertical angle of rotation around the vertical axis
	 * @param horizontal angle of rotation up and down
	 */
	void rotate(float vertical, float horizontal);
};

