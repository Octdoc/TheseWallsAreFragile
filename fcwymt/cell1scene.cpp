#include "cell1scene.h"

namespace twaf
{
	bool Cell1Scene::OnDialogBox(int x, int y)
	{
		return x > 65 && x < 834 && y > 339 && y < 484;
	}
	bool Cell1Scene::OnOldman(int x, int y)
	{
		return x > 574 && x < 720 && y > 254 && y < 400;
	}
	bool Cell1Scene::OnDoor(int x, int y)
	{
		return x > 370 && x < 525 && y > 170 && y < 390;
	}
	bool Cell1Scene::CanClickOldman()
	{
		return m_step == 0 ||
			m_step == 5 ||
			m_step == 9 ||
			m_step == 11;
	}
	bool Cell1Scene::CanClickDialog()
	{
		return m_step == 1 ||
			m_step == 2 ||
			m_step == 3 ||
			m_step == 4 ||
			m_step == 6 ||
			m_step == 7 ||
			m_step == 8 ||
			m_step == 12 ||
			m_step == 13 ||
			m_step == 14 ||
			m_step == 15 ||
			m_step == 16;
	}
	bool Cell1Scene::CanClickDoor()
	{
		return m_step == 11 ||
			m_step == 17;
	}
	void Cell1Scene::SetCursorIcon()
	{
		bool clickable = false;
		if (CanClickOldman() && OnOldman(m_mousex, m_mousey))
			clickable = true;
		else if (CanClickDoor() && OnDoor(m_mousex, m_mousey))
			clickable = true;
		else if (CanClickDialog() && OnDialogBox(m_mousex, m_mousey))
			clickable = true;

		if (m_clickable != clickable)
		{
			m_clickable = clickable;
			SetCursor(LoadCursor(NULL, m_clickable ? IDC_HAND : IDC_ARROW));
		}
	}
	void Cell1Scene::ClickEvent(int x, int y)
	{
		if (CanClickOldman() && OnOldman(m_mousex, m_mousey))
			ProgressStory(m_step + 1);
		else if (CanClickDoor() && OnDoor(m_mousex, m_mousey))
			ProgressStory(m_step + 1);
		else if (CanClickDialog() && OnDialogBox(m_mousex, m_mousey))
			ProgressStory(m_step + 1);
	}
	void Cell1Scene::NewDialogbox(const wchar_t* text)
	{
		SAFE_DELETE(m_dialogTexture);
		m_dialogTexture = m_graphics.CreateTextTexture(text);
	}
	void Cell1Scene::ProgressStory(int newStep)
	{
		m_timeCounter = 0.0f;
		m_step = newStep;
		switch (m_step)
		{
		case 1:
			NewDialogbox((L"Welcome. You are " + m_userName + L", yes?").c_str());
			break;
		case 2:
			NewDialogbox(L"You are the one with the otherworldly\npowers.");
			break;
		case 3:
			NewDialogbox(L"I was wondering if you would come.");
			break;
		case 4:
			NewDialogbox(L"I will be here whenever you need\nguidance.");
			break;
		case 5:
			SAFE_DELETE(m_dialogTexture);
			break;
		case 6:
			NewDialogbox(L"Are you not bored?");
			break;
		case 7:
			NewDialogbox(L"I know a way out. There was a door.png\nnext to where you came in.");
			break;
		case 8:
			NewDialogbox(L"You have the power to install it on the\nwall.");
			break;
		case 9:
			SAFE_DELETE(m_dialogTexture);
			break;
		case 10:
			NewDialogbox(L"Simply drop the file on the window.");
			break;
		case 11:
			SAFE_DELETE(m_dialogTexture);
			break;
		case 12:
			NewDialogbox(L"Before you go. You need to know one\nthing.");
			break;
		case 13:
			NewDialogbox(L"This world is incomplete.");
			break;
		case 14:
			NewDialogbox(L"I cannot escape because nothing exists\noutside of this prison.");
			break;
		case 15:
			NewDialogbox(L"You however have eyes not of this world\nand arms passing things over.");
			break;
		case 16:
			NewDialogbox(L"You can make it out. And when you do,\nI ask you to rescue me too.");
			break;
		case 17:
			SAFE_DELETE(m_dialogTexture);
			break;
		case 18:
			m_brightness -= 0.001f;
			break;
		case 19:
			NewDialogbox(L"There should be something somewhere...");
			break;
		}
	}
	void Cell1Scene::TimedProgressionCheck()
	{
		switch (m_step)
		{
		case 5:
			if (m_timeCounter > 3.0f)
				ProgressStory(6);
			break;
		case 9:
			if (m_timeCounter > 5.0f)
				ProgressStory(10);
			break;
		case 11:
			if (m_timeCounter > 1.5f)
				ProgressStory(12);
			break;
		case 18:
			if (m_timeCounter > 4.0f)
				ProgressStory(19);
			break;
		}
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
		m_step(0),
		m_userName(),
		m_dialogTexture(nullptr),
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
		m_timeCounter += deltaTime;
		TimedProgressionCheck();

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

		if (m_dialogTexture)
			m_graphics.RenderDialog(m_dialogTexture, m_oldmanTexture, float2(830.0f, 430.0f), float2(64.0f));

		if (m_brightness < 1.0f)
			m_brightness -= deltaTime * 0.6f;

		float effectBuffer[8] = { max(m_brightness, 0.0f), 180.0f, 100.0f };
		m_graphics.WriteEffectBuffer(effectBuffer);
		m_graphics.Present();
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
		ClickEvent(x, y);
	}
	void Cell1Scene::MouseRButtonDown(int x, int y)
	{
	}
	void Cell1Scene::MouseRButtonUp(int x, int y)
	{
		ClickEvent(x, y);
	}
	void Cell1Scene::KeyDown(int keyCode)
	{
	}
	void Cell1Scene::KeyUp(int keyCode)
	{
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
				int red = 255, green = 255, blue = 255;
				do
				{
					infile >> buffer;
					if (buffer == "red:")
						infile >> red;
					else if (buffer == "green:")
						infile >> green;
					else if (buffer == "blue:")
						infile >> blue;
				} while (!infile.eof());
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
			if (m_step < 11)
				ProgressStory(11);
			return true;
		}

		return false;
	}
	bool Cell1Scene::AllowExit()
	{
		int ret = MessageBox(m_graphics.getHWND(), L"Are you sure you want to leave the old man imprisoned?", L"Return to the real world alone?", MB_YESNO);
		if (ret == IDYES)
			return true;
		return false;
	}
}