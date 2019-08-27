#include "escapestage.h"

namespace twaf
{
	Texture* EscapeStage::s_wallTextures[8]{};
	Model* EscapeStage::s_wallModel = nullptr;
	Model* EscapeStage::s_pillarModel = nullptr;
	Model* EscapeStage::s_ememyModel = nullptr;

	EscapeStage::EscapeStage(Graphics& graphics, unsigned index) :
		m_graphics(graphics),
		m_wallTextureIndex(7),
		m_wall(),
		m_enemyTexture(nullptr),
		m_enemy(),
		m_waterTexture(nullptr),
		m_water(),
		m_timecounter(0.0f),
		m_solved(false),
		m_enemyRuns(false),
		m_issueDialog(),
		m_stageIndex(index)
	{
		m_wall.position.x = (float)index * 2.5f;
		m_enemy.position.x = m_wall.position.x;
		m_enemy.position.z = -0.62f;
		m_enemy.scale = 0.7f;
		m_water.scale = 0.2f;
	}
	EscapeStage::~EscapeStage()
	{
		SAFE_DELETE(m_enemyTexture);
	}
	void EscapeStage::LoadResources(Graphics& graphics)
	{
		Image wallImage(L"Media/corridor.png");
		for (int i = 0; i < 7; i++)
		{
			Image img(wallImage);
			img.ForEach([i](Color c)->Color {
				if ((i & 0x1) == 0) c.blue = 0;
				if ((i & 0x2) == 0) c.green = 0;
				if ((i & 0x4) == 0) c.red = 0;
				return c;
				});
			s_wallTextures[i] = graphics.CreateTexture(img.Pixels(), img.Width(), img.Height());
		}
		s_wallTextures[7] = graphics.CreateTexture(wallImage.Pixels(), wallImage.Width(), wallImage.Height());

		s_wallModel = graphics.LoadModel(L"Media/wall.bmd");
		s_pillarModel = graphics.LoadModel(L"Media/pillar.bmd");
		s_ememyModel = graphics.LoadModel(L"Media/oldman.bmd");
	}
	void EscapeStage::FreeResources()
	{
		for (int i = 0; i < 8; i++)
			SAFE_DELETE(s_wallTextures[i]);
		SAFE_DELETE(s_wallModel);
		SAFE_DELETE(s_pillarModel);
		SAFE_DELETE(s_ememyModel);
	}
	void EscapeStage::Update(float deltaTime)
	{
		m_timecounter += deltaTime;
		if (m_timecounter > 0.0f && m_enemyRuns && m_enemyTexture != nullptr)
		{
			m_enemy.position.x += deltaTime * 2.0f;
			if (m_enemy.position.x > m_wall.position.x + 2.5f)
			{
				m_enemyRuns = false;
				SAFE_DELETE(m_enemyTexture);
				SAFE_DELETE(m_waterTexture);
				m_solved = true;
			}
		}
	}
	void EscapeStage::Render(float4 lightColor, float ambient)
	{
		m_graphics.WriteLightData(
			lightColor,
			float3(m_wall.position.x, 1.5f, -0.3f),
			ambient);
		m_graphics.WriteEntityData(m_wall);
		s_wallTextures[7]->SetTexture();
		s_pillarModel->Render();
		s_wallTextures[m_wallTextureIndex]->SetTexture();
		s_wallModel->Render();
		if (m_enemyTexture)
		{
			m_graphics.WriteLightData(
				lightColor,
				float3(m_wall.position.x, 0.5f, -1.0f),
				ambient);
			m_enemyTexture->SetTexture();
			m_graphics.WriteEntityData(m_enemy);
			s_ememyModel->Render();

			if (m_waterTexture)
			{
				m_waterTexture->SetTexture();
				m_water.position = m_enemy.position + float3(0.5f, 0.2f, -0.1f);
				m_graphics.WriteEntityData(m_water);
				s_ememyModel->Render();
			}
		}
	}
	void EscapeStage::FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension) {}

#pragma region Stage0
	EscapeStage0::EscapeStage0(Graphics& graphics) :
		EscapeStage(graphics, 0)
	{
		m_solved = true;
	}
#pragma endregion

#pragma region Stage1
	EscapeStage1::EscapeStage1(Graphics& graphics) :
		EscapeStage(graphics, 1)
	{
		m_enterDialog = L"I will need your help from the other\nworld.";
		m_solved = true;
	}
#pragma endregion

#pragma region Stage2
	EscapeStage2::EscapeStage2(Graphics& graphics) :
		EscapeStage(graphics, 2)
	{
		m_enterDialog = L"I cannot go farther while he is there.";
		m_issueDialog = L"He looks thirsty. Some water may lure\nhim away.";
		Image image(L"Media/enemy_thirsty.png");
		m_enemyTexture = m_graphics.CreateTexture(image.Pixels(), image.Width(), image.Height());
	}
	void EscapeStage2::FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension)
	{
		if (name == L"water" && extension == L".png")
		{
			m_waterTexture = m_graphics.LoadTexture(filename);
			m_timecounter = -0.5f;
			m_enemyRuns = true;
		}
	}
#pragma endregion

#pragma region Stage3
	EscapeStage3::EscapeStage3(Graphics& graphics) :
		EscapeStage(graphics, 3)
	{
		m_enterDialog = L"I do not think there is a way to make him move.";
		m_issueDialog = L"Maybe if he could not be seen.";
		Image image(L"Media/enemy_angry.png");
		image.ForEach([](Color c)->Color {
			c.green = 0;
			c.blue = 0;
			return c;
			});
		m_enemyTexture = m_graphics.CreateTexture(image.Pixels(), image.Width(), image.Height());
	}
	void EscapeStage3::Update(float deltaTime)
	{
		EscapeStage::Update(deltaTime);
		m_solved = false;
		if (visibleColors.x == 0.0f)
			m_solved = true;
		if (lightColor.x == 0.0f)
			if (visibleColors.y == 0.0f || visibleColors.z == 0.0f)
				m_solved = true;
	}
#pragma endregion

#pragma region Stage4
	EscapeStage4::EscapeStage4(Graphics& graphics) :
		EscapeStage(graphics, 4)
	{
		m_issueDialog = L"There is nothing. I cannot cross.";
		m_wallTextureIndex = 4;
		m_solved = false;
	}
	void EscapeStage4::Update(float deltaTime)
	{
		EscapeStage::Update(deltaTime);
		m_solved = true;
		if (visibleColors.x == 0.0f)
			m_solved = false;
		if (lightColor.x == 0.0f)
			if (visibleColors.y == 0.0f || visibleColors.z == 0.0f)
				m_solved = false;
	}
#pragma endregion

#pragma region Stage5
	EscapeStage5::EscapeStage5(Graphics& graphics) :
		EscapeStage(graphics, 5)
	{
		//m_enterDialog = L"I wonder what happened to him.";
		m_issueDialog = L"He might need something.";
		Image angry(L"Media/enemy_angry.png");
		Image thirsty(L"Media/enemy_thirsty.png");
		unsigned w = min(angry.Width(), thirsty.Width());
		unsigned h = min(angry.Height(), thirsty.Height());
		for (unsigned y = 0; y < h; y++)
			for (unsigned x = 0; x < w; x++)
				thirsty(x, y).red = angry(x, y).red;
		m_enemyTexture = m_graphics.CreateTexture(thirsty.Pixels(), thirsty.Width(), thirsty.Height());
		m_solved = false;
		m_canDrink = false;
	}
	void EscapeStage5::Update(float deltaTime)
	{
		EscapeStage::Update(deltaTime);
		m_canDrink = false;
		if (visibleColors.x == 0.0f)
			m_canDrink = true;
		if (lightColor.x == 0.0f)
			if (visibleColors.y == 0.0f || visibleColors.z == 0.0f)
				m_canDrink = true;
	}
	void EscapeStage5::FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension)
	{
		if (m_canDrink && name == L"water" && extension == L".png")
		{
			m_waterTexture = m_graphics.LoadTexture(filename);
			m_timecounter = -0.5f;
			m_enemyRuns = true;
		}
	}
#pragma endregion

#pragma region Stage6
	EscapeStage6::EscapeStage6(Graphics& graphics) :
		EscapeStage(graphics, 6)
	{
		m_issueDialog = L"He looks easy to scare off.";
		m_enemyTexture = m_graphics.LoadTexture(L"Media/enemy_scared.png");
	}
	void EscapeStage6::FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension)
	{
		if (name == L"yell" && extension == L".wav")
		{
			m_enemyRuns = true;
			m_timecounter = -0.1f;
		}
	}
#pragma endregion

#pragma region Stage7
	EscapeStage7::EscapeStage7(Graphics& graphics) :
		EscapeStage(graphics, 7)
	{
		m_wallTextureIndex = 4;
		m_issueDialog = L"I need clear path.";
		Image image(L"Media/enemy_thirsty.png");
		image.ForEach([](Color c)->Color {
			c.green = 0;
			c.blue = 0;
			return c;
			});
		m_enemyTexture = m_graphics.CreateTexture(image.Pixels(), image.Width(), image.Height());
		m_canDrink = false;
	}
	void EscapeStage7::Update(float deltaTime)
	{
		EscapeStage::Update(deltaTime);
		m_canDrink = true;
		if (visibleColors.x == 0.0f)
			m_canDrink = false;
		if (lightColor.x == 0.0f)
			if (visibleColors.y == 0.0f || visibleColors.z == 0.0f)
				m_canDrink = false;
	}
	void EscapeStage7::FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension)
	{
		if (m_canDrink && name == L"water" && extension == L".png")
		{
			m_waterTexture = m_graphics.LoadTexture(filename);
			m_timecounter = -0.5f;
			m_enemyRuns = true;
		}
	}
#pragma endregion

#pragma region Stage8
	EscapeStage8::EscapeStage8(Graphics& graphics) :
		EscapeStage(graphics, 8)
	{
		m_issueDialog = L"I cannot go past him.";
		Image angry(L"Media/enemy_angry.png");
		Image scared(L"Media/enemy_scared.png");
		unsigned w = min(angry.Width(), scared.Width());
		unsigned h = min(angry.Height(), scared.Height());
		for (unsigned y = 0; y < h; y++)
			for (unsigned x = 0; x < w; x++)
				angry(x, y).green = scared(x, y).green;
		m_enemyTexture = m_graphics.CreateTexture(angry.Pixels(), angry.Width(), angry.Height());
		m_canScare = false;
		m_solved = false;
	}
	void EscapeStage8::Update(float deltaTime)
	{
		EscapeStage::Update(deltaTime);
		m_canScare = visibleColors.y > 0.0f && lightColor.y > 0.0f && lightColor.x == 0.0f && lightColor.z == 0.0f;
	}
	void EscapeStage8::FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension)
	{
		if (m_canScare && name == L"yell" && extension == L".wav")
		{
			m_enemyRuns = true;
			m_timecounter = -0.1f;
		}
	}
#pragma endregion

#pragma region Stage9
	EscapeStage9::EscapeStage9(Graphics& graphics) :
		EscapeStage(graphics, 9)
	{
		m_enterDialog = L"The corridor ends here.";
		m_issueDialog = L"There should be a hidden passage\nsomewhere.";
	}
	void EscapeStage9::Update(float deltaTime)
	{
		EscapeStage::Update(deltaTime);
		if (m_timecounter > 1.5f && m_timecounter < 3.0f)
		{
			m_graphics.getCamera().position.x += deltaTime * 0.5f;
		}
	}
	void EscapeStage9::FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension)
	{
		if (m_timecounter > 3.2f && name == L"door" && extension == L".png")
		{
			m_enemy.position.x = m_wall.position.x;
			m_enemy.position.y = m_wall.position.y;
			m_enemy.position.z = m_wall.position.z - 0.01f;
			m_enemy.scale = 1.0f;
			m_enemyTexture = m_graphics.LoadTexture(filename);
			m_solved = true;
		}
	}
#pragma endregion
}