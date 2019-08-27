#include "pch.h"
#include "image.h"
#include <wincodec.h>

#pragma comment (lib, "windowscodecs.lib")

namespace twaf
{
	Image::Image() :
		m_image(nullptr),
		m_width(0), 
		m_height(0) {}
	Image::Image(unsigned width, unsigned height) :
		m_image(new Color[(size_t)width * height]),
		m_width(width),
		m_height(height) 
	{
		memset(m_image, 0, (size_t)m_width * m_height * 4);
	}
	Image::Image(const Image& other) : 
		m_image(new Color[(size_t)other.m_width * other.m_height]),
		m_width(other.m_width), 
		m_height(other.m_height)
	{
		memcpy(m_image, other.m_image, (size_t)m_width * m_height * 4);
	}
	Image::Image(Image&& other) noexcept: 
		m_image(other.m_image), 
		m_width(other.m_width),
		m_height(other.m_height)
	{
		other.m_image = nullptr;
		other.m_width = 0;
		other.m_height = 0;
	}
	Image::Image(const wchar_t* filename) : Image()
	{
		if (!Load(filename)) Clear();
	}
	Image::~Image()
	{
		Clear();
	}

	Image& Image::operator=(const Image& image)
	{
		Clear();
		m_width = image.m_width;
		m_height = image.m_height;
		m_image = new Color[(size_t)m_width * m_height];
		memcpy(m_image, image.m_image, (size_t)m_width * m_height * 4);
		return *this;
	}

	void Image::ForEach(std::function<Color(Color c)> f)
	{
		for (unsigned y = 0; y < m_height; y++)
			for (unsigned x = 0; x < m_width; x++)
				setPixel(f(getPixel(x, y)), x, y);
	}

	bool Image::Load(const wchar_t* filename)
	{
		Clear();
		struct WIC_Stuff
		{
			IWICImagingFactory* factory;
			IWICBitmapDecoder* decoder;
			IWICBitmapFrameDecode* frame;
			IWICFormatConverter* converter;

			WIC_Stuff() :
				factory(nullptr),
				decoder(nullptr),
				frame(nullptr),
				converter(nullptr) {}
			~WIC_Stuff()
			{
				if (factory) factory->Release();
				if (decoder) decoder->Release();
				if (frame) frame->Release();
				if (converter) converter->Release();
			}
		};
		HRESULT hr;
		UINT frameCount;
		WIC_Stuff wic;

		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)& wic.factory);
		if (FAILED(hr)) return false;

		hr = wic.factory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wic.decoder);
		if (FAILED(hr)) return false;

		hr = wic.decoder->GetFrameCount(&frameCount);
		if (FAILED(hr) || frameCount == 0) return false;

		hr = wic.decoder->GetFrame(0, &wic.frame);
		if (FAILED(hr)) return false;

		hr = wic.factory->CreateFormatConverter(&wic.converter);
		if (FAILED(hr)) return false;

		hr = wic.converter->Initialize(wic.frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom);
		if (FAILED(hr)) return false;

		hr = wic.converter->GetSize(&m_width, &m_height);
		if (FAILED(hr)) return false;

		m_image = new Color[(size_t)m_width * m_height];
		if (m_image == nullptr)
		{
			m_width = 0; m_height = 0;
			return false;
		}

		hr = wic.converter->CopyPixels(nullptr, m_width * 4, m_width * m_height * 4, (BYTE*)m_image);
		if (FAILED(hr))
		{
			Clear();
			return false;
		}

		return true;
	}

	void Image::Clear()
	{
		if (m_image)
		{
			delete[]m_image;
			m_width = 0;
			m_height = 0;
		}
	}

	Color& Image::getPixel(int x, int y)
	{
		return m_image[m_width * y + x];
	}

	Color& Image::operator()(int x, int y)
	{
		return getPixel(x, y);
	}

	Color Image::getPixel(int x, int y) const
	{
		return m_image[m_width * y + x];
	}

	Color Image::operator()(int x, int y) const
	{
		return getPixel(x, y);
	}

	void Image::setPixel(Color c, int x, int y)
	{
		m_image[m_width * y + x] = c;
	}

}