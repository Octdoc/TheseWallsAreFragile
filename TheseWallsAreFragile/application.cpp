#include "pch.h"
#include "application.h"
#include "lastcellscene.h"
#include "resource.h"

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return ((twaf::Application*)GetWindowLongPtr(hwnd, GWLP_USERDATA))->MessageHandler(hwnd, msg, wparam, lparam);
}
static LRESULT CALLBACK InitialWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_CREATE)
	{
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(((CREATESTRUCT*)lparam)->lpCreateParams));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
		return WndProc(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

namespace twaf
{
	void Application::FileDropEvent(HDROP hDrop)
	{
		WCHAR filename[MAX_PATH];
		filename[0] = '\0';
		DragQueryFile(hDrop, 0, filename, MAX_PATH);

		bool usedFile = false;
		if (m_scene)
			usedFile = m_scene->FileDrop(filename);
		if (!usedFile)
			LoadFile(filename);
		SetFocus(m_mainWindow);

		DragFinish(hDrop);
		SetForegroundWindow(m_mainWindow);
	}
	void Application::MouseMoveEvent(LPARAM lparam)
	{
		LONG x = LOWORD(lparam);
		LONG y = HIWORD(lparam);
		if (m_scene)
			m_scene->MouseMove(x, y, x - m_cursor.x, y - m_cursor.y);
		m_cursor.x = x;
		m_cursor.y = y;
	}
	void Application::MouseLButtonDownEvent(LPARAM lparam)
	{
		if (m_scene)
			m_scene->MouseLButtonDown(LOWORD(lparam), HIWORD(lparam));
	}
	void Application::MouseLButtonUpEvent(LPARAM lparam)
	{
		if (m_scene)
			m_scene->MouseLButtonUp(LOWORD(lparam), HIWORD(lparam));
	}
	void Application::MouseRButtonDownEvent(LPARAM lparam)
	{
		if (m_scene)
			m_scene->MouseRButtonDown(LOWORD(lparam), HIWORD(lparam));
	}
	void Application::MouseRButtonUpEvent(LPARAM lparam)
	{
		if (m_scene)
			m_scene->MouseRButtonUp(LOWORD(lparam), HIWORD(lparam));
	}
	void Application::KeyDownEvent(WPARAM wparam)
	{
		if (m_scene)
			m_scene->KeyDown((int)wparam);
	}
	void Application::KeyUpEvent(WPARAM wparam)
	{
		if (m_scene)
			m_scene->KeyUp((int)wparam);
	}
	void Application::Update(double deltaTime)
	{
		if (m_scene)
			m_scene->Update(deltaTime);
		else
		{
			m_graphics.ClearScreen();
			m_graphics.Present();
		}
	}

	void Application::LoadFile(const wchar_t* filename)
	{
		std::wstring extension;
		unsigned lastDotIndex = 0;
		for (unsigned i = 0; filename[i]; i++)
			if (filename[i] == '.')
				lastDotIndex = i;
		for (unsigned i = lastDotIndex; filename[i]; i++)
		{
			wchar_t ch = filename[i];
			if (ch >= 'A' && ch <= 'Z')
				ch = ch - 'A' + 'a';
			extension += ch;
		}

		if (extension == L".dll")
			LoadDLL(filename);
	}

	void Application::LoadDLL(const wchar_t* filename)
	{
		SAFE_DELETE(m_scene);
		if (m_dll)
			FreeLibrary(m_dll);
		m_dll = LoadLibrary(filename);

		if (m_dll)
		{
			typedef Scene* (__stdcall * CreateScene_Function)(Graphics & graphics);
			CreateScene_Function CreateScene = (CreateScene_Function)GetProcAddress(m_dll, "CreateScene");
			if (CreateScene)
				m_scene = CreateScene(m_graphics);
			else
			{
				FreeLibrary(m_dll);
				m_dll = NULL;
			}
		}
	}

	Application::Application() :
		m_dll(NULL),
		m_mainWindow(NULL),
		m_graphics(),
		m_scene(nullptr),
		m_cursor({ 0, 0 }) {}

	Application::~Application()
	{
		SAFE_DELETE(m_scene);
		if (m_dll)
			FreeLibrary(m_dll);
		m_graphics.Shutdown();
	}

	void Application::Init(HINSTANCE hInstance, int width, int height)
	{
		WNDCLASSEX wc{};
		wc.cbSize = sizeof(wc);
		wc.hInstance = hInstance;
		wc.hIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wc.lpfnWndProc = InitialWndProc;
		wc.lpszClassName = APPLICATION_NAME;
		RegisterClassEx(&wc);
		DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;
		RECT rect{};
		rect.right = width;
		rect.bottom = height;
		AdjustWindowRectEx(&rect, style, FALSE, exStyle);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		CreateWindowEx(exStyle, APPLICATION_NAME, APPLICATION_NAME, style,
			max(0, (GetSystemMetrics(SM_CXSCREEN) - width) / 2), max(0, (GetSystemMetrics(SM_CYSCREEN) - height) / 2),
			width, height, NULL, NULL, hInstance, (LPVOID)this);
		DragAcceptFiles(m_mainWindow, TRUE);
		GetCursorPos(&m_cursor);
		ScreenToClient(m_mainWindow, &m_cursor);
		ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
		UpdateWindow(m_mainWindow);
		m_graphics.Init(m_mainWindow);
	}

	void Application::Run(std::vector<std::wstring>& cmdLine)
	{
		if (cmdLine.size() > 0)
			LoadFile(cmdLine[0].c_str());
		else
			m_scene = new LastCellScene(m_graphics);
		MSG msg{};
		auto prevTime = std::chrono::steady_clock::now();
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				auto currentTime = std::chrono::steady_clock::now();
				double deltaTime = std::chrono::duration<double>(currentTime - prevTime).count();
				prevTime = currentTime;
				Update(min(deltaTime, 0.05f));
			}
		}
	}

	LRESULT Application::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_CREATE:
			m_mainWindow = hwnd;
			return 0;
		case WM_DROPFILES:
			FileDropEvent((HDROP)wparam);
			return 0;
		case WM_MOUSEMOVE:
			MouseMoveEvent(lparam);
			return 0;
		case WM_LBUTTONDOWN:
			MouseLButtonDownEvent(lparam);
			return 0;
		case WM_LBUTTONUP:
			MouseLButtonUpEvent(lparam);
			return 0;
		case WM_RBUTTONDOWN:
			MouseRButtonDownEvent(lparam);
			return 0;
		case WM_RBUTTONUP:
			MouseRButtonUpEvent(lparam);
			return 0;
		case WM_KEYDOWN:
			KeyDownEvent(wparam);
			return 0;
		case WM_KEYUP:
			KeyUpEvent(wparam);
			return 0;
		case WM_CLOSE:
			if (!m_scene || m_scene->AllowExit())
				PostQuitMessage(0);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}