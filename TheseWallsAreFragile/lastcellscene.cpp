#include "pch.h"
#include "lastcellscene.h"

namespace twaf
{
	void LastCellScene::SetCursorIcon()
	{
		bool clickable = false;
		if (m_dialogTexture)
			if (OnDialogBox(m_mousex, m_mousey))
				clickable = true;
		if (m_clickable != clickable)
		{
			m_clickable = clickable;
			SetCursor(LoadCursor(NULL, m_clickable ? IDC_HAND : IDC_ARROW));
		}
	}
	bool LastCellScene::OnDialogBox(int x, int y)
	{
		return x > 65 && x < 834 && y > 339 && y < 484;
	}
	void LastCellScene::DialogClick()
	{
		CreateDialogWindow();
	}
	const wchar_t* LastCellScene::DialogText()
	{
		LPCWSTR dialog[] = {
			L"You came here 5 seconds ago and did\nnothing.",
			L"Why are you here? Weren't you meant to\nbe somewhere else?",
			L"You might want to visit the old man\nfirst. He is over there in \"cell1.bat\"."
		};
		if ((sizeof(dialog) / sizeof(LPCWSTR)) <= m_dialogIndex)
			return nullptr;
		return dialog[m_dialogIndex++];
	}
	void LastCellScene::CreateDialogWindow()
	{
		SAFE_DELETE(m_dialogTexture);
		const wchar_t* text = DialogText();
		if (text)
			m_dialogTexture = m_graphics.CreateTextTexture(text);
		else
			PostQuitMessage(0);
	}
	LastCellScene::LastCellScene(Graphics& graphics) :
		m_graphics(graphics),
		m_dialogTexture(nullptr),
		m_mousex(0), m_mousey(0),
		m_clickable(false),
		m_totalTime(0.0f),
		m_dialogIndex(0)
	{
		graphics.UseEffect(L"Media/no.effect", 180, 100);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	LastCellScene::~LastCellScene()
	{
		SAFE_DELETE(m_dialogTexture);
	}
	void LastCellScene::Update(double deltaTime)
	{
		SetCursorIcon();
		m_graphics.ClearScreen();
		m_totalTime += deltaTime;
		if (m_dialogIndex == 0 && m_dialogTexture == nullptr)
			if (m_totalTime > 5.0f)
				CreateDialogWindow();
		if (m_dialogTexture)
			m_graphics.RenderDialog(m_dialogTexture);
		float effectBuffer[8] = { 1.0f };
		m_graphics.WriteEffectBuffer(effectBuffer);
		m_graphics.Present();
	}
	void LastCellScene::MouseMove(int x, int y, int dx, int dy)
	{
		m_mousex = x;
		m_mousey = y;
	}
	void LastCellScene::MouseLButtonDown(int x, int y)
	{
	}
	void LastCellScene::MouseLButtonUp(int x, int y)
	{
		if (OnDialogBox(x, y))
			DialogClick();
	}
	void LastCellScene::MouseRButtonDown(int x, int y)
	{
	}
	void LastCellScene::MouseRButtonUp(int x, int y)
	{
		if (OnDialogBox(x, y))
			DialogClick();
	}
	void LastCellScene::KeyDown(int keyCode)
	{
	}
	void LastCellScene::KeyUp(int keyCode)
	{
		switch (keyCode)
		{
		case VK_RETURN:
		case VK_SPACE:
			DialogClick();
			break;
		}
	}
	bool LastCellScene::FileDrop(const wchar_t* filename)
	{
		return false;
	}
}