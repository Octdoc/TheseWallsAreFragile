#pragma once

#include "TheseWallsAreFragile/scene.h"
#include "image.h"
#include <string>

namespace twaf
{
	class EscapeStage
	{
	protected:
		static Texture* s_wallTextures[8];
		static Model* s_wallModel;
		static Model* s_pillarModel;
		static Model* s_ememyModel;

		Graphics& m_graphics;

		int m_wallTextureIndex;
		Positionf m_wall;

		Texture* m_enemyTexture;
		Positionf m_enemy;
		Texture* m_waterTexture;
		Positionf m_water;
		float m_timecounter;
		bool m_solved;
		bool m_enemyRuns;
		bool m_stageLoaded;

		std::wstring m_enterDialog;
		std::wstring m_issueDialog;
		unsigned m_stageIndex;

	public:
		float3 visibleColors;
		float3 lightColor;

	public:
		EscapeStage(Graphics& graphics, unsigned index);
		virtual ~EscapeStage();
		static void LoadResources(Graphics& graphics);
		static void FreeResources();

		virtual void Update(float deltaTime);
		void Render(float4 lightColor, float ambient);

		virtual void FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension);

		inline void LoadingDone() { m_stageLoaded = true; }
		inline const wchar_t* EnterDialog() { return m_enterDialog.c_str(); }
		inline const wchar_t* IssueDialog() { return m_issueDialog.c_str(); }
		inline bool Solved() { return m_stageLoaded && m_solved; }
	};

	class EscapeStage0 :public EscapeStage
	{
	public:
		EscapeStage0(Graphics& graphics);
	};

	class EscapeStage1 :public EscapeStage
	{
	public:
		EscapeStage1(Graphics& graphics);
	};

	class EscapeStage2 :public EscapeStage
	{
	public:
		EscapeStage2(Graphics& graphics);
		virtual void FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension) override;
	};

	class EscapeStage3 :public EscapeStage
	{
	public:
		EscapeStage3(Graphics& graphics);
		virtual void Update(float deltaTime) override;
	};

	class EscapeStage4 :public EscapeStage
	{
	public:
		EscapeStage4(Graphics& graphics);
		virtual void Update(float deltaTime) override;
	};

	class EscapeStage5 :public EscapeStage
	{
		bool m_canDrink;

	public:
		EscapeStage5(Graphics& graphics);
		virtual void Update(float deltaTime) override;
		virtual void FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension) override;
	};

	class EscapeStage6 :public EscapeStage
	{
	public:
		EscapeStage6(Graphics& graphics);
		virtual void FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension) override;
	};

	class EscapeStage7 :public EscapeStage
	{
		bool m_canDrink;

	public:
		EscapeStage7(Graphics& graphics);
		virtual void Update(float deltaTime) override;
		virtual void FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension) override;
	};

	class EscapeStage8 :public EscapeStage
	{
		bool m_canScare;

	public:
		EscapeStage8(Graphics& graphics);
		virtual void Update(float deltaTime) override;
		virtual void FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension) override;
	};

	class EscapeStage9 :public EscapeStage
	{
	public:
		EscapeStage9(Graphics& graphics);
		virtual void Update(float deltaTime) override;
		virtual void FileDrop(const wchar_t* filename, std::wstring& name, std::wstring& extension) override;
	};
}