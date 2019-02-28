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

		void Create(const HWND hWnd, const HINSTANCE hInstance);
		void Destroy() noexcept;

		auto OnKeyDown(const DWORD DIK_KeyCode) noexcept->const bool;
		auto OnKeyUp(const DWORD DIK_KeyCode) const noexcept->const bool;
		auto OnMouseMove() noexcept->const DIMOUSESTATE2;
		auto OnMouseButtonDown(const int button) noexcept->const bool;
		auto OnMouseButtonUp(const int button) noexcept->const bool;

		auto IsMouseButtonDown(const int button) noexcept->const bool;
		auto GetKeyState(const DWORD DIK_KeyCode) const noexcept->const bool;
		void GetAllKeyState(bool* Keys) noexcept;

	private:
		void CreateMouseDevice(DWORD dwFlags);
		void CreateKeyboardDevice(DWORD dwFlags);

		void CheckMouseButton(const int button) noexcept;

	private:
		HWND m_hWnd;

		LPDIRECTINPUT8 m_pDirectInput;
		LPDIRECTINPUTDEVICE8 m_pKeyboardDevice;
		LPDIRECTINPUTDEVICE8 m_pMouseDevice;

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