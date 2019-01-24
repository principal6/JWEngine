#include "JWGame.h"

using namespace JWENGINE;

// Static member variables declaration
const int JWGame::WINDOW_X = 50;
const int JWGame::WINDOW_Y = 50;

JWGame::JWGame()
{
	m_TimerStart = 0;
	m_TimerSec = 0;
	m_TimerAnim = 0;
	m_FPS = 0;
	m_KeyPressCount = 0;
	m_KeyToggleCount = 0;
	m_bSpriteWalking = false;
	m_bDrawBoundingBoxes = false;
}

auto JWGame::Create(int Width, int Height)->EError
{
	// Set base directory
	wchar_t tempDir[MAX_FILE_LEN]{};
	GetCurrentDirectoryW(MAX_FILE_LEN, tempDir);
	m_BaseDir = tempDir;
	m_BaseDir = m_BaseDir.substr(0, m_BaseDir.find(PROJECT_FOLDER));

	std::wcout << m_BaseDir.c_str() << std::endl;

	// Create base (window and initialize Direct3D9)
	if (m_Window = MAKE_UNIQUE(JWWindow)())
	{
		if (JW_FAILED(m_Window->CreateGameWindow(WINDOW_X, WINDOW_Y, Width, Height)))
			return EError::WINDOW_NOT_CREATED;

		// Set main window handle
		m_hMainWnd = m_Window->GethWnd();
	}

	// Create input device
	if (m_Input = MAKE_UNIQUE(JWInput)())
	{
		// Pass main window's handle to JWInput
		if (JW_FAILED(m_Input->Create(m_Window->GethWnd(), m_Window->GethInstance())))
			return EError::INPUT_NOT_CREATED;
	}

	// Create font object
	if (m_Font = MAKE_UNIQUE(JWFont)())
	{
		if (JW_FAILED(m_Font->Create(m_Window.get(), m_BaseDir)))
			return EError::FONT_NOT_CREATED;
	}

	// Create image object
	if (m_Background = MAKE_UNIQUE(JWBackground)())
	{
		if (JW_FAILED(m_Background->Create(m_Window.get(), m_BaseDir)))
			return EError::IMAGE_NOT_CREATED;
	}
	
	// Create map object
	if (m_Map = MAKE_UNIQUE(JWMap)())
	{
		if (JW_FAILED(m_Map->Create(m_Window.get(), m_BaseDir)))
			return EError::MAP_NOT_CREATED;
	}
	
	// Create sprite object
	if (m_Sprite = MAKE_UNIQUE(JWLife)())
	{
		if (JW_FAILED(m_Sprite->Create(m_Window.get(), m_BaseDir, m_Map.get())))
			return EError::SPRITE_NOT_CREATED;
	}

	// Create monster manager object
	if (m_MonsterManager = MAKE_UNIQUE(JWMonsterManager)())
	{
		if (JW_FAILED(m_MonsterManager->Create(m_Window.get(), m_BaseDir, m_Map.get())))
			return EError::MONSTERMANAGER_NOT_CREATED;
	}

	// Create effect manager object
	if (m_EffectManager = MAKE_UNIQUE(JWEffect)())
	{
		if (JW_FAILED(m_EffectManager->Create(m_Window.get(), m_BaseDir, m_Map.get())))
			return EError::EFFECTMANAGER_NOT_CREATED;
	}

	return EError::OK;
}

void JWGame::SetRenderFunction(PF_RENDER pfRender)
{
	m_pfRender = pfRender;
}

void JWGame::SetKeyboardFunction(PF_KEYBOARD pfKeyboard)
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

auto JWGame::LoadMap(WSTRING FileName)->EError
{
	if (m_Map)
	{
		m_Map->LoadMap(FileName);
		return EError::OK;
	}
	return EError::OBJECT_NOT_CREATED;
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

void JWGame::MainLoop()
{
	DetectInput();

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

void JWGame::DetectInput()
{
	m_bSpriteWalking = false;

	m_Input->GetAllKeyState(m_Keys);

	for (int i = 0; i < NUM_KEYS; i++)
	{
		if (m_Keys[i] == true)
		{
			if (m_pfKeyboard)
				m_pfKeyboard(i);
		}
	}

	if ((!m_bSpriteWalking) && (!m_Sprite->IsBeingAnimated()))
	{
		m_Sprite->SetAnimation(EAnimationID::Idle);
	}
}

void JWGame::Destroy()
{
	JW_DESTROY_SMART(m_Font);
	JW_DESTROY_SMART(m_EffectManager);
	JW_DESTROY_SMART(m_MonsterManager);
	JW_DESTROY_SMART(m_Sprite);
	JW_DESTROY_SMART(m_Map);
	JW_DESTROY_SMART(m_Background);
	JW_DESTROY_SMART(m_Input);
	JW_DESTROY_SMART(m_Window);
}

void JWGame::SetBackground(WSTRING TextureFN)
{
	assert(m_Background);
	m_Background->SetTexture(TextureFN);
}

auto JWGame::SpriteCreate(WSTRING TextureFN, POINT UnitSize, int numCols, int numRows, float Scale)->JWLife*
{
	assert(m_Sprite);
	return m_Sprite->MakeLife(TextureFN, UnitSize, numCols, numRows, Scale);
}

void JWGame::SpriteWalk(EAnimationDirection Direction)
{
	assert(m_Sprite);

	m_bSpriteWalking = true;
	m_Sprite->Walk(Direction);
}

void JWGame::SpriteJump()
{
	assert(m_Sprite);
	m_Sprite->Jump();
}

void JWGame::SpriteSetAnimation(EAnimationID AnimationID)
{
	assert(m_Sprite);
	m_Sprite->SetAnimation(AnimationID);
}

auto JWGame::SpawnEffect(int EffectID, int Damage)->JWEffect*
{
	assert(m_EffectManager);
	return m_EffectManager->Spawn(EffectID, m_Sprite->GetCenterPosition(), m_Sprite->GetDirection(), Damage);
}

auto JWGame::SpawnMonster(WSTRING MonsterName, D3DXVECTOR2 GlobalPosition)->JWMonster*
{
	assert(m_MonsterManager);
	return m_MonsterManager->Spawn(MonsterName, GlobalPosition);
}

auto JWGame::GetFontObject()->JWFont*
{
	assert(m_Font);
	return m_Font.get();
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

void JWGame::DrawFont()
{
	assert(m_Font);
	m_Font->Draw();
}

void JWGame::DrawAllBase()
{
	DrawBackground();
	DrawMap();
	DrawMonsters();
	DrawSprite();
	DrawEffects();
	DrawFont();
}