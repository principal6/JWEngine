#pragma once

#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWInput.h"
#include "../CoreBase/JWText.h"
#include "../Core/JWBackground.h"
#include "../Core/JWLife.h"
#include "../Core/JWMonsters.h"
#include "../Core/JWEffect.h"
#include "../Core/JWMap.h"

namespace JWENGINE
{
	using PF_RENDER = void(*)();
	using PF_KEYBOARD = void(*)(DWORD DIK_KeyCode);

	static const wchar_t PROJECT_FOLDER[]{ L"\\Game" };

	class JWGame final
	{
	public:
		JWGame();
		~JWGame() {};

		auto JWGame::Create(int Width, int Height)->EError;
		void JWGame::SetRenderFunction(PF_RENDER pfRender);
		void JWGame::SetKeyboardFunction(PF_KEYBOARD pfKeyboard);
		void JWGame::ToggleBoundingBox();
		auto JWGame::LoadMap(WSTRING FileName)->EError;

		void JWGame::Run();
		void JWGame::Shutdown();

		void JWGame::SetBackground(WSTRING TextureFN);

		auto JWGame::SpriteCreate(WSTRING TextureFN, POINT UnitSize, int numCols, int numRows, float Scale = 1.0f)->JWLife*;
		void JWGame::SpriteWalk(EAnimationDirection Direction);
		void JWGame::SpriteJump();
		void JWGame::SpriteSetAnimation(EAnimationID AnimationID);

		auto JWGame::SpawnMonster(WSTRING MonsterName, D3DXVECTOR2 GlobalPosition)->JWMonster*;
		auto JWGame::SpawnEffect(int EffectID, int Damage)->JWEffect*;

		auto JWGame::GetTextObject()->JWText*;
		auto JWGame::GetSpriteObject()->JWLife*;
		auto JWGame::GetMonsterManagerObject()->JWMonsterManager*;
		auto JWGame::GetEffectManagerObject()->JWEffect*;

	private:
		void JWGame::Destroy();

		void JWGame::MainLoop();
		void JWGame::DetectInput();

		void JWGame::DrawBackground();
		void JWGame::DrawMap();
		void JWGame::DrawMonsters();
		void JWGame::DrawSprite();
		void JWGame::DrawEffects();
		void JWGame::DrawFont();

		void JWGame::DrawAllBase();

	private:
		static const int ANIM_TICK = 70;
		static const int KEY_TOGGLE_INTERVAL = 15;
		static const int WINDOW_X = 50;
		static const int WINDOW_Y = 50;

		bool m_Keys[NUM_KEYS];

		ULONGLONG m_TimerStart;
		ULONGLONG m_TimerSec;
		ULONGLONG m_TimerAnim;
		int m_FPS;
		int m_KeyPressCount;
		int m_KeyToggleCount;
		bool m_bDrawBoundingBoxes;

		PF_RENDER m_pfRender;
		PF_KEYBOARD m_pfKeyboard;

		UNIQUE_PTR<JWWindow> m_Window;
		UNIQUE_PTR<JWInput> m_Input;
		UNIQUE_PTR<JWBackground> m_Background;
		UNIQUE_PTR<JWMap> m_Map;
		UNIQUE_PTR<JWLife> m_Sprite;
		UNIQUE_PTR<JWMonsterManager> m_MonsterManager;
		UNIQUE_PTR<JWEffect> m_EffectManager;
		UNIQUE_PTR<JWText> m_Text;

		WSTRING m_BaseDir;
		HWND m_hMainWnd;
		MSG m_MSG;
	};
};