#pragma once

#include "pch.h"
#include "graphics.h"

namespace twaf
{
	class Scene
	{
	public:
		virtual ~Scene() = default;
		virtual void Update(double deltaTime) = 0;
		virtual void MouseMove(int x, int y, int dx, int dy) = 0;
		virtual void MouseLButtonDown(int x, int y) = 0;
		virtual void MouseLButtonUp(int x, int y) = 0;
		virtual void MouseRButtonDown(int x, int y) = 0;
		virtual void MouseRButtonUp(int x, int y) = 0;
		virtual void KeyDown(int keyCode) = 0;
		virtual void KeyUp(int keyCode) = 0;
		virtual bool FileDrop(const wchar_t* filename) = 0;
		virtual bool AllowExit() = 0;
	};
}