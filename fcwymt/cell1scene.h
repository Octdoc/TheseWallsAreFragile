#pragma once

#include "TheseWallsAreFragile/scene.h"
#include "TheseWallsAreFragile/math/firstpersonpositioncontroller.hpp"

namespace twaf
{
	class Cell1Scene :public Scene
	{
		Graphics& m_graphics;
		Model* m_roomModel;
		Texture* m_roomTexture;

		Model* m_imageModel;
		Texture* m_oldmanTexture;
		Positionf m_oldman;
		Texture* m_doorTexture;
		Positionf m_door;

		PixelShader* m_pixelShader;
		int m_mousex, m_mousey;
		bool m_clickable;

		std::wstring m_userName;
		Texture* m_dialogTexture;
		unsigned m_dialogIndex;
		bool m_dialogConditionMet;
		bool m_dialogClicked;
		float m_timeCounter;
		float m_brightness;
		float4 m_lightColor;

	private:
		void SetCursorIcon();
		bool OnDialogBox(int x, int y);
		void DialogClick();
		const wchar_t* DialogText();
		void CreateDialogWindow();
		void UpdateDialog();
		void GoOutTheDoor();
		void FirstUpdateLoop(float deltaTime);
		void SecondUpdateLoop(float deltaTime);

	public:
		Cell1Scene(Graphics& graphics);
		~Cell1Scene();

		virtual void Update(double deltaTime) override;
		virtual void MouseMove(int x, int y, int dx, int dy) override;
		virtual void MouseLButtonDown(int x, int y) override;
		virtual void MouseLButtonUp(int x, int y) override;
		virtual void MouseRButtonDown(int x, int y) override;
		virtual void MouseRButtonUp(int x, int y) override;
		virtual void KeyDown(int keyCode) override;
		virtual void KeyUp(int keyCode) override;
		virtual bool FileDrop(const wchar_t* filename) override;
	};
}