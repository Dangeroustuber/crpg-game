#pragma once

#include "pch.h"

#include <SimpleMath.h>

struct Camera {
	bool initialize(float inWidth, float inHeight, float inNearPlane, float inFarPlane);
	void lookAt(const DirectX::SimpleMath::Vector3& inPosition, const DirectX::SimpleMath::Vector3& target, const DirectX::SimpleMath::Vector3& up);

	DirectX::SimpleMath::Matrix getViewProjectionMatrix() const;

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projectionMatrix;

	float width = 0.0f;
	float height = 0.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
};