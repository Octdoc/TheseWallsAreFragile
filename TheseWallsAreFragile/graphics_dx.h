#pragma once

#include "pch.h"
#include "graphics.h"
#include "camera.h"
#include "image.h"

namespace twaf
{
	class Graphics_DX;
	class Model_DX;
	class Texture_DX;
	class RenderTargetTexture_DX;
	class SamplerState;
	class PixelShader_DX;
	class VertexShader_DX;
	class CBuffer_DX;

	class Graphics_DX : public Graphics
	{
		HWND m_hwnd;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_context;
		IDXGISwapChain* m_swapChain;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11Texture2D* m_depthBuffer;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11RasterizerState* m_rasterizerState;
		ID3D11BlendState* m_blendState;
		D3D11_VIEWPORT m_viewport;
		D3D_FEATURE_LEVEL m_featureLevel;

		VertexShader_DX* m_vertexShader;
		VertexShader_DX* m_textureRenderVertexShader;
		PixelShader_DX* m_textureRenderPixelShader;
		Model_DX* m_textureRenderModel;
		SamplerState* m_samplerState;
		CBuffer_DX* m_vsBuffer;
		CBuffer_DX* m_psBuffer;
		CBuffer_DX* m_textureRenderCBuffer;

		PixelShader_DX* m_effectShader;
		RenderTargetTexture_DX* m_effectScreen;
		float m_effectBuffer[8];

		Texture* m_dialogTexture;
		Texture* m_talkerIcon;
		float2 m_talkerIconPosition;
		float2 m_talkerIconsize;

		Image m_font;
		Image m_dialogFrame;

		Camera m_camera;

	private:
		void SetScreenAsRenderTarget();

	public:
		Graphics_DX();
		virtual ~Graphics_DX() override;

		void Init(HWND hwnd);
		void Shutdown();

		inline ID3D11Device* Device() { return m_device; }
		inline ID3D11DeviceContext* Context() { return m_context; }

		virtual void UseEffect(const wchar_t* pixelShaderFileName, unsigned width, unsigned height) override;
		virtual void DontUseEffect() override;

		virtual void ClearScreen() override;
		virtual void ClearScreen(float r, float g, float b, float a = 1.0f) override;
		virtual void Present() override;

		virtual void SetDefaultVertexShader() override;
		virtual void RenderTexture(Texture* texture, float2 position, float2 size) override;
		void RenderTexture(Texture* texture, float2 position, float2 size, PixelShader_DX* pixelShader);
		virtual void RenderDialog(Texture* text) override;
		virtual void RenderDialog(Texture* text, Texture* icon, float2 position, float2 size) override;

		virtual void UpdateCamera() override;
		virtual void SetCameraOrthographic(float width, float height) override;
		virtual void SetCameraProjective(float fov, float screenAspect) override;
		virtual Positionf& getCamera() override;

		virtual void WriteEntityData(Positionf& position) override;
		virtual void WriteEffectBuffer(float buffer[8]) override;
		virtual void WriteLightData(float4 color, float3 position, float ambient) override;

		virtual Model* LoadModel(const wchar_t* filename) override;
		virtual Texture* LoadTexture(const wchar_t* filename) override;
		virtual Texture* CreateTexture(void* pixels, unsigned width, unsigned height) override;
		virtual Texture* CreateTextTexture(const wchar_t* text) override;
		virtual PixelShader* LoadPixelShader(const wchar_t* filename) override;

		const char* VS_Target();
		const char* PS_Target();
	};

	class Model_DX :public Model
	{
		Graphics_DX& m_graphics;
		ID3D11Buffer* m_vertexBuffer;
		ID3D11Buffer* m_indexBuffer;
		unsigned m_indexCount;
		unsigned m_vertexSize;

	public:
		Model_DX(Graphics_DX& graphics);
		Model_DX(Graphics_DX& graphics, const wchar_t* filename);
		virtual ~Model_DX() override;
		virtual void Render() override;
	};

	class Texture_DX :public Texture
	{
	protected:
		Graphics_DX& m_graphics;
		ID3D11ShaderResourceView* m_shaderResourceView;
		unsigned m_width;
		unsigned m_height;

	protected:
		Texture_DX(Graphics_DX& graphics);
		void CreateFromImage(void* pixels, unsigned width, unsigned height);

	public:
		Texture_DX(Graphics_DX& graphics, Image& image);
		Texture_DX(Graphics_DX& graphics, const wchar_t* filename);
		Texture_DX(Graphics_DX& graphics, void* pixels, unsigned width, unsigned height);
		virtual ~Texture_DX() override;
		virtual void SetTexture() override;
	};

	class RenderTargetTexture_DX :public Texture_DX
	{
		ID3D11Texture2D* m_renderTargetTexture;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11Texture2D* m_depthBuffer;
		D3D11_VIEWPORT m_viewPort;

	public:
		RenderTargetTexture_DX(Graphics_DX& graphics, unsigned width, unsigned height);
		virtual ~RenderTargetTexture_DX() override;

		void SetAsRenderTarget();
		void ClearRenderTarget();
		void ClearRenderTarget(float r, float g, float b, float a = 1.0f);
	};

	class SamplerState
	{
		Graphics_DX& m_graphics;
		ID3D11SamplerState* m_samplerState;

	public:
		SamplerState(Graphics_DX& graphics);
		~SamplerState();
		void SetSamplerState();
	};

	class PixelShader_DX :public PixelShader
	{
		Graphics_DX& m_graphics;
		ID3D11PixelShader* m_pixelShader;

	public:
		PixelShader_DX(Graphics_DX& graphics, const wchar_t* filename);
		virtual ~PixelShader_DX() override;
		virtual void SetShader() override;
	};

	class VertexShader_DX
	{
		Graphics_DX& m_graphics;
		ID3D11VertexShader* m_vertexShader;
		ID3D11InputLayout* m_inputLayout;

	public:
		VertexShader_DX(Graphics_DX& graphics, bool effect);
		~VertexShader_DX();
		void SetShader();
	};

	class CBuffer_DX
	{
		Graphics_DX& m_graphics;
		ID3D11Buffer* m_buffer;
		unsigned m_size;

	public:
		CBuffer_DX(Graphics_DX& graphics, unsigned size);
		~CBuffer_DX();
		void SetToVertexShader();
		void SetToPixelShader();
		void Write(void* data);
	};
}