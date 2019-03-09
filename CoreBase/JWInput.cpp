#include "JWInput.h"

using namespace JWENGINE;

JWInput::~JWInput()
{
	if (m_pMouseDevice)
	{
		m_pMouseDevice->Unacquire();
		m_pMouseDevice->Release();
		m_pMouseDevice = nullptr;
	}
	if (m_pKeyboardDevice)
	{
		m_pKeyboardDevice->Unacquire();
		m_pKeyboardDevice->Release();
		m_pKeyboardDevice = nullptr;
	}

	JW_RELEASE(m_pDirectInput);
}

void JWInput::Create(const HWND hWnd, const HINSTANCE hInstance)
{
	m_hWnd = hWnd;

	memset(m_MouseBtnDown, false, sizeof(m_MouseBtnDown));
	memset(m_MouseBtnUp, false, sizeof(m_MouseBtnUp));
	memset(m_MouseBtnIdle, true, sizeof(m_MouseBtnIdle));

	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&m_pDirectInput, nullptr)))
	{
		THROW_CREATION_FAILED;
	}

	try
	{
		CreateMouseDevice(DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
		CreateKeyboardDevice(DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	}
	catch (creation_failed& e)
	{
		std::cout << e.what() << std::endl;

		abort();
	}
	catch (dxinput_failed& e)
	{
		std::cout << e.what() << std::endl;

		abort();
	}
}

PRIVATE void JWInput::CreateMouseDevice(DWORD dwFlags)
{
	if (FAILED(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr)))
		THROW_CREATION_FAILED;
	
	if (FAILED(m_pMouseDevice->SetDataFormat(&c_dfDIMouse2)))
		THROW_DXINPUT_FAILED;

	if (FAILED(m_pMouseDevice->SetCooperativeLevel(m_hWnd, dwFlags)))
		THROW_DXINPUT_FAILED;

	if (FAILED(m_pMouseDevice->Acquire()))
		THROW_DXINPUT_FAILED;
}

PRIVATE void JWInput::CreateKeyboardDevice(DWORD dwFlags)
{
	if (FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, nullptr)))
		THROW_CREATION_FAILED;

	if (FAILED(m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard)))
		THROW_DXINPUT_FAILED;

	if (FAILED(m_pKeyboardDevice->SetCooperativeLevel(m_hWnd, dwFlags)))
		THROW_DXINPUT_FAILED;

	if (FAILED(m_pKeyboardDevice->Acquire()))
		THROW_DXINPUT_FAILED;
}

auto JWInput::OnKeyDown(DWORD DIK_KeyCode) noexcept->bool
{
	HRESULT hr;
	if (FAILED(hr = m_pKeyboardDevice->GetDeviceState(sizeof(m_BufferKeyState),
		(LPVOID)&m_BufferKeyState)))
	{
		if (hr == DIERR_INPUTLOST)
		{
			m_pKeyboardDevice->Acquire();
		}
	}

	if (m_BufferKeyState[DIK_KeyCode] & 0x80)	// 키가 눌리면 true를 반환
	{
		m_KeyUp[DIK_KeyCode] = false;
		m_KeyDown[DIK_KeyCode] = true;
		return true;
	}
	else
	{
		if (m_KeyDown[DIK_KeyCode] == true) // 이전에 키가 눌려있었다면?
		{
			m_KeyUp[DIK_KeyCode] = true;
		}
		else
		{
			m_KeyUp[DIK_KeyCode] = false;
		}
		
		m_KeyDown[DIK_KeyCode] = false;
		return false;
	}
}

auto JWInput::OnKeyUp(DWORD DIK_KeyCode) const noexcept->bool
{
	return m_KeyUp[DIK_KeyCode];
}

auto JWInput::OnMouseMove() noexcept->const DIMOUSESTATE2
{
	DIMOUSESTATE2 Result;

	HRESULT hr;
	memset(&m_MouseState, 0, sizeof(m_MouseState));
	if (FAILED(hr = m_pMouseDevice->GetDeviceState(sizeof(m_MouseState), (LPVOID)&m_MouseState)))
	{
		if (hr == DIERR_INPUTLOST)
		{
			m_pMouseDevice->Acquire();
		}
	}

	Result.lX = m_MouseState.lX;
	Result.lY = m_MouseState.lY;
	Result.lZ = m_MouseState.lZ;

	return Result;
}

auto JWInput::OnMouseButtonDown(int button) noexcept->bool
{
	CheckMouseButton(button);

	if (m_MouseBtnDown[button] == true)
	{
		m_MouseBtnDown[button] = false;
		m_MouseBtnIdle[button] = false;
		return true;
	}

	return false;
}

auto JWInput::OnMouseButtonUp(int button) noexcept->bool
{
	CheckMouseButton(button);

	if (m_MouseBtnUp[button] == true)
	{
		m_MouseBtnUp[button] = false;
		m_MouseBtnIdle[button] = true;
		return true;
	}

	return false;
}

PRIVATE void JWInput::CheckMouseButton(int button) noexcept
{
	if (m_MouseState.rgbButtons[button] & 0x80) // 버튼이 눌림
	{
		if ((m_MouseBtnDown[button] == false) && (m_MouseBtnIdle[button] == true))
		{
			m_MouseBtnUp[button] = false;
			m_MouseBtnDown[button] = true;
		}
	}
	else if (!m_MouseState.rgbButtons[button]) // 버튼이 눌리지 않음
	{
		if ((m_MouseBtnDown[button] == false) && (m_MouseBtnIdle[button] == false))
		{
			m_MouseBtnUp[button] = true;
			m_MouseBtnIdle[button] = true;
			m_MouseBtnDown[button] = false;
		}
		else if ((m_MouseBtnDown[button] == true) && (m_MouseBtnIdle[button] == true))
		{
			m_MouseBtnUp[button] = true;
			m_MouseBtnDown[button] = false;
		}
	}
}

auto JWInput::GetKeyState(DWORD DIK_KeyCode) const noexcept->bool
{
	if (m_BufferKeyState[DIK_KeyCode] & 0x80)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void JWInput::GetAllKeyState(bool* Keys) noexcept
{
	// Check all DIK states
	HRESULT hr;
	if (FAILED(hr = m_pKeyboardDevice->GetDeviceState(sizeof(m_BufferKeyState), (LPVOID)&m_BufferKeyState)))
	{
		if (hr == DIERR_INPUTLOST)
		{
			m_pKeyboardDevice->Acquire();
		}
	}

	for (int i = 1; i <= NUM_KEYS; i++)
	{
		Keys[i] = false;

		if (m_BufferKeyState[i] & 0x80)
		{
			Keys[i] = true;
		}
	}
}

auto JWInput::IsMouseButtonDown(int button) noexcept->bool
{
	CheckMouseButton(button);

	return m_MouseBtnDown[button];
}