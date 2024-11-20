#include "Camera.hpp"

using namespace DirectX::SimpleMath;

bool Camera::initialize(float inWidth, float inHeight, float inNearPlane, float inFarPlane) {
	width = inWidth;
	height = inHeight;
	nearPlane = inNearPlane;
	farPlane = inFarPlane;

	lookAt({ 10.0f, 10.0f, -10.0f }, Vector3::Zero, Vector3::UnitY);

	return true;
}

DirectX::SimpleMath::Matrix Camera::getViewProjectionMatrix() const {
	return viewMatrix * projectionMatrix;
}

void Camera::lookAt(const DirectX::SimpleMath::Vector3& inPosition, const DirectX::SimpleMath::Vector3& target, const DirectX::SimpleMath::Vector3& up) {
	this->position = inPosition;
	viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(position, target, up);

	float viewWidth = 10.0f;
	float viewHeight = viewWidth * (height / width);
	projectionMatrix = DirectX::SimpleMath::Matrix::CreateOrthographic(viewWidth, viewHeight, nearPlane, farPlane);
}