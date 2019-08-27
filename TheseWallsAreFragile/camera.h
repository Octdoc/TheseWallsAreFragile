#pragma once

#include "math/position.hpp"

namespace twaf
{
	class Camera :public Positionf
	{
		float4x4 m_viewMatrix;
		float4x4 m_projectionMatrix;

	public:
		Camera();

		void SetOrthographic(float width, float height);
		void SetProjective(float fov, float screenAspect);

		void Update();
		float4x4 getViewMatrix();
		float4x4 getProjectionMatrix();
		float4x4 GetCameraMatrix();
	};
}