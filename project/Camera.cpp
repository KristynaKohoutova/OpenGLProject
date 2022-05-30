#include "Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

#include "Object.h"

// Default camera values
const float DEF_FOV = 45.0f; // degrees

Camera::Camera() 
	:mPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	mTargetPos(glm::vec3(0.0f, 0.0f, 0.0f)),
	mUp(0.0f, 1.0f, 0.0f),
	mRight(0.0f, 0.0f, 0.0f),
	WORLD_UP(0.0f, 1.0f, 0.0f),
	mYaw(glm::pi<float>()),
	mPitch(0.0f),
	mFOV(DEF_FOV)
{
}

glm::mat4 Camera::getViewMatrix() const {

	return glm::lookAt(mPosition, mTargetPos, mUp);
}

const glm::vec3& Camera::getLook() const{
	
	return mLook;
}

const glm::vec3& Camera::getRight() const{
	
	return mRight;
}

const glm::vec3& Camera::getUp() const{
	
	return mUp;
}

const glm::vec3& Camera::getPosition() const {

	return mPosition;
}
// FPS Camera

// Constructor
FPSCamera::FPSCamera(glm::vec3 position, float yaw, float pitch)
{
	mPosition = position;
	mYaw = glm::radians(yaw);
	mPitch = glm::radians(pitch);
}

FPSCamera::FPSCamera(glm::vec3 position, glm::vec3 target)
{
	mPosition = position;
	mTargetPos = target;

	// Calculate the vector that looks at the target from the camera position
	glm::vec3 lookDir = position - target;

	// Now Calculate the pitch and yaw from the target look vector.  (radians)
	mPitch = -atan2(lookDir.y, sqrt(lookDir.x * lookDir.x + lookDir.z * lookDir.z));
	mYaw = atan2(lookDir.x, lookDir.z) + glm::pi<float>();
}

// Set the camera position
void FPSCamera::setPosition(const glm::vec3& position)
{
	mPosition = position;
}


// Move the camera position
void FPSCamera::move(const glm::vec3& offsetPos)
{
	
	mPosition += offsetPos;
	
	updateCameraVectors();
}


// Rotate camera using Yaw and Pitch angles passed in degrees
void FPSCamera::rotate(float yaw, float pitch){
	
	mYaw += glm::radians(yaw);
	mPitch += glm::radians(pitch);


	// Constrain the pitch
	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

	// Constraint the yaw [0, 2*pi]
	if (mYaw > glm::two_pi<float>())
		mYaw -= glm::two_pi<float>();
	else if (mYaw < 0.0)
		mYaw += glm::two_pi<float>();

	updateCameraVectors();
}

// Calculate the vectors from the cameras Euler Angles
void FPSCamera::updateCameraVectors() {

	glm::vec3 look;
	look.x = cosf(mPitch) * sinf(mYaw);
	look.y = sinf(mPitch);
	look.z = cosf(mPitch) * cosf(mYaw);

	mLook = glm::normalize(look);

	mRight = glm::normalize(glm::cross(mLook, WORLD_UP));
	mUp = glm::normalize(glm::cross(mRight, mLook));

	mTargetPos = mPosition + mLook;

}
	

// Orbit Camera

OrbitCamera::OrbitCamera()
	:mRadius(10.0f)
{}

void OrbitCamera::setLookAt(const glm::vec3& target) {
	mTargetPos = target;
}


void OrbitCamera::setRadius(float radius) {
	mRadius = glm::clamp(radius, 2.0f, 80.0f);
}

void OrbitCamera::rotate(float yaw, float pitch) {
	mYaw = glm::radians(yaw);
	mPitch = glm::radians(pitch);

	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f -0.1f);

	updateCameraVectors();
}

void OrbitCamera::updateCameraVectors() {

	mPosition.x = mTargetPos.x + mRadius * cosf(mPitch) * sinf(mYaw);
	mPosition.y = mTargetPos.y + mRadius * sinf(mPitch);
	mPosition.z = mTargetPos.z + mRadius * cosf(mPitch) * cosf(mYaw);

}