#include "pch.h"
#include "graphics_dx.h"

namespace twaf
{
#pragma region Graphics

	void Graphics_DX::SetScreenAsRenderTarget()
	{
		m_context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		m_context->RSSetViewports(1, &m_viewport);
	}
	Graphics_DX::Graphics_DX() :
		m_hwnd(NULL),
		m_device(nullptr),
		m_context(nullptr),
		m_swapChain(nullptr),
		m_renderTargetView(nullptr),
		m_depthBuffer(nullptr),
		m_depthStencilView(nullptr),
		m_depthStencilState(nullptr),
		m_rasterizerState(nullptr),
		m_blendState(nullptr),
		m_featureLevel(D3D_FEATURE_LEVEL_10_0),
		m_vertexShader(nullptr),
		m_textureRenderVertexShader(nullptr),
		m_textureRenderPixelShader(nullptr),
		m_textureRenderModel(nullptr),
		m_samplerState(nullptr),
		m_vsBuffer(nullptr),
		m_psBuffer(nullptr),
		m_textureRenderCBuffer(nullptr),
		m_effectShader(nullptr),
		m_effectScreen(nullptr),
		m_dialogTexture(nullptr) {}
	Graphics_DX::~Graphics_DX()
	{
		Shutdown();
	}
	HWND Graphics_DX::getHWND()
	{
		return m_hwnd;
	}
	void Graphics_DX::Init(HWND hwnd)
	{
		if (!m_font.Load(L"Media/font16.png"))
			throw std::exception("Could not load font.");
		if (!m_dialogFrame.Load(L"Media/dialog.png"))
			throw std::exception("Could not load image.");

		m_hwnd = hwnd;
		HRESULT hr;
		RECT rect;
		GetClientRect(hwnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;
		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr,
			0, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, &m_featureLevel, &m_context);
		if (FAILED(hr))
			throw std::exception("DirectX device and swap chain could not be created");

		ID3D11Texture2D* backBufferPtr = nullptr;
		hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& backBufferPtr);
		if (FAILED(hr))
		{
			SAFE_RELEASE(backBufferPtr);
			throw std::exception("DirectX error: Could not get swapchain buffer");
		}
		hr = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
		SAFE_RELEASE(backBufferPtr);
		if (FAILED(hr))
			throw std::exception("DirectX error: Could not create render target");

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (FAILED(hr))
			throw std::exception("Could not create depth stencil state");

		D3D11_TEXTURE2D_DESC depthBufferDesc{};
		depthBufferDesc.Width = width;
		depthBufferDesc.Height = height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		hr = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthBuffer);
		if (FAILED(hr))
			throw std::exception("Could not create depth buffer");

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = m_device->CreateDepthStencilView(m_depthBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(hr))
			throw std::exception("Could not create depth stencil view");

		D3D11_RASTERIZER_DESC rasterizerDesc{};
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
		if (FAILED(hr))
			throw std::exception("Failed to create rasterizer state");

		D3D11_BLEND_DESC blendDesc{};
		/*blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;*/
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		hr = m_device->CreateBlendState(&blendDesc, &m_blendState);
		if (FAILED(hr))
			throw std::exception("Failed to create blend state");

		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = (float)width;
		m_viewport.Height = (float)height;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		m_context->OMSetDepthStencilState(m_depthStencilState, 0);
		m_context->RSSetViewports(1, &m_viewport);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_context->RSSetState(m_rasterizerState);
		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_context->OMSetBlendState(m_blendState, blendFactor, 0xffffffff);

		m_vertexShader = new VertexShader_DX(*this, false);
		m_samplerState = new SamplerState(*this);
		m_vsBuffer = new CBuffer_DX(*this, 2 * sizeof(float4x4));
		m_psBuffer = new CBuffer_DX(*this, 8 * sizeof(float));
		m_textureRenderCBuffer = new CBuffer_DX(*this, 2 * sizeof(float2));
		m_textureRenderModel = new Model_DX(*this);
		m_textureRenderVertexShader = new VertexShader_DX(*this, true);
		m_textureRenderPixelShader = new PixelShader_DX(*this, L"Media/no.effect");
		m_samplerState->SetSamplerState();
		m_vsBuffer->SetToVertexShader();
		m_psBuffer->SetToPixelShader();
	}
	void Graphics_DX::Shutdown()
	{
		m_hwnd = NULL;
		SAFE_RELEASE(m_device);
		SAFE_RELEASE(m_context);
		SAFE_RELEASE(m_swapChain);
		SAFE_RELEASE(m_renderTargetView);
		SAFE_RELEASE(m_depthBuffer);
		SAFE_RELEASE(m_depthStencilView);
		SAFE_RELEASE(m_depthStencilState);
		SAFE_RELEASE(m_rasterizerState);
		SAFE_RELEASE(m_blendState);

		SAFE_DELETE(m_vertexShader);
		SAFE_DELETE(m_textureRenderVertexShader);
		SAFE_DELETE(m_textureRenderPixelShader);
		SAFE_DELETE(m_textureRenderModel);
		SAFE_DELETE(m_samplerState);
		SAFE_DELETE(m_vsBuffer);
		SAFE_DELETE(m_psBuffer);
		SAFE_DELETE(m_textureRenderCBuffer);
		SAFE_DELETE(m_effectShader);
		SAFE_DELETE(m_effectScreen);
		m_dialogTexture = nullptr;
	}
	void Graphics_DX::UseEffect(const wchar_t* pixelShaderFileName, unsigned width, unsigned height)
	{
		PixelShader_DX* effectShader = nullptr;
		RenderTargetTexture_DX* effectScreen = nullptr;
		try
		{
			effectShader = new PixelShader_DX(*this, pixelShaderFileName);
			effectScreen = new RenderTargetTexture_DX(*this, width, height);
			SAFE_DELETE(m_effectShader);
			SAFE_DELETE(m_effectScreen);
			m_effectShader = effectShader;
			m_effectScreen = effectScreen;
		}
		catch (...)
		{
			SAFE_DELETE(effectShader);
			SAFE_DELETE(effectScreen);
		}
	}
	void Graphics_DX::DontUseEffect()
	{
		SAFE_DELETE(m_effectShader);
		SAFE_DELETE(m_effectScreen);
	}
	void Graphics_DX::ClearScreen()
	{
		ClearScreen(0.0f, 0.0f, 0.0f);
	}
	void Graphics_DX::ClearScreen(float r, float g, float b, float a)
	{
		if (m_effectScreen)
		{
			m_effectScreen->SetAsRenderTarget();
			m_effectScreen->ClearRenderTarget(r, g, b, a);
		}
		float color[] = { r, g, b, a };
		m_context->ClearRenderTargetView(m_renderTargetView, color);
		m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	void Graphics_DX::Present()
	{
		if (m_effectScreen)
		{
			SetScreenAsRenderTarget();
			m_psBuffer->Write(m_effectBuffer);
			RenderTexture(m_effectScreen, float2(0.0f, 0.0f), float2(900.0f, 500.0f), m_effectShader);
		}
		if (m_dialogTexture)
		{
			float effectBuffer[8] = { 1.0f };
			m_psBuffer->Write(effectBuffer);
			RenderTexture(m_dialogTexture, float2(66.0f, 340.0f), float2(768.0f, 144.0f));
			if (m_talkerIcon)
				RenderTexture(m_talkerIcon, m_talkerIconPosition, m_talkerIconsize);
			m_dialogTexture = nullptr;
		}
		m_swapChain->Present(1, 0);
	}
	void Graphics_DX::SetDefaultVertexShader()
	{
		m_vertexShader->SetShader();
		m_vsBuffer->SetToVertexShader();
	}
	void Graphics_DX::RenderTexture(Texture* texture, float2 position, float2 size)
	{
		RenderTexture(texture, position, size, m_textureRenderPixelShader);
	}
	void Graphics_DX::RenderTexture(Texture* texture, float2 position, float2 size, PixelShader_DX* pixelShader)
	{
		float buffer[] = {
			position.x / 450.0f - 1.0f,
			1.0f - (position.y + size.y) / 250.0f,
			size.x / 450.0f,
			size.y / 250.0f
		};
		m_textureRenderVertexShader->SetShader();
		pixelShader->SetShader();
		m_textureRenderCBuffer->SetToVertexShader();
		m_textureRenderCBuffer->Write(buffer);
		texture->SetTexture();
		m_textureRenderModel->Render();
	}
	void Graphics_DX::RenderDialog(Texture* text)
	{
		m_dialogTexture = text;
		m_talkerIcon = nullptr;
	}
	void Graphics_DX::RenderDialog(Texture* text, Texture* icon, float2 position, float2 size)
	{
		m_dialogTexture = text;
		m_talkerIcon = icon;
		m_talkerIconPosition = position;
		m_talkerIconsize = size;
	}
	void Graphics_DX::UpdateCamera()
	{
		m_camera.Update();
	}
	void Graphics_DX::SetCameraOrthographic(float width, float height)
	{
		m_camera.SetOrthographic(width, height);
	}
	void Graphics_DX::SetCameraProjective(float fov, float screenAspect)
	{
		m_camera.SetProjective(fov, screenAspect);
	}
	Positionf& Graphics_DX::getCamera()
	{
		return m_camera;
	}
	void Graphics_DX::WriteEntityData(Positionf& position)
	{
		float4x4 matrices[] = {
			position.GetWorldMatrix(),
			m_camera.GetCameraMatrix()
		};
		m_vsBuffer->Write(matrices);
	}
	void Graphics_DX::WriteEffectBuffer(float buffer[8])
	{
		memcpy(m_effectBuffer, buffer, sizeof(m_effectBuffer));
	}
	void Graphics_DX::WriteLightData(float4 color, float3 position, float ambient)
	{
		float buffer[] = {
			color.x, color.y, color.z, color.w,
			position.x, position.y, position.z,
			ambient
		};
		m_psBuffer->Write(buffer);
	}
	Model* Graphics_DX::LoadModel(const wchar_t* filename)
	{
		return new Model_DX(*this, filename);
	}
	Texture* Graphics_DX::LoadTexture(const wchar_t* filename)
	{
		return new Texture_DX(*this, filename);
	}
	Texture* Graphics_DX::CreateTexture(void* pixels, unsigned width, unsigned height)
	{
		return new Texture_DX(*this, pixels, width, height);
	}
	Texture* Graphics_DX::CreateTextTexture(const wchar_t* text)
	{
		class CreateTextClass
		{
			Image& m_font;
			Image m_texture;
			std::wstring m_text;
			unsigned m_x;
			unsigned m_y;

		private:
			inline unsigned CharacterX(wchar_t ch) { return (ch % 16) * CharacterW(); }
			inline unsigned CharacterY(wchar_t ch) { return (ch / 16) * CharacterH(); }
			inline unsigned CharacterW() { return 8; }
			inline unsigned CharacterH() { return 16; }
			inline void NewLine() { m_x = 6; m_y += CharacterH(); }
			void DrawCharacter(wchar_t ch, unsigned x, unsigned y)
			{
				if (ch > 127) ch = L'?';
				unsigned cx = CharacterX(ch);
				unsigned cy = CharacterY(ch);
				for (unsigned fx = 0; fx < CharacterW(); fx++)
				{
					for (unsigned fy = 0; fy < CharacterH(); fy++)
					{
						Color c = m_font(cx + fx, cy + fy);
						if (c.alpha)
							m_texture(x + fx, y + fy) = c;
					}
				}
			}
			void Create()
			{
				for (wchar_t ch : m_text)
				{
					if (ch == L'\n')
						NewLine();
					else
					{
						DrawCharacter(ch, m_x, m_y);
						m_x += CharacterW() - 2;
					}
					if (m_x + CharacterW() >= m_texture.Width())
						NewLine();
					if (m_y + CharacterH() >= m_texture.Height())
						break;
				}
			}

		public:
			CreateTextClass(Image& font, Image& dialogFrame, const wchar_t* text) :
				m_font(font),
				m_texture(dialogFrame),
				m_text(text),
				m_x(6),
				m_y(6)
			{
				Create();
			}
			Image& getImage() { return m_texture; }
		};
		CreateTextClass image(m_font, m_dialogFrame, text);
		return new Texture_DX(*this, image.getImage());
	}
	PixelShader* Graphics_DX::LoadPixelShader(const wchar_t* filename)
	{
		return new PixelShader_DX(*this, filename);
	}
	const char* Graphics_DX::VS_Target()
	{
		switch (m_featureLevel)
		{
		case D3D_FEATURE_LEVEL_10_0:
			return "vs_4_0";
		case D3D_FEATURE_LEVEL_10_1:
			return "vs_4_1";
		case D3D_FEATURE_LEVEL_11_0:
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_12_0:
		case D3D_FEATURE_LEVEL_12_1:
			return "vs_5_0";
		}
		return nullptr;
	}
	const char* Graphics_DX::PS_Target()
	{
		switch (m_featureLevel)
		{
		case D3D_FEATURE_LEVEL_10_0:
			return "ps_4_0";
		case D3D_FEATURE_LEVEL_10_1:
			return "ps_4_1";
		case D3D_FEATURE_LEVEL_11_0:
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_12_0:
		case D3D_FEATURE_LEVEL_12_1:
			return "ps_5_0";
		}
		return nullptr;
	}

#pragma endregion

#pragma region Model

	Model_DX::Model_DX(Graphics_DX& graphics) :
		m_graphics(graphics),
		m_vertexBuffer(nullptr),
		m_indexBuffer(nullptr),
		m_indexCount(0)
	{
		m_indexCount = 6;
		m_vertexSize = sizeof(float) * 5;
		float vertices[] = {
		1.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,		0.0f, 1.0f
		};
		unsigned indices[] = { 0, 2, 1, 0, 3, 2 };

		HRESULT hr;
		D3D11_BUFFER_DESC bufferDesc{};
		D3D11_SUBRESOURCE_DATA subResourceData{};
		ID3D11Device* device = graphics.Device();

		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(vertices);
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		subResourceData.pSysMem = vertices;
		hr = device->CreateBuffer(&bufferDesc, &subResourceData, &m_vertexBuffer);
		if (FAILED(hr))
			throw std::exception("Failed to create vertex buffer");

		bufferDesc.ByteWidth = sizeof(indices);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subResourceData.pSysMem = indices;
		hr = device->CreateBuffer(&bufferDesc, &subResourceData, &m_indexBuffer);
		if (FAILED(hr))
			throw std::exception("Failed to create index buffer");
	}

	Model_DX::Model_DX(Graphics_DX& graphics, const wchar_t* filename) :
		m_graphics(graphics),
		m_vertexBuffer(nullptr),
		m_indexBuffer(nullptr),
		m_indexCount(0)
	{
		struct Vertex
		{
			float3 position;
			float2 tex;
			float3 normal;
		};
		unsigned vertexCount = 0;
		m_vertexSize = sizeof(Vertex);
		std::ifstream infile;
		infile.open(filename, std::ios::in | std::ios::binary);
		if (!infile.is_open())
			throw std::exception("Could not open model file");
		infile.read((char*)& vertexCount, sizeof(vertexCount));
		infile.read((char*)& m_indexCount, sizeof(m_indexCount));
		std::vector<Vertex> vertices(vertexCount);
		std::vector<unsigned> indices(m_indexCount);
		infile.read((char*)vertices.data(), (size_t)vertexCount * sizeof(Vertex));
		infile.read((char*)indices.data(), (size_t)m_indexCount * sizeof(unsigned));
		infile.close();

		HRESULT hr;
		D3D11_BUFFER_DESC bufferDesc{};
		D3D11_SUBRESOURCE_DATA subResourceData{};
		ID3D11Device* device = graphics.Device();

		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = vertexCount * sizeof(Vertex);
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		subResourceData.pSysMem = vertices.data();
		hr = device->CreateBuffer(&bufferDesc, &subResourceData, &m_vertexBuffer);
		if (FAILED(hr))
			throw std::exception("Failed to create vertex buffer");

		bufferDesc.ByteWidth = m_indexCount * sizeof(unsigned);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subResourceData.pSysMem = indices.data();
		hr = device->CreateBuffer(&bufferDesc, &subResourceData, &m_indexBuffer);
		if (FAILED(hr))
			throw std::exception("Failed to create index buffer");
	}
	Model_DX::~Model_DX()
	{
		SAFE_RELEASE(m_vertexBuffer);
		SAFE_RELEASE(m_indexBuffer);
	}
	void Model_DX::Render()
	{
		UINT stride = m_vertexSize;
		UINT offset = 0;
		ID3D11DeviceContext* context = m_graphics.Context();

		context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		context->DrawIndexed(m_indexCount, 0, 0);
	}

#pragma endregion

#pragma region Texture

	Texture_DX::Texture_DX(Graphics_DX& graphics) :
		m_graphics(graphics),
		m_shaderResourceView(nullptr),
		m_width(0),
		m_height(0) {}
	void Texture_DX::CreateFromImage(void* pixels, unsigned width, unsigned height)
	{
		m_width = width;
		m_height = height;

		ID3D11Device* device = m_graphics.Device();
		ID3D11DeviceContext* context = m_graphics.Context();
		HRESULT hr;
		ID3D11Texture2D* texture;

		D3D11_TEXTURE2D_DESC t2dd{};
		t2dd.Width = m_width;
		t2dd.Height = m_height;
		t2dd.MipLevels = 1;
		t2dd.ArraySize = 1;
		t2dd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		t2dd.SampleDesc.Count = 1;
		t2dd.SampleDesc.Quality = 0;
		t2dd.CPUAccessFlags = 0;
		t2dd.Usage = D3D11_USAGE_IMMUTABLE;
		t2dd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		t2dd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA imgData{};
		imgData.pSysMem = pixels;
		imgData.SysMemPitch = m_width * 4;
		imgData.SysMemSlicePitch = 0;
		hr = device->CreateTexture2D(&t2dd, &imgData, &texture);
		if (FAILED(hr))
			throw std::exception("Failed to create texture");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
		srvd.Format = t2dd.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(texture, &srvd, &m_shaderResourceView);
		if (FAILED(hr))
			throw std::exception("Failed to create shader resource view");
	}
	Texture_DX::Texture_DX(Graphics_DX& graphics, Image& image) :
		Texture_DX(graphics, image.Pixels(), image.Width(), image.Height()) {}
	Texture_DX::Texture_DX(Graphics_DX& graphics, const wchar_t* filename) :
		m_graphics(graphics),
		m_shaderResourceView(nullptr),
		m_width(0),
		m_height(0)
	{
		Image image;
		if (!image.Load(filename))
			throw std::exception("Could not load image");
		CreateFromImage(image.Pixels(), image.Width(), image.Height());
	}
	Texture_DX::Texture_DX(Graphics_DX& graphics, void* pixels, unsigned width, unsigned height) :
		m_graphics(graphics),
		m_shaderResourceView(nullptr),
		m_width(0),
		m_height(0)
	{
		CreateFromImage(pixels, width, height);
	}
	Texture_DX::~Texture_DX()
	{
		SAFE_RELEASE(m_shaderResourceView);
	}
	void Texture_DX::SetTexture()
	{
		m_graphics.Context()->PSSetShaderResources(0, 1, &m_shaderResourceView);
	}

#pragma endregion

#pragma region RenderTargetTexture

	RenderTargetTexture_DX::RenderTargetTexture_DX(Graphics_DX& graphics, unsigned width, unsigned height) :
		Texture_DX(graphics)
	{
		ID3D11Device* device = graphics.Device();
		HRESULT hr;

		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&textureDesc, nullptr, &m_renderTargetTexture);
		if (FAILED(hr))
			throw std::exception("Failed to create texture");

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		hr = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
		if (FAILED(hr))
			throw std::exception("Failed to create shader resource view");

		D3D11_TEXTURE2D_DESC depthBufferDesc;
		depthBufferDesc.Width = width;
		depthBufferDesc.Height = height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthBuffer);
		if (FAILED(hr))
			throw std::exception("Failed to create back buffer");

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthBufferDesc.Format;
		depthStencilViewDesc.Flags = 0;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(m_depthBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(hr))
			throw std::exception("Failed to create depth stencil view");

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		hr = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
		if (FAILED(hr))
			throw std::exception("Failed to create render target view");

		m_viewPort.TopLeftX = 0.0f;
		m_viewPort.TopLeftY = 0.0f;
		m_viewPort.Width = static_cast<float>(width);
		m_viewPort.Height = static_cast<float>(height);
		m_viewPort.MinDepth = 0.0f;
		m_viewPort.MaxDepth = 1.0f;

		m_width = width;
		m_height = height;
	}
	RenderTargetTexture_DX::~RenderTargetTexture_DX()
	{
		SAFE_RELEASE(m_renderTargetTexture);
		SAFE_RELEASE(m_renderTargetView);
		SAFE_RELEASE(m_depthStencilView);
		SAFE_RELEASE(m_depthBuffer);
	}
	void RenderTargetTexture_DX::SetAsRenderTarget()
	{
		ID3D11DeviceContext* context = m_graphics.Context();
		context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		context->RSSetViewports(1, &m_viewPort);
	}
	void RenderTargetTexture_DX::ClearRenderTarget()
	{
		ClearRenderTarget(0.0f, 0.0f, 0.0f);
	}
	void RenderTargetTexture_DX::ClearRenderTarget(float r, float g, float b, float a)
	{
		float color[] = { r,g,b,a };
		ID3D11DeviceContext* context = m_graphics.Context();
		context->ClearRenderTargetView(m_renderTargetView, color);
		context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

#pragma endregion

#pragma region SamplerState

	SamplerState::SamplerState(Graphics_DX& graphics) :
		m_graphics(graphics),
		m_samplerState(nullptr)
	{
		HRESULT hr;
		D3D11_SAMPLER_DESC sd;
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias = 0.0f;
		sd.MaxAnisotropy = 1;
		sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sd.BorderColor[0] = 0.0f;
		sd.BorderColor[1] = 0.0f;
		sd.BorderColor[2] = 0.0f;
		sd.BorderColor[3] = 0.0f;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;

		hr = graphics.Device()->CreateSamplerState(&sd, &m_samplerState);
		if (FAILED(hr))
			throw std::exception("Failed to create sampler state");
	}
	SamplerState::~SamplerState()
	{
		SAFE_RELEASE(m_samplerState);
	}
	void SamplerState::SetSamplerState()
	{
		m_graphics.Context()->PSSetSamplers(0, 1, &m_samplerState);
	}

#pragma endregion

#pragma region PixelShader

	PixelShader_DX::PixelShader_DX(Graphics_DX& graphics, const wchar_t* filename) :
		m_graphics(graphics),
		m_pixelShader(nullptr)
	{
		ID3DBlob* shaderCode = nullptr;
		ID3DBlob* errorMessage = nullptr;
		HRESULT hr = D3DCompileFromFile(filename, nullptr, nullptr, "main", graphics.PS_Target(), 0, 0, &shaderCode, &errorMessage);
		if (FAILED(hr))
		{
			if (errorMessage)
			{
				std::ofstream errorFile("psError.txt");
				errorFile.write((char*)errorMessage->GetBufferPointer(), errorMessage->GetBufferSize());
				errorFile.close();
			}
			SAFE_RELEASE(errorMessage);
			SAFE_RELEASE(shaderCode);
			throw std::exception("Could not compile pixel shader from file");
		}
		SAFE_RELEASE(errorMessage);
		hr = graphics.Device()->CreatePixelShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), nullptr, &m_pixelShader);
		SAFE_RELEASE(shaderCode);
		if (FAILED(hr))
			throw std::exception("Could not create pixel shader");
	}
	PixelShader_DX::~PixelShader_DX()
	{
		SAFE_RELEASE(m_pixelShader);
	}
	void PixelShader_DX::SetShader()
	{
		m_graphics.Context()->PSSetShader(m_pixelShader, nullptr, 0);
	}

#pragma endregion

#pragma region VertexShader

	VertexShader_DX::VertexShader_DX(Graphics_DX& graphics, bool effect) :
		m_graphics(graphics),
		m_vertexShader(nullptr),
		m_inputLayout(nullptr)
	{
		std::string shader = effect ?
			"\
cbuffer PositionBuffer\n\
{\n\
	float2 position;\n\
	float2 size;\n\
}\n\
struct VertexInputType\n\
{\n\
	float3 position : POSITION;\n\
	float2 tex : TEXCOORD;\n\
};\n\
struct PixelInputType\n\
{\n\
	float4 position : SV_POSITION;\n\
	float2 tex : TEXCOORD;\n\
};\n\
PixelInputType main(VertexInputType input)\n\
{\n\
	PixelInputType output;\n\
	float2 pos = input.position.xy * size + position;\n\
	output.position = float4(pos, 0.1f, 1.0f);\n\
	output.tex = input.tex;\n\
	return output;\n\
}":
		"\
cbuffer MatrixBuffer\n\
{\n\
	matrix worldMatrix;\n\
	matrix cameraMatrix;\n\
};\n\
struct VertexInputType\n\
{\n\
	float3 position : POSITION;\n\
	float2 tex : TEXCOORD;\n\
	float3 normal : NORMAL;\n\
};\n\
struct PixelInputType\n\
{\n\
	float4 position : SV_POSITION;\n\
	float3 pos : POSITION;\n\
	float2 tex : TEXCOORD;\n\
	float3 normal : NORMAL;\n\
};\n\
PixelInputType main(VertexInputType input)\n\
{\n\
	PixelInputType output;\n\
	output.position = mul(float4(input.position, 1.0f), worldMatrix);\n\
	output.pos = output.position.xyz;\n\
	output.position = mul(output.position, cameraMatrix);\n\
	output.tex = input.tex;\n\
	output.normal = mul(input.normal, (float3x3)worldMatrix);\n\
	return output;\n\
}";

		ID3DBlob* shaderBuffer = nullptr;
		ID3DBlob* errorMessage = nullptr;
		HRESULT hr = D3DCompile(shader.c_str(), shader.length(), nullptr, nullptr, nullptr, "main", graphics.VS_Target(), D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &errorMessage);
		if (FAILED(hr))
		{
			if (errorMessage)
			{
				std::ofstream errorFile("vsError.txt");
				errorFile.write((char*)errorMessage->GetBufferPointer(), errorMessage->GetBufferSize());
				errorFile.close();
			}
			SAFE_RELEASE(errorMessage);
			SAFE_RELEASE(shaderBuffer);
			throw std::exception("Failed to compile vertex shader.");
		}
		SAFE_RELEASE(errorMessage);
		ID3D11Device* device = graphics.Device();
		hr = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
		if (FAILED(hr))
		{
			SAFE_RELEASE(shaderBuffer);
			throw std::exception("Failed to create vertex shader.");
		}
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[3];
		inputLayoutDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		inputLayoutDesc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		inputLayoutDesc[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		hr = device->CreateInputLayout(inputLayoutDesc, effect ? 2 : 3,
			shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &m_inputLayout);
		SAFE_RELEASE(shaderBuffer);
		if (FAILED(hr))
			throw std::exception("Failed to create input layout.");
	}
	VertexShader_DX::~VertexShader_DX()
	{
		SAFE_RELEASE(m_vertexShader);
		SAFE_RELEASE(m_inputLayout);
	}
	void VertexShader_DX::SetShader()
	{
		ID3D11DeviceContext* context = m_graphics.Context();
		context->IASetInputLayout(m_inputLayout);
		context->VSSetShader(m_vertexShader, nullptr, 0);
	}

#pragma endregion

#pragma region CBuffer

	CBuffer_DX::CBuffer_DX(Graphics_DX& graphics, unsigned size) :
		m_graphics(graphics),
		m_buffer(nullptr),
		m_size(size)
	{
		D3D11_BUFFER_DESC bufferDesc;
		HRESULT hr;
		ID3D11Device* device = graphics.Device();

		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = m_size;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = device->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
		if (FAILED(hr))
			throw std::exception("Failed to create constant buffer");
	}
	CBuffer_DX::~CBuffer_DX()
	{
		SAFE_RELEASE(m_buffer);
	}
	void CBuffer_DX::SetToVertexShader()
	{
		m_graphics.Context()->VSSetConstantBuffers(0, 1, &m_buffer);
	}
	void CBuffer_DX::SetToPixelShader()
	{
		m_graphics.Context()->PSSetConstantBuffers(0, 1, &m_buffer);
	}
	void CBuffer_DX::Write(void* data)
	{
		ID3D11DeviceContext* context = m_graphics.Context();
		D3D11_MAPPED_SUBRESOURCE resource;
		if (SUCCEEDED(context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
		{
			memcpy(resource.pData, data, m_size);
			context->Unmap(m_buffer, 0);
		}
	}

#pragma endregion
}