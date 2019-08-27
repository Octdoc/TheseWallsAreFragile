#include "escapescene.h"

#pragma comment(lib, "winmm.lib")

namespace twaf
{
	void EscapeScene::SetCursorIcon()
	{
		bool clickable = false;
		if (OnOldman(m_mousex, m_mousey))
			clickable = true;
		if (OnDialogBox(m_mousex, m_mousey))
			clickable = true;
		if (OnDoor(m_mousex, m_mousey))
			clickable = true;
		if (m_clickable != clickable)
		{
			m_clickable = clickable;
			SetCursor(LoadCursor(NULL, m_clickable ? IDC_HAND : IDC_ARROW));
		}
	}
	void EscapeScene::CreateStages()
	{
		EscapeStage::LoadResources(m_graphics);
		m_stages[0] = new EscapeStage0(m_graphics);
		m_stages[1] = new EscapeStage1(m_graphics);
		m_stages[2] = new EscapeStage2(m_graphics);
		m_stages[3] = new EscapeStage3(m_graphics);
		m_stages[4] = new EscapeStage4(m_graphics);
		m_stages[5] = new EscapeStage5(m_graphics);
		m_stages[6] = new EscapeStage6(m_graphics);
		m_stages[7] = new EscapeStage7(m_graphics);
		m_stages[8] = new EscapeStage8(m_graphics);
		m_stages[9] = new EscapeStage9(m_graphics);
	}
	bool EscapeScene::OnDialogBox(int x, int y)
	{
		return m_dialogTexture != nullptr && x > 65 && x < 834 && y > 339 && y < 484;
	}
	bool EscapeScene::OnOldman(int x, int y)
	{
		bool ret = m_mousex > 29 && m_mousex < 190 && m_mousey > 264 && m_mousey < 420;
		if (m_currentStage == 9 && m_stages[m_currentStage]->Solved())
			return false;
		return ret;
	}
	void EscapeScene::OldmanClick()
	{
		if (m_stages[m_currentStage]->Solved())
		{
			SAFE_DELETE(m_dialogTexture);
			StartScreenShift();
		}
		else
		{
			const wchar_t* issueDialog = m_stages[m_currentStage]->IssueDialog();
			if (issueDialog[0])
				m_dialogTexture = m_graphics.CreateTextTexture(issueDialog);
		}
	}
	bool EscapeScene::OnDoor(int x, int y)
	{
		return m_currentStage == 9 && m_stages[m_currentStage]->Solved() && m_mousex > 214 && m_mousex < 400 && m_mousey > 139 && m_mousey < 400;
	}
	void EscapeScene::DoorClick()
	{
		m_brightness -= 0.001f;
		m_timeCounter = 0.0f;
	}
	void EscapeScene::DialogClick()
	{
		SAFE_DELETE(m_dialogTexture);
		if (m_brightness < 0.0f)
		{
			CreateDialogWindow();
		}
	}
	void EscapeScene::StartScreenShift()
	{
		m_shiftScreen = true;
		m_timeCounter = 0.0f;
		m_shiftStartX = m_graphics.getCamera().position.x;
		m_currentStage++;
	}
	void EscapeScene::FirstUpdate(float deltaTime)
	{
		SetCursorIcon();
		m_timeCounter += deltaTime;
#ifdef FREE_LOOK
		m_camController.Update(deltaTime);
#endif
		m_stages[m_currentStage]->visibleColors = m_visibleColors;
		m_stages[m_currentStage]->lightColor = float3(m_lightColor.x, m_lightColor.y, m_lightColor.z);
		m_stages[m_currentStage]->Update(deltaTime);
		if (m_shiftScreen)
		{
			float shiftTime = 2.0f / 3.0f;
			m_graphics.getCamera().position.x = m_shiftStartX + 2.5f * m_timeCounter / shiftTime;
			if (m_timeCounter > shiftTime)
			{
				m_graphics.getCamera().position.x = m_shiftStartX + 2.5f;
				m_shiftScreen = false;
				const wchar_t* enterDialog = m_stages[m_currentStage]->EnterDialog();
				if (enterDialog[0])
					m_dialogTexture = m_graphics.CreateTextTexture(enterDialog);
			}
		}

		m_graphics.UpdateCamera();
		m_graphics.ClearScreen();

		m_graphics.SetDefaultVertexShader();
		m_pixelShader->SetShader();
		for (int i = max(m_currentStage - 2, 0); i < min(m_currentStage + 2, STAGE_COUNT); i++)
			m_stages[i]->Render(m_lightColor, 0.25f);

		m_oldman.position.x = m_graphics.getCamera().position.x - 1.0f;
		m_graphics.WriteLightData(
			m_lightColor,
			float3(m_oldman.position.x, 0.75f, -0.75f),
			0.25f);
		m_oldmanTexture->SetTexture();
		m_graphics.WriteEntityData(m_oldman);
		m_imageModel->Render();

		if (m_dialogTexture)
			m_graphics.RenderDialog(m_dialogTexture, m_oldmanTexture, float2(830.0f, 430.0f), float2(64.0f));

		if (m_brightness < 1.0f)
		{
			m_brightness -= deltaTime * 0.6f;
			if (m_brightness < 0.0f)
			{
				m_timeCounter = 0.0f;
				m_dialogIndex = 0;
			}
		}

		float effectBuffer[8] = { m_brightness, 180.0f, 100.0f };
		m_graphics.WriteEffectBuffer(effectBuffer);
		m_graphics.Present();
	}
	void EscapeScene::SecondUpdate(float deltaTime)
	{
		m_timeCounter += deltaTime;
		bool clickable = false;
		if (OnDialogBox(m_mousex, m_mousey))
			clickable = true;
		if (m_clickable != clickable)
		{
			m_clickable = clickable;
			SetCursor(LoadCursor(NULL, m_clickable ? IDC_HAND : IDC_ARROW));
		}

		if (m_dialogTexture == nullptr && m_timeCounter > 1.5f)
		{
			CreateDialogWindow();
		}
		if (m_timeCounter < 0.0f && m_timeCounter > -18.5f)
			PostQuitMessage(0);

		m_graphics.ClearScreen();
		if (m_dialogTexture)
			m_graphics.RenderDialog(m_dialogTexture, m_oldmanTexture, float2(830.0f, 430.0f), float2(64.0f));
		m_graphics.Present();
	}
	const wchar_t* EscapeScene::EpilogDialog()
	{
		static LPCWSTR dialog[] = {
			L"You rescued me and for that I am\neternally thankful.",
			L"As old as I am I do not have much time\nto enjoy my new found freedom,",
			L"but the time I have will not go to waste.",
			L"",
			L"Thank you for your time and effort.",
			L" "
		};
		if ((sizeof(dialog) / sizeof(LPCWSTR)) <= m_dialogIndex)
			return nullptr;
		return dialog[m_dialogIndex++];
	}
	void EscapeScene::CreateDialogWindow()
	{
		SAFE_DELETE(m_dialogTexture);
		const wchar_t* text = EpilogDialog();
		if (text)
		{
			if (text[0])
			{
				if (text[0] != ' ')
				{
					m_dialogTexture = m_graphics.CreateTextTexture(text);
				}
				else
				{
					m_timeCounter = -20.0f;
				}
			}
			else
			{
				std::wstring textMaker = L"Thank you, " + m_userName + L"!";
				m_dialogTexture = m_graphics.CreateTextTexture(textMaker.c_str());
			}
		}
	}
	EscapeScene::EscapeScene(Graphics& graphics) :
		m_graphics(graphics),
		m_dialogTexture(nullptr),
#ifdef FREE_LOOK
		m_camController(graphics.getCamera()),
		m_isLeftButtonDown(false),
#endif
		m_shiftScreen(false),
		m_timeCounter(0.0f),
		m_shiftStartX(0.0f),
		m_stages{},
		m_imageModel(nullptr),
		m_oldmanTexture(nullptr),
		m_oldman(),
		m_currentStage(1),	//TODO: need to be 1
		m_endDoorPlaced(false),
		m_pixelShader(nullptr),
		m_mousex(0), m_mousey(0),
		m_clickable(false),
		m_dialogIndex(0)
	{
		m_lightColor = float4(1.0f, 0.8f, 0.5f, 1.0f);
		m_visibleColors = 1.0f;
		m_brightness = 1.0f;
		graphics.SetCameraProjective((float)PI * 0.175f, 1.8f);
		m_graphics.getCamera().position = float3(-0.3f + (float)m_currentStage * 2.5f, 0.417f, -3.28f);
		m_graphics.getCamera().rotation = float3(-0.042f, 0.016f, 0.0f);
		graphics.UseEffect(L"Media/no.effect", 180, 100);
		m_pixelShader = graphics.LoadPixelShader(L"other world/redgreenblue.eye");

		m_imageModel = graphics.LoadModel(L"Media/oldman.bmd");
		m_oldmanTexture = graphics.LoadTexture(L"Media/oldman.png");
		m_oldman.position.z = -0.5f;
		m_oldman.scale = 0.5f;

		CreateStages();
		const wchar_t* enterDialog = m_stages[m_currentStage]->EnterDialog();
		if (enterDialog[0])
			m_dialogTexture = m_graphics.CreateTextTexture(enterDialog);
		wchar_t userName[64];
		DWORD length = 64;
		GetUserName(userName, &length);
		m_userName = userName;

		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	EscapeScene::~EscapeScene()
	{
		SAFE_DELETE(m_dialogTexture);
		SAFE_DELETE(m_imageModel);
		SAFE_DELETE(m_oldmanTexture);
		SAFE_DELETE(m_pixelShader);
		for (int i = 0; i < STAGE_COUNT; i++)
			SAFE_DELETE(m_stages[i]);
		EscapeStage::FreeResources();
	}
	void EscapeScene::Update(double deltaTime)
	{
		if (m_brightness > 0.0f)
			FirstUpdate(deltaTime);
		else
			SecondUpdate(deltaTime);
	}
	void EscapeScene::MouseMove(int x, int y, int dx, int dy)
	{
		m_mousex = x;
		m_mousey = y;
#ifdef FREE_LOOK
		if (m_isLeftButtonDown)
			m_camController.MouseMove(dx, dy);
#endif
	}
	void EscapeScene::MouseLButtonDown(int x, int y)
	{
#ifdef FREE_LOOK
		m_isLeftButtonDown = true;
#endif
	}
	void EscapeScene::MouseLButtonUp(int x, int y)
	{
#ifdef FREE_LOOK
		m_isLeftButtonDown = false;
#endif
		if (OnDialogBox(x, y))
			DialogClick();
		else if (OnOldman(x, y))
			OldmanClick();
		else if (OnDoor(x, y))
			DoorClick();
	}
	void EscapeScene::MouseRButtonDown(int x, int y)
	{
	}
	void EscapeScene::MouseRButtonUp(int x, int y)
	{
		if (OnDialogBox(x, y))
			DialogClick();
		else if (OnOldman(x, y))
			OldmanClick();
		else if (OnDoor(x, y))
			DoorClick();
	}
	void EscapeScene::KeyDown(int keyCode)
	{
#ifdef FREE_LOOK
		m_camController.KeyDown(keyCode);
#endif
	}
	void EscapeScene::KeyUp(int keyCode)
	{
#ifdef FREE_LOOK
		m_camController.KeyUp(keyCode);
#endif
		switch (keyCode)
		{
		case VK_RETURN:
		case VK_SPACE:
			DialogClick();
			break;
		}
	}
	bool EscapeScene::FileDrop(const wchar_t* filename)
	{
		SAFE_DELETE(m_dialogTexture);

		std::wstring name;
		std::wstring extension;
		unsigned lastSlashIndex = 0;
		unsigned lastDotIndex = 0;
		for (unsigned i = 0; filename[i]; i++)
		{
			if (filename[i] == '/' || filename[i] == '\\')
				lastSlashIndex = i;
			if (filename[i] == '.')
				lastDotIndex = i;
		}
		for (unsigned i = lastSlashIndex + 1; filename[i] != '.' && filename[i] != '\0'; i++)
			name += filename[i];
		for (unsigned i = lastDotIndex; filename[i]; i++)
		{
			wchar_t ch = filename[i];
			if (ch >= 'A' && ch <= 'Z')
				ch = ch - 'A' + 'a';
			extension += ch;
		}

		m_stages[m_currentStage]->FileDrop(filename, name, extension);

		if (extension == L".eye")
		{
			try
			{
				PixelShader* pixelShader = m_graphics.LoadPixelShader(filename);
				SAFE_DELETE(m_pixelShader);
				m_pixelShader = pixelShader;
				if (name == L"nored")
					m_visibleColors = float3(0.0f, 1.0f, 1.0f);
				else if (name == L"nogreen")
					m_visibleColors = float3(1.0f, 0.0f, 1.0f);
				else if (name == L"noblue")
					m_visibleColors = float3(1.0f, 1.0f, 0.0f);
				else if (name == L"redgreenblue")
					m_visibleColors = float3(1.0f, 1.0f, 1.0f);
			}
			catch (...) {}
		}
		if (extension == L".light")
		{
			std::ifstream infile(filename);
			if (infile.is_open())
			{
				std::string buffer;
				int red = 1.0f, green = 1.0f, blue = 1.0f;
				infile >> buffer;
				if (buffer == "red:")
					infile >> red;
				infile >> buffer;
				if (buffer == "green:")
					infile >> green;
				infile >> buffer;
				if (buffer == "blue:")
					infile >> blue;
				m_lightColor.x = (float)red / 255.0f;
				m_lightColor.y = (float)green / 255.0f;
				m_lightColor.z = (float)blue / 255.0f;
				m_lightColor.w = 1.0f;
				infile.close();
				return true;
			}
		}
		if (extension == L".effect")
		{
			try
			{
				m_graphics.UseEffect(filename, 180, 100);
			}
			catch (...) {}
		}
		if (extension == L".wav")
		{
			PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);
		}
		//if (m_currentStage == 9 && name == L"door" && extension == L".png")
		//{
		//
		//}

		return false;
	}
	}