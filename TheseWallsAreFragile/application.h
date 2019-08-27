#pragma once

#include "pch.h"
#include "scene.h"
#include "graphics_dx.h"

namespace twaf
{
	class Application
	{
		HINSTANCE m_dll;
		HWND m_mainWindow;
		Graphics_DX m_graphics;
		Scene* m_scene;
		POINT m_cursor;

	private:
		void FileDropEvent(HDROP hDrop);
		void MouseMoveEvent(LPARAM lparam);
		void MouseLButtonDownEvent(LPARAM lparam);
		void MouseLButtonUpEvent(LPARAM lparam);
		void MouseRButtonDownEvent(LPARAM lparam);
		void MouseRButtonUpEvent(LPARAM lparam);
		void KeyDownEvent(WPARAM wparam);
		void KeyUpEvent(WPARAM wparam);
		void Update(double deltaTime);

		void LoadFile(const wchar_t* filename);
		void LoadDLL(const wchar_t* filename);

	public:
		Application();
		~Application();
		void Init(HINSTANCE hInstance, int width, int height);
		void Run(std::vector<std::wstring>& cmdLine);
		LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};
}