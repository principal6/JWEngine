#include "JWGame.h"

using namespace JWENGINE;

JWGame::JWGame()
{
	m_TimerStart = 0;
	m_TimerSec = 0;
	m_TimerAnim = 0;
	m_FPS = 0;
	m_KeyPressCount = 0;
	m_KeyToggleCount = 0;
	m_bDrawBoundingBoxes = false;
}

void JWGame::Create(const WSTRING GameName, CINT Width, CINT Height)
{
	// Set base directory
	wchar_t tempDir[MAX_FILE_LEN]{};
	GetCurrentDirectoryW(MAX_FILE_LEN, tempDir);
	m_BaseDir = tempDir;
	m_BaseDir = m_BaseDir.substr(0, m_BaseDir.find(PROJECT_FOLDER));

	std::wcout << m_BaseDir.c_str() << std::endl;

	// Create base (window and initialize Direct3D9)
	SWindowCreationData game_window_data = SWindowCreationData(GameName, WINDOW_X, WINDOW_Y, Width, Height, DEFAULT_COLOR_BLACK);

	if (m_Window = MAKE_UNIQUE(JWWindow)())
	{
		m_Window->CreateGameWindow(game_window_data);

		// Set main window handle
		m_hMainWnd = m_Window->GethWnd();
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}

	// Create input device
	if (m_Input = MAKE_UNIQUE(JWInput)())
	{
		// Pass main window's handle to JWInput
		m_Input->Create(m_Window->GethWnd(), m_Window->GethInstance());
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}

	// Create font object
	if (m_Text = MAKE_UNIQUE(JWText)())
	{
		m_Text->CreateInstantText(m_Window.get(), &m_BaseDir);
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}

	// Create image object
	if (m_Background = MAKE_UNIQUE(JWBackground)())
	{
		m_Background->Create(m_Window.get(), &m_BaseDir);
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}
	
	// Create map object
	if (m_Map = MAKE_UNIQUE(JWMap)())
	{
		m_Map->Create(m_Window.get(), &m_BaseDir);
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}
	
	// Create sprite object
	if (m_Sprite = MAKE_UNIQUE(JWLife)())
	{
		m_Sprite->Create(m_Window.get(), &m_BaseDir, m_Map.get());
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}

	// Create monster manager object
	if (m_MonsterManager = MAKE_UNIQUE(JWMonsterManager)())
	{
		m_MonsterManager->Create(m_Window.get(), &m_BaseDir, m_Map.get());
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}

	// Create effect manager object
	if (m_EffectManager = MAKE_UNIQUE(JWEffect)())
	{
		m_EffectManager->Create(m_Window.get(), &m_BaseDir, m_Map.get());
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}
}

void JWGame::SetRenderFunction(const PF_RENDER pfRender)
{
	m_pfRender = pfRender;
}

void JWGame::SetKeyboardFunction(const PF_KEYBOARD pfKeyboard)
{
	m_pfKeyboard = pfKeyboard;
}

void JWGame::ToggleBoundingBox()
{
	if (m_KeyToggleCount == 0)
	{
		m_bDrawBoundingBoxes = !m_bDrawBoundingBoxes;
		m_KeyToggleCount++;
	}
}

void JWGame::LoadMap(const WSTRING FileName)
{
	if (m_Map)
	{
		m_Map->LoadMap(FileName);
	}
}

void JWGame::Run()
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

	Destroy();
}

void JWGame::Shutdown()
{
	if (m_hMainWnd)
	{
		DestroyWindow(m_hMainWnd);
	}
}

PRIVATE void JWGame::MainLoop()
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

PRIVATE void JWGame::DetectInput()
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

void JWGame::Destroy()
{
	JW_DESTROY_SMART(m_Text);

	JW_DESTROY_SMART(m_EffectManager);
	JW_DESTROY_SMART(m_MonsterManager);
	JW_DESTROY_SMART(m_Sprite);
	JW_DESTROY_SMART(m_Map);
	JW_DESTROY_SMART(m_Background);
	JW_DESTROY_SMART(m_Input);
	JW_DESTROY_SMART(m_Window);
}

void JWGame::SetBackgroundTexture(const WSTRING TextureFN)
{
	assert(m_Background);
	m_Background->SetTexture(TextureFN);
}

auto JWGame::SpriteCreate(const WSTRING TextureFN, const POINT UnitSize, CINT numCols, CINT numRows, const float Scale)->JWLife*
{
	assert(m_Sprite);
	return m_Sprite->MakeLife(TextureFN, UnitSize, numCols, numRows, Scale);
}

void JWGame::SpriteWalk(const EAnimationDirection Direction)
{
	assert(m_Sprite);
	m_Sprite->Walk(Direction);
}

void JWGame::SpriteJump()
{
	assert(m_Sprite);
	m_Sprite->Jump();
}

void JWGame::SpriteSetAnimation(const EAnimationID AnimationID)
{
	assert(m_Sprite);
	m_Sprite->SetAnimation(AnimationID);
}

auto JWGame::SpawnEffect(CINT EffectID, CINT Damage)->JWEffect*
{
	assert(m_EffectManager);
	return m_EffectManager->Spawn(EffectID, m_Sprite->GetCenterPosition(), m_Sprite->GetDirection(), Damage);
}

auto JWGame::SpawnMonster(const WSTRING MonsterName, const D3DXVECTOR2 GlobalPosition)->JWMonster*
{
	assert(m_MonsterManager);
	return m_MonsterManager->Spawn(MonsterName, GlobalPosition);
}

auto JWGame::GetTextObject()->JWText*
{
	assert(m_Text);
	return m_Text.get();
}

auto JWGame::GetSpriteObject()->JWLife*
{
	assert(m_Sprite);
	return m_Sprite.get();
}

auto JWGame::GetMonsterManagerObject()->JWMonsterManager*
{
	assert(m_MonsterManager);
	return m_MonsterManager.get();
}

auto JWGame::GetEffectManagerObject()->JWEffect*
{
	assert(m_EffectManager);
	return m_EffectManager.get();
}

void JWGame::DrawBackground()
{
	assert(m_Background);
	m_Background->Draw();
}

void JWGame::DrawMap()
{
	assert(m_Map);
	m_Map->Draw();
}

void JWGame::DrawMonsters()
{
	assert(m_MonsterManager);
	m_MonsterManager->Draw();
	if (m_bDrawBoundingBoxes)
		m_MonsterManager->DrawBoundingBox();
}

void JWGame::DrawSprite()
{
	assert(m_Sprite);
	m_Sprite->Draw();
	if (m_bDrawBoundingBoxes)
		m_Sprite->DrawBoundingBox();
}

void JWGame::DrawEffects()
{
	assert(m_EffectManager);
	m_EffectManager->Draw();
	if (m_bDrawBoundingBoxes)
		m_EffectManager->DrawBoundingBox();
}

void JWGame::DrawAllBase()
{
	DrawBackground();
	DrawMap();
	DrawMonsters();
	DrawSprite();
	DrawEffects();
}