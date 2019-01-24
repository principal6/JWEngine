#include "JWInput.h"

using namespace JWENGINE;

JWInput::JWInput()
{
	m_DI8 = nullptr;
	m_DIDevKeyboard = nullptr;
	m_DIDevMouse = nullptr;

	memset(&m_BufferKeyState, 0, sizeof(m_BufferKeyState));
	memset(&m_KeyDown, false, sizeof(m_KeyDown));
	memset(&m_KeyUp, false, sizeof(m_KeyUp));

	m_MouseX = 0;
	m_MouseY = 0;
}

auto JWInput::Create(HWND hWnd, HINSTANCE hInstance)->EError
{
	m_hWnd = hWnd;

	memset(m_MouseBtnDown, false, sizeof(m_MouseBtnDown));
	memset(m_MouseBtnUp, false, sizeof(m_MouseBtnUp));
	memset(m_MouseBtnIdle, true, sizeof(m_MouseBtnIdle));

	if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void **) &m_DI8, nullptr)))
		return EError::OBJECT_NOT_CREATED;

	if (FAILED(CreateMouseDevice(DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return EError::OBJECT_NOT_CREATED;

	if (FAILED(CreateKeyboardDevice(DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return EError::OBJECT_NOT_CREATED;

	return EError::OK;
}

auto JWInput::CreateMouseDevice(DWORD dwFlags)->bool
{
	if(FAILED(m_DI8->CreateDevice(GUID_SysMouse, &m_DIDevMouse, nullptr)))
		return false;

	if(FAILED(m_DIDevMouse->SetDataFormat(&c_dfDIMouse2)))
		return false;

	if(FAILED(m_DIDevMouse->SetCooperativeLevel(m_hWnd, dwFlags)))
		return false;

	if(FAILED(m_DIDevMouse->Acquire()))
		return false;

	return true;
}

auto JWInput::CreateKeyboardDevice(DWORD dwFlags)->bool
{
	if(FAILED(m_DI8->CreateDevice(GUID_SysKeyboard, &m_DIDevKeyboard, nullptr)))
		return false;

	if(FAILED(m_DIDevKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;

	if(FAILED(m_DIDevKeyboard->SetCooperativeLevel(m_hWnd, dwFlags)))
		return false;

	if(FAILED(m_DIDevKeyboard->Acquire()))
		return false;
	
	return true;
}

void JWInput::Destroy()
{
	if (m_DIDevMouse)
	{
		m_DIDevMouse->Unacquire();
		m_DIDevMouse->Release();
		m_DIDevMouse = nullptr;
	}
	if (m_DIDevKeyboard)
	{
		m_DIDevKeyboard->Unacquire();
		m_DIDevKeyboard->Release();
		m_DIDevKeyboard = nullptr;
	}
	if (m_DI8)
	{
		m_DI8->Release();
		m_DI8 = nullptr;
	}
}

auto JWInput::OnKeyDown(DWORD DIK_KeyCode)->bool
{
	HRESULT hr;
	if(FAILED(hr = m_DIDevKeyboard->GetDeviceState(sizeof(m_BufferKeyState),
		(LPVOID)&m_BufferKeyState)))
	{
		if(hr == DIERR_INPUTLOST)
			m_DIDevKeyboard->Acquire();
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

auto JWInput::OnKeyUp(DWORD DIK_KeyCode)->bool
{
	return m_KeyUp[DIK_KeyCode];
}

auto JWInput::OnMouseMove()->DIMOUSESTATE2
{
	DIMOUSESTATE2 Result;
	HRESULT hr;
	
	memset(&m_MouseState, 0, sizeof(m_MouseState));
	if(FAILED(hr = m_DIDevMouse->GetDeviceState(sizeof(m_MouseState),(LPVOID) &m_MouseState)))
	{
		if(hr == DIERR_INPUTLOST)
			m_DIDevMouse->Acquire();
	}

	Result.lX = m_MouseState.lX;
	Result.lY = m_MouseState.lY;
	Result.lZ = m_MouseState.lZ;

	return Result;
}

auto JWInput::CheckMouseButton(int button)->bool
{
	if(m_MouseState.rgbButtons[button] & 0x80) // 버튼이 눌림
	{
		if ( (m_MouseBtnDown[button] == false) && (m_MouseBtnIdle[button] == true) )
		{
			m_MouseBtnUp[button] = false;
			m_MouseBtnDown[button] = true;
		}
	}
	else if(!m_MouseState.rgbButtons[button]) // 버튼이 눌리지 않음
	{
		if ( (m_MouseBtnDown[button] == false) && (m_MouseBtnIdle[button] == false) )
		{
			m_MouseBtnUp[button] = true;
			m_MouseBtnIdle[button] = true;
			m_MouseBtnDown[button] = false;
		}
		else if ( (m_MouseBtnDown[button] == true) && (m_MouseBtnIdle[button] == true) )
		{
			m_MouseBtnUp[button] = true;
			m_MouseBtnDown[button] = false;
		}
	}

	return true;
}

auto JWInput::OnMouseButtonDown(int button)->bool
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

auto JWInput::OnMouseButtonUp(int button)->bool
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

auto JWInput::GetKeyState(DWORD DIK_KeyCode) const->bool
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

void JWInput::GetAllKeyState(bool* Keys)
{
	// Check all DIK states
	HRESULT hr;
	if (FAILED(hr = m_DIDevKeyboard->GetDeviceState(sizeof(m_BufferKeyState), (LPVOID)&m_BufferKeyState)))
	{
		if (hr == DIERR_INPUTLOST)
		{
			m_DIDevKeyboard->Acquire();
		}
	}

	for (int i = 1; i <= NUM_KEYS; i++)
	{
		Keys[i] = false;

		if (m_BufferKeyState[i] & 0x80)
			Keys[i] = true;
	}
}

auto JWInput::GetMouseButtonDown(int button)->bool
{
	CheckMouseButton(button);
	return m_MouseBtnDown[button];
}