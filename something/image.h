#pragma once

#include <functional>

namespace twaf
{
	struct Color
	{
		unsigned char blue;
		unsigned char green;
		unsigned char red;
		unsigned char alpha;
	};

	class Image
	{
		Color* m_image;
		unsigned m_width, m_height;

	public:
		Image();
		Image(unsigned width, unsigned height);
		Image(const Image& other);
		Image(Image&& other) noexcept;
		Image(const wchar_t* filename);
		~Image();

		Image& operator=(const Image& image);

		void ForEach(std::function<Color(Color c)> f);
		bool Load(const wchar_t* filename);
		void Clear();

		inline bool Empty() const { return m_image == nullptr; }
		inline int Width() const { return m_width; }
		inline int Height() const { return m_height; }
		inline Color* Pixels() const { return m_image; }
		Color& getPixel(int x, int y);
		Color& operator()(int x, int y);
		Color getPixel(int x, int y) const;
		Color operator()(int x, int y) const;
		void setPixel(Color c, int x, int y);
	};
}