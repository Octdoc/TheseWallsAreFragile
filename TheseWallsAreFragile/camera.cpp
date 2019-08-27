#include "pch.h"
#include "camera.h"

namespace twaf
{
	Camera::Camera() :
		m_viewMatrix(),
		m_projectionMatrix() {}
	void Camera::SetOrthographic(float width, float height)
	{
		m_projectionMatrix = float4x4::Orthographic(width, height, 0.1f, 1000.0f);
	}
	void Camera::SetProjective(float fov, float screenAspect)
	{
		m_projectionMatrix = float4x4::PerspectiveFOV(fov, screenAspect, 0.1f, 1000.0f);
	}
	void Camera::Update()
	{
		m_viewMatrix = float4x4::RotationCamera(position, rotation);
	}
	float4x4 Camera::getViewMatrix()
	{
		return m_viewMatrix;
	}
	float4x4 Camera::getProjectionMatrix()
	{
		return m_projectionMatrix;
	}
	float4x4 Camera::GetCameraMatrix()
	{
		return m_projectionMatrix * m_viewMatrix;
	}
}