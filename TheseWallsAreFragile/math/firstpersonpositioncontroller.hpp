#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "position.hpp"

namespace twaf
{
	template <typename T>
	class FirstPersonPositionController
	{
		Position<T>& m_target;
		unsigned m_keyFlags;

	public:
		T sensitivity;
		T speed;

	public:
		FirstPersonPositionController(Position<T>& target) :
			m_target(target),
			m_keyFlags(0),
			sensitivity(0.008),
			speed(1.0) {}
		void Update(T deltaTime)
		{
			T delta = speed * deltaTime;
			if (m_keyFlags & 0x40)
				delta *= 5;
			if (m_keyFlags & 0x02)
				m_target.MoveLeft(delta);
			if (m_keyFlags & 0x08)
				m_target.MoveRight(delta);
			if (m_keyFlags & 0x01)
				m_target.MoveForward(delta);
			if (m_keyFlags & 0x04)
				m_target.MoveBackward(delta);
			if (m_keyFlags & 0x20)
				m_target.MoveUp(delta);
			if (m_keyFlags & 0x10)
				m_target.MoveDown(delta);
		}
		void MouseMove(int dx, int dy)
		{
			m_target.TurnRight(sensitivity * dx);
			m_target.LookDown(sensitivity * dy);
		}
		void KeyDown(unsigned keyCode)
		{
			switch (keyCode)
			{
			case 'W':
				m_keyFlags |= 0x01;
				break;
			case 'A':
				m_keyFlags |= 0x02;
				break;
			case 'S':
				m_keyFlags |= 0x04;
				break;
			case 'D':
				m_keyFlags |= 0x08;
				break;
			case VK_CONTROL:
				m_keyFlags |= 0x10;
				break;
			case VK_SPACE:
				m_keyFlags |= 0x20;
				break;
			case VK_SHIFT:
				m_keyFlags |= 0x40;
				break;
			}
		}
		void KeyUp(unsigned keyCode)
		{
			switch (keyCode)
			{
			case 'W':
				m_keyFlags &= ~0x01;
				break;
			case 'A':
				m_keyFlags &= ~0x02;
				break;
			case 'S':
				m_keyFlags &= ~0x04;
				break;
			case 'D':
				m_keyFlags &= ~0x08;
				break;
			case VK_CONTROL:
				m_keyFlags &= ~0x10;
				break;
			case VK_SPACE:
				m_keyFlags &= ~0x20;
				break;
			case VK_SHIFT:
				m_keyFlags &= ~0x40;
				break;
			}
		}
	};
}