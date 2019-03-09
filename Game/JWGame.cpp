#include "JWGame.h"

using namespace JWENGINE;

void JWGame::Create(const WSTRING& GameName, int Width, int Height)
{
	// Set base directory
	wchar_t tempDir[MAX_FILE_LEN]{};
	GetCurrentDirectoryW(MAX_FILE_LEN, tempDir);
	m_BaseDir = tempDir;
	m_BaseDir = m_BaseDir.substr(0, m_BaseDir.find(PROJECT_FOLDER));

	std::wcout << m_BaseDir.c_str() << std::endl;

	// Create base (window and initialize Direct3D9)
	SWindowCreationData game_window_data = SWindowCreationData(GameName, WINDOW_X, WINDOW_Y, Width, Height, DEFAULT_COLOR_BLACK);

	m_Window = MAKE_UNIQUE(JWWindow)();
	m_Window->CreateGameWindow(game_window_data);

	// Set main window handle
	m_hMainWnd = m_Window->GethWnd();

	// Create input device
	m_Input = MAKE_UNIQUE(JWInput)();
	try
	{
		m_Input->Create(m_Window->GethWnd(), m_Window->GethInstance());
	}
	catch (creation_failed& e)
	{
		std::cout << e.what() << std::endl;

		abort();
	}

	// Create font object
	m_Text = MAKE_UNIQUE(JWText)();
	m_Text->CreateInstantText(*m_Window.get(), m_BaseDir);
	
	// Create image object
	m_Background = MAKE_UNIQUE(JWBackground)();
	m_Background->Create(*m_Window.get(), m_BaseDir);
	
	// Create map object
	m_Map = MAKE_UNIQUE(JWMap)();
	m_Map->Create(*m_Window.get(), m_BaseDir);

	// Create sprite object
	m_Sprite = MAKE_UNIQUE(JWLife)();
	m_Sprite->Create(*m_Window.get(), m_BaseDir, *m_Map.get());

	// Create monster manager object
	m_MonsterManager = MAKE_UNIQUE(JWMonsterManager)();
	m_MonsterManager->Create(*m_Window.get(), m_BaseDir, *m_Map.get());

	// Create effect manager object
	m_EffectManager = MAKE_UNIQUE(JWEffect)();
	m_EffectManager->Create(*m_Window.get(), m_BaseDir, *m_Map.get());
}

void JWGame::SetRenderFunction(const PF_RENDER pfRender) noexcept
{
	m_pfRender = pfRender;
}

void JWGame::SetKeyboardFunction(const PF_KEYBOARD pfKeyboard) noexcept
{
	m_pfKeyboard = pfKeyboard;
}

void JWGame::ToggleBoundingBox() noexcept
{
	if (m_KeyToggleCount == 0)
	{
		m_bDrawBoundingBoxes = !m_bDrawBoundingBoxes;
		m_KeyToggleCount++;
	}
}

void JWGame::LoadMap(const WSTRING& FileName) noexcept
{
	if (m_Map)
	{
		m_Map->LoadMap(FileName);
	}
}

void JWGame::Run() noexcept
{
	m_TimerStart = GetTickCount64();

	while (m_MSG.message != WM_QUIT)
	{
		if (PeekMessage(&m_MSG, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&m_MSG);
			DispatchMessage(&m_MSG);
		}
		else
		{
			MainLoop();
		}
	}
}

void JWGame::Shutdown() noexcept
{
	if (m_hMainWnd)
	{
		DestroyWindow(m_hMainWnd);
	}
}

PRIVATE void JWGame::MainLoop() noexcept
{
	DetectInput();

	m_Window->UpdateWindowInputState(m_MSG);

	// Timer for calculating FPS
	if (GetTickCount64() >= m_TimerSec + 1000)
	{
		m_TimerSec = GetTickCount64();
		ULONGLONG tTime = (m_TimerSec - m_TimerStart) / 1000;
		std::cout << "Time: " << tTime << " // FPS: " << m_FPS << std::endl;
		m_FPS = 0;
	}

	// Timer for steady animations
	if (GetTickCount64() >= m_TimerAnim + ANIM_TICK)
	{
		m_TimerAnim = GetTickCount64();
		m_MonsterManager->Animate();
		m_Sprite->Animate();
	}

	if (m_KeyToggleCount > 0)
	{
		m_KeyToggleCount++;

		if (m_KeyToggleCount > KEY_TOGGLE_INTERVAL)
			m_KeyToggleCount = 0;
	}

	m_Window->BeginRender();

	// Apply gravity
	m_MonsterManager->Gravitate();
	m_Sprite->Gravitate();

	if (m_Map)
	{
		D3DXVECTOR2 mapOffset = m_Sprite->GetOffsetForMapMove();
		m_Map->SetGlobalPosition(-mapOffset);
	}

	if ((m_EffectManager) && (m_MonsterManager))
	{
		m_EffectManager->CheckCollisionWithMonsters(m_MonsterManager.get());
	}

	DrawAllBase();

	if (m_pfRender)
		m_pfRender();

	m_Window->EndRender();
	m_FPS++;

	/** Slow down the process for debugging */
	//Sleep(100);
}

PRIVATE void JWGame::DetectInput() noexcept
{
	m_Input->GetAllKeyState(m_Keys);

	for (int i = 0; i < NUM_KEYS; i++)
	{
		if (m_Keys[i] == true)
		{
			if (m_pfKeyboard)
				m_pfKeyboard(i);
		}
	}
}

void JWGame::SetBackgroundTexture(const WSTRING& TextureFN) noexcept
{
	assert(m_Background);
	m_Background->SetTexture(TextureFN);
}

auto JWGame::SpriteCreate(const WSTRING& TextureFN, const POINT& UnitSize, int numCols, int numRows, float Scale) noexcept->JWLife*
{
	assert(m_Sprite);
	return m_Sprite->MakeLife(TextureFN, UnitSize, numCols, numRows, Scale);
}

void JWGame::SpriteWalk(EAnimationDirection Direction) noexcept
{
	assert(m_Sprite);
	m_Sprite->Walk(Direction);
}

void JWGame::SpriteJump() noexcept
{
	assert(m_Sprite);
	m_Sprite->Jump();
}

void JWGame::SpriteSetAnimation(EAnimationID AnimationID) noexcept
{
	assert(m_Sprite);
	m_Sprite->SetAnimation(AnimationID);
}

auto JWGame::SpawnMonster(const WSTRING& MonsterName, const D3DXVECTOR2& GlobalPosition) noexcept->JWMonster*
{
	assert(m_MonsterManager);
	return m_MonsterManager->Spawn(MonsterName, GlobalPosition);
}

auto JWGame::SpawnEffect(int EffectID, int Damage) noexcept->JWEffect*
{
	assert(m_EffectManager);
	return m_EffectManager->Spawn(EffectID, m_Sprite->GetCenterPosition(), m_Sprite->GetDirection(), Damage);
}

auto JWGame::GetTextObject() const noexcept->JWText*
{
	assert(m_Text);
	return m_Text.get();
}

auto JWGame::GetSpriteObject() const noexcept->JWLife*
{
	assert(m_Sprite);
	return m_Sprite.get();
}

auto JWGame::GetMonsterManagerObject() const noexcept->JWMonsterManager*
{
	assert(m_MonsterManager);
	return m_MonsterManager.get();
}

auto JWGame::GetEffectManagerObject() const noexcept->JWEffect*
{
	assert(m_EffectManager);
	return m_EffectManager.get();
}

void JWGame::DrawBackground() noexcept
{
	assert(m_Background);
	m_Background->Draw();
}

void JWGame::DrawMap() noexcept
{
	assert(m_Map);
	m_Map->Draw();
}

void JWGame::DrawMonsters() noexcept
{
	assert(m_MonsterManager);
	m_MonsterManager->Draw();
	if (m_bDrawBoundingBoxes)
		m_MonsterManager->DrawBoundingBox();
}

void JWGame::DrawSprite() noexcept
{
	assert(m_Sprite);
	m_Sprite->Draw();
	if (m_bDrawBoundingBoxes)
		m_Sprite->DrawBoundingBox();
}

void JWGame::DrawEffects() noexcept
{
	assert(m_EffectManager);
	m_EffectManager->Draw();
	if (m_bDrawBoundingBoxes)
		m_EffectManager->DrawBoundingBox();
}

void JWGame::DrawAllBase() noexcept
{
	DrawBackground();
	DrawMap();
	DrawMonsters();
	DrawSprite();
	DrawEffects();
}