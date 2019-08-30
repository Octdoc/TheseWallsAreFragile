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
		int m_step;

		std::wstring m_userName;
		Texture* m_dialogTexture;
		bool m_dialogConditionMet;
		bool m_dialogClicked;
		float m_timeCounter;
		float m_brightness;
		float4 m_lightColor;

	private:
		bool OnDialogBox(int x, int y);
		bool OnOldman(int x, int y);
		bool OnDoor(int x, int y);
		bool CanClickOldman();
		bool CanClickDialog();
		bool CanClickDoor();
		void SetCursorIcon();
		void ClickEvent(int x, int y);
		void NewDialogbox(const wchar_t* text);
		void ProgressStory(int newStep);
		void TimedProgressionCheck();

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
		virtual bool AllowExit() override;
	};
}