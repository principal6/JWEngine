#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include "JWCommon.h"
#include <dinput.h>

namespace JWENGINE
{
	class JWInput final
	{
	public:
		JWInput();
		~JWInput() {};

		auto JWInput::Create(HWND hWnd, HINSTANCE hInstance)->EError;
		void JWInput::Destroy();

		auto JWInput::OnKeyDown(DWORD DIK_KeyCode)->bool;
		auto JWInput::OnKeyUp(DWORD DIK_KeyCode)->bool;
		auto JWInput::OnMouseMove()->DIMOUSESTATE2;
		auto JWInput::OnMouseButtonDown(int button)->bool;
		auto JWInput::OnMouseButtonUp(int button)->bool;

		auto JWInput::GetMouseButtonDown(int button)->bool;
		auto JWInput::GetKeyState(DWORD DIK_KeyCode) const->bool;
		void JWInput::GetAllKeyState(bool* Keys);

	private:
		bool JWInput::CreateMouseDevice(DWORD dwFlags);
		bool JWInput::CreateKeyboardDevice(DWORD dwFlags);
		bool JWInput::CheckMouseButton(int button);

	private:
		HWND m_hWnd;

		LPDIRECTINPUT8 m_DI8;
		LPDIRECTINPUTDEVICE8 m_DIDevKeyboard;
		LPDIRECTINPUTDEVICE8 m_DIDevMouse;

		char m_BufferKeyState[NUM_KEYS];
		bool m_KeyDown[NUM_KEYS];
		bool m_KeyUp[NUM_KEYS];
		DIMOUSESTATE2 m_MouseState;

		int m_MouseX;
		int m_MouseY;
		bool m_MouseBtnDown[3];
		bool m_MouseBtnUp[3];
		bool m_MouseBtnIdle[3];
	};
};