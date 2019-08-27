#pragma once

//#define FREE_LOOK

#include "escapestage.h"
#ifdef FREE_LOOK
#include "TheseWallsAreFragile/math/firstpersonpositioncontroller.hpp"
#endif

#define STAGE_COUNT 10

namespace twaf
{
	class EscapeScene :public Scene
	{
		Graphics& m_graphics;
		Texture* m_dialogTexture;
#ifdef FREE_LOOK
		FirstPersonPositionController<float> m_camController;
		bool m_isLeftButtonDown;
#endif
		bool m_shiftScreen;
		float m_timeCounter;
		float m_shiftStartX;
		float4 m_lightColor;
		float3 m_visibleColors;
		float m_brightness;

		EscapeStage* m_stages[STAGE_COUNT];

		Model* m_imageModel;
		Texture* m_oldmanTexture;
		Positionf m_oldman;
		int m_currentStage;
		bool m_endDoorPlaced;

		PixelShader* m_pixelShader;
		int m_mousex, m_mousey;
		bool m_clickable;

		unsigned m_dialogIndex;
		std::wstring m_userName;

	private:
		void SetCursorIcon();
		void CreateStages();

		bool OnDialogBox(int x, int y);
		void DialogClick();
		bool OnOldman(int x, int y);
		void OldmanClick();
		bool OnDoor(int x, int y);
		void DoorClick();
		
		void StartScreenShift();
		void FirstUpdate(float deltaTime);
		void SecondUpdate(float deltaTime);

		const wchar_t* EpilogDialog();
		void CreateDialogWindow();

	public:
		EscapeScene(Graphics& graphics);
		~EscapeScene();

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