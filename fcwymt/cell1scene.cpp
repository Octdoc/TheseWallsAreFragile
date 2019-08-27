#include "cell1scene.h"

namespace twaf
{
	void Cell1Scene::SetCursorIcon()
	{
		bool clickable = false;
		if (m_doorTexture)
			if (m_dialogIndex > 11 && m_dialogTexture == nullptr)
				if (m_mousex > 370 && m_mousex < 525 && m_mousey > 170 && m_mousey < 390)
					clickable = true;
		if (m_dialogIndex == 0)
			if (m_mousex > 574 && m_mousex < 720 && m_mousey > 254 && m_mousey < 400)
				clickable = true;
		if (m_dialogTexture)
			if (OnDialogBox(m_mousex, m_mousey))
				clickable = true;
		if (m_clickable != clickable)
		{
			m_clickable = clickable;
			SetCursor(LoadCursor(NULL, m_clickable ? IDC_HAND : IDC_ARROW));
		}
	}
	bool Cell1Scene::OnDialogBox(int x, int y)
	{
		return x > 65 && x < 834 && y > 339 && y < 484;
	}
	void Cell1Scene::DialogClick()
	{
		SAFE_DELETE(m_dialogTexture);
		m_dialogClicked = true;
		if ((m_dialogIndex == 4 || m_dialogIndex == 7) && m_dialogConditionMet)
		{
			m_dialogConditionMet = false;
			m_timeCounter = 0.0f;
		}
	}
	const wchar_t* Cell1Scene::DialogText()
	{
		static LPCWSTR dialog[] = {
			L"",
			L"You are the one with the otherworldly\npowers.",
			L"I was wondering if you would come.",
			L"I will be here whenever you need\nguidance.",
			L"Aren't you bored?",
			L"I know a way out. There was a door.png\nnext to where you came in.",
			L"You have the power to install it on the\nwall.",
			L"Before you go. You need to know one\nthing.",
			L"This world is incomplete.",
			L"I cannot escape because nothing exists\noutside of this prison.",
			L"You however have eyes not of this world\nand arms passing things over.",
			L"You can make it out. And when you do,\nI ask you to rescue me too."
		};
		if ((sizeof(dialog) / sizeof(LPCWSTR)) <= m_dialogIndex)
			return nullptr;
		return dialog[m_dialogIndex++];
	}
	void Cell1Scene::CreateDialogWindow()
	{
		SAFE_DELETE(m_dialogTexture);
		const wchar_t* text = DialogText();
		if (text)
		{
			if (text[0])
			{
				m_dialogTexture = m_graphics.CreateTextTexture(text);
			}
			else
			{
				std::wstring textMaker;
				switch (m_dialogIndex)
				{
				case 1:
					textMaker = L"Welcome. You are " + m_userName + L", yes?";
					break;
				}
				m_dialogTexture = m_graphics.CreateTextTexture(textMaker.c_str());
			}
		}
	}
	void Cell1Scene::UpdateDialog()
	{
		if (!m_dialogConditionMet)
		{
			if (m_dialogIndex == 4)
			{
				if (m_timeCounter > 5.0f)
				{
					m_dialogConditionMet = true;
					m_dialogClicked = true;
				}
			}
			if (m_dialogIndex == 7)
			{
				if (m_timeCounter > 1.5f && m_doorTexture != nullptr)
				{
					m_dialogConditionMet = true;
					m_dialogClicked = true;
				}
				if (m_timeCounter > 15.0f && m_doorTexture == nullptr)
				{
					SAFE_DELETE(m_dialogTexture);
					m_dialogTexture = m_graphics.CreateTextTexture(L"Simply drop the file on the window.");
				}
			}
		}
		if (m_dialogConditionMet && m_dialogClicked)
			CreateDialogWindow();
		m_dialogClicked = false;
		if (m_dialogTexture)
			m_graphics.RenderDialog(m_dialogTexture, m_oldmanTexture, float2(830.0f, 430.0f), float2(64.0f));
	}
	void Cell1Scene::GoOutTheDoor()
	{
		if (m_dialogIndex > 11 &&
			m_dialogTexture == nullptr)
		{
			m_brightness -= 0.001f;
			m_timeCounter = 0.0f;
		}
	}
	void Cell1Scene::FirstUpdateLoop(float deltaTime)
	{
		m_timeCounter += deltaTime;
		float speed = 1.0f * deltaTime;

		m_graphics.UpdateCamera();
		m_graphics.SetDefaultVertexShader();
		m_pixelShader->SetShader();
		m_graphics.WriteLightData(
			m_lightColor,
			float3(m_oldman.position.x, 1.0f, 0.3f),
			0.25f);
		m_graphics.ClearScreen();

		m_roomTexture->SetTexture();
		Positionf entity;
		m_graphics.WriteEntityData(entity);
		m_roomModel->Render();

		if (m_doorTexture)
		{
			m_doorTexture->SetTexture();
			m_graphics.WriteEntityData(m_door);
			m_imageModel->Render();
		}

		m_oldmanTexture->SetTexture();
		m_graphics.WriteEntityData(m_oldman);
		m_imageModel->Render();

		UpdateDialog();

		if (m_brightness < 1.0f)
		{
			m_brightness -= deltaTime * 0.6f;
			if (m_brightness < 0.0f)
			{
				m_timeCounter = 0.0f;
				SAFE_DELETE(m_doorTexture);
			}
		}
		float effectBuffer[8] = { max(m_brightness, 0.0f), 180.0f, 100.0f };
		m_graphics.WriteEffectBuffer(effectBuffer);
		m_graphics.Present();
	}
	void Cell1Scene::SecondUpdateLoop(float deltaTime)
	{
		m_graphics.ClearScreen();
		if (m_dialogTexture == nullptr)
		{
			if (m_timeCounter > 7.0f)
				m_dialogTexture = m_graphics.CreateTextTexture(L"There should be something somewhere...");
			else
				m_timeCounter += deltaTime;
		}
		if (m_dialogTexture)
			m_graphics.RenderDialog(m_dialogTexture);
		float effectBuffer[8] = { 1.0f };
		m_graphics.WriteEffectBuffer(effectBuffer);
		m_graphics.Present();
	}
	Cell1Scene::Cell1Scene(Graphics& graphics) :
		m_graphics(graphics),
		m_roomModel(nullptr),
		m_roomTexture(nullptr),
		m_imageModel(nullptr),
		m_oldmanTexture(nullptr),
		m_oldman(),
		m_doorTexture(nullptr),
		m_door(),
		m_pixelShader(nullptr),
		m_mousex(0), m_mousey(0),
		m_clickable(false),
		m_userName(),
		m_dialogTexture(nullptr),
		m_dialogIndex(0),
		m_dialogConditionMet(false),
		m_timeCounter(0.0f),
		m_brightness(1.0f),
		m_lightColor(1.0f, 0.8f, 0.5f, 1.0f)
	{
		m_roomModel = graphics.LoadModel(L"Media/cell1.bmd");
		m_roomTexture = graphics.LoadTexture(L"Media/cell1.png");
		m_imageModel = graphics.LoadModel(L"Media/oldman.bmd");
		m_oldmanTexture = graphics.LoadTexture(L"Media/oldman.png");
		m_pixelShader = graphics.LoadPixelShader(L"other world/redgreenblue.eye");
		m_graphics.getCamera().position = float3(0.0f, 1.2f, -4.1f);
		m_graphics.getCamera().rotation = float3();
		graphics.SetCameraProjective((float)PI * 0.25f, 1.8f);
		graphics.UseEffect(L"Media/no.effect", 180, 100);

		m_oldman.position.x = 1.6f;
		m_oldman.position.z = 0.8f;
		m_oldman.scale = 1.2f;

		m_door.position.x = 0.0f;
		m_door.position.z = 0.95f;
		m_door.scale = 2.0f;

		wchar_t userName[64];
		DWORD length = 64;
		GetUserName(userName, &length);
		m_userName = userName;
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	Cell1Scene::~Cell1Scene()
	{
		SAFE_DELETE(m_roomModel);
		SAFE_DELETE(m_roomTexture);
		SAFE_DELETE(m_imageModel);
		SAFE_DELETE(m_oldmanTexture);
		SAFE_DELETE(m_doorTexture);
		SAFE_DELETE(m_pixelShader);
		SAFE_DELETE(m_dialogTexture);
	}
	void Cell1Scene::Update(double deltaTime)
	{
		SetCursorIcon();
		if (m_brightness < 0.0f)
			SecondUpdateLoop(deltaTime);
		else
			FirstUpdateLoop(deltaTime);
	}
	void Cell1Scene::MouseMove(int x, int y, int dx, int dy)
	{
		m_mousex = x;
		m_mousey = y;
	}
	void Cell1Scene::MouseLButtonDown(int x, int y)
	{
	}
	void Cell1Scene::MouseLButtonUp(int x, int y)
	{
		if (x > 370 && x < 525 && y > 170 && y < 390)
			GoOutTheDoor();
		if (x > 574 && x < 720 && y > 254 && y < 400)
			if (m_dialogIndex == 0)
			{
				m_dialogConditionMet = true;
				m_dialogClicked = true;
			}
		if (OnDialogBox(x, y))
			DialogClick();
	}
	void Cell1Scene::MouseRButtonDown(int x, int y)
	{
	}
	void Cell1Scene::MouseRButtonUp(int x, int y)
	{
		if (x > 370 && x < 525 && y > 170 && y < 390)
			GoOutTheDoor();
		if (x > 574 && x < 720 && y > 254 && y < 400)
			if (m_dialogIndex == 0)
			{
				m_dialogConditionMet = true;
				m_dialogClicked = true;
			}
		if (OnDialogBox(x, y))
			DialogClick();
	}
	void Cell1Scene::KeyDown(int keyCode)
	{
	}
	void Cell1Scene::KeyUp(int keyCode)
	{
		switch (keyCode)
		{
		case VK_RETURN:
		case VK_SPACE:
			DialogClick();
			break;
		}
	}
	bool Cell1Scene::FileDrop(const wchar_t* filename)
	{
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

		if (extension == L".eye")
		{
			try
			{
				PixelShader* pixelShader = m_graphics.LoadPixelShader(filename);
				SAFE_DELETE(m_pixelShader);
				m_pixelShader = pixelShader;
			}
			catch (...) {}
		}
		if (extension == L".light")
		{
			std::ifstream infile(filename);
			if (infile.is_open())
			{
				std::string buffer;
				int red, green, blue;
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
		if (extension == L".png" && name == L"door")
		{
			SAFE_DELETE(m_doorTexture);
			m_doorTexture = m_graphics.LoadTexture(filename);
			if (m_dialogIndex <= 7)
			{
				m_dialogIndex = 7;
				m_timeCounter = 0.0f;
			}
			return true;
		}

		return false;
	}
}