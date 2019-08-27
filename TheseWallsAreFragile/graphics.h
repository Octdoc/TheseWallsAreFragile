#pragma once

#include "math/position.hpp"

namespace twaf
{
	class Model
	{
	public:
		virtual ~Model() = default;
		virtual void Render() = 0;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual void SetTexture() = 0;
	};

	class PixelShader
	{
	public:
		virtual ~PixelShader() = default;
		virtual void SetShader() = 0;
	};

	class Graphics
	{
	public:
		Graphics() = default;
		virtual ~Graphics() = default;

		virtual void UseEffect(const wchar_t* pixelShaderFileName, unsigned width, unsigned height) = 0;
		virtual void DontUseEffect() = 0;

		virtual void ClearScreen() = 0;
		virtual void ClearScreen(float r, float g, float b, float a = 1.0f) = 0;
		virtual void Present() = 0;

		virtual void SetDefaultVertexShader() = 0;
		virtual void RenderTexture(Texture* texture, float2 position, float2 size) = 0;
		virtual void RenderDialog(Texture* text) = 0;
		virtual void RenderDialog(Texture* text, Texture* icon, float2 position, float2 size) = 0;

		virtual void UpdateCamera() = 0;
		virtual void SetCameraOrthographic(float width, float height) = 0;
		virtual void SetCameraProjective(float fov, float screenAspect) = 0;
		virtual Positionf& getCamera() = 0;

		virtual void WriteEntityData(Positionf& position) = 0;
		virtual void WriteEffectBuffer(float buffer[8]) = 0;
		virtual void WriteLightData(float4 color, float3 position, float ambient) = 0;

		virtual Model* LoadModel(const wchar_t* filename) = 0;
		virtual Texture* LoadTexture(const wchar_t* filename) = 0;
		virtual Texture* CreateTexture(void *pixels, unsigned width, unsigned height) = 0;
		virtual Texture* CreateTextTexture(const wchar_t* text) = 0;
		virtual PixelShader* LoadPixelShader(const wchar_t* filename) = 0;
	};
}