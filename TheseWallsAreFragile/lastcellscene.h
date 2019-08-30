#pragma once

#include "scene.h"

namespace twaf
{
	class LastCellScene :public Scene
	{
		Graphics& m_graphics;
		Texture* m_dialogTexture;
		int m_mousex, m_mousey;
		bool m_clickable;

		float m_totalTime;
		unsigned m_dialogIndex;

	private:
		void SetCursorIcon();
		bool OnDialogBox(int x, int y);
		void DialogClick();
		const wchar_t* DialogText();
		void CreateDialogWindow();

	public:
		LastCellScene(Graphics& graphics);
		~LastCellScene();

		virtual void Update(double deltaTime) override;
		virtual void MouseMove(int x, int y, int dx, int dy) override;
		virtual void MouseLButtonDown(int x, int y) override;
		virtual void MouseLButtonUp(int x, int y) override;
		virtual void MouseRButtonDown(int x, int y) override;
		virtual void MouseRButtonUp(int x, int y) override;
		virtual void KeyDown(int keyCode) override;
		virtual void KeyUp(int keyCode) override;
		virtual bool FileDrop(const wchar_t* filename) override;
		virtual bool AllowExit() override;
	};
}