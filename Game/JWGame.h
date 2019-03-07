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
		JWGame() {};
		~JWGame() {};

		void Create(const WSTRING& GameName, int Width, int Height);

		void SetRenderFunction(const PF_RENDER pfRender) noexcept;
		void SetKeyboardFunction(const PF_KEYBOARD pfKeyboard) noexcept;

		void ToggleBoundingBox() noexcept;
		void LoadMap(const WSTRING& FileName) noexcept;

		void Run() noexcept;
		void Shutdown() noexcept;

		void SetBackgroundTexture(const WSTRING& TextureFN) noexcept;

		auto SpriteCreate(const WSTRING& TextureFN, const POINT& UnitSize, int numCols, int numRows, float Scale = 1.0f) noexcept->JWLife*;
		void SpriteWalk(EAnimationDirection Direction) noexcept;
		void SpriteJump() noexcept;
		void SpriteSetAnimation(EAnimationID AnimationID) noexcept;

		auto SpawnMonster(const WSTRING& MonsterName, const D3DXVECTOR2& GlobalPosition) noexcept->JWMonster*;
		auto SpawnEffect(int EffectID, int Damage) noexcept->JWEffect*;

		auto GetTextObject() const noexcept->JWText*;
		auto GetSpriteObject() const noexcept->JWLife*;
		auto GetMonsterManagerObject() const noexcept->JWMonsterManager*;
		auto GetEffectManagerObject() const noexcept->JWEffect*;

	private:
		void MainLoop() noexcept;
		void DetectInput() noexcept;

		void DrawBackground() noexcept;
		void DrawMap() noexcept;
		void DrawMonsters() noexcept;
		void DrawSprite() noexcept;
		void DrawEffects() noexcept;

		void DrawAllBase() noexcept;

	private:
		static constexpr int ANIM_TICK{ 70 };
		static constexpr int KEY_TOGGLE_INTERVAL{ 15 };
		static constexpr int WINDOW_X{ 50 };
		static constexpr int WINDOW_Y{ 50 };

		bool m_Keys[NUM_KEYS]{};

		ULONGLONG m_TimerStart{};
		ULONGLONG m_TimerSec{};
		ULONGLONG m_TimerAnim{};
		int m_FPS{};
		int m_KeyPressCount{};
		int m_KeyToggleCount{};
		bool m_bDrawBoundingBoxes{ false };

		PF_RENDER m_pfRender{ nullptr };
		PF_KEYBOARD m_pfKeyboard{ nullptr };

		UNIQUE_PTR<JWWindow> m_Window;
		UNIQUE_PTR<JWInput> m_Input;
		UNIQUE_PTR<JWBackground> m_Background;
		UNIQUE_PTR<JWMap> m_Map;
		UNIQUE_PTR<JWLife> m_Sprite;
		UNIQUE_PTR<JWMonsterManager> m_MonsterManager;
		UNIQUE_PTR<JWEffect> m_EffectManager;
		UNIQUE_PTR<JWText> m_Text;

		WSTRING m_BaseDir;
		HWND m_hMainWnd{ nullptr };
		MSG m_MSG{};
	};
};