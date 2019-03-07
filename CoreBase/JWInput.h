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
		JWInput() {};
		~JWInput();

		void Create(const HWND hWnd, const HINSTANCE hInstance);
		
		auto OnKeyDown(DWORD DIK_KeyCode) noexcept->bool;
		auto OnKeyUp(DWORD DIK_KeyCode) const noexcept->bool;
		auto OnMouseMove() noexcept->const DIMOUSESTATE2;
		auto OnMouseButtonDown(int button) noexcept->bool;
		auto OnMouseButtonUp(int button) noexcept->bool;

		auto IsMouseButtonDown(int button) noexcept->bool;
		auto GetKeyState(DWORD DIK_KeyCode) const noexcept->bool;
		void GetAllKeyState(bool* Keys) noexcept;

	private:
		void CreateMouseDevice(DWORD dwFlags);
		void CreateKeyboardDevice(DWORD dwFlags);

		void CheckMouseButton(int button) noexcept;

	private:
		HWND m_hWnd{ nullptr };

		LPDIRECTINPUT8 m_pDirectInput{ nullptr };
		LPDIRECTINPUTDEVICE8 m_pKeyboardDevice{ nullptr };
		LPDIRECTINPUTDEVICE8 m_pMouseDevice{ nullptr };

		char m_BufferKeyState[NUM_KEYS]{};
		bool m_KeyDown[NUM_KEYS]{};
		bool m_KeyUp[NUM_KEYS]{};
		DIMOUSESTATE2 m_MouseState{};

		int m_MouseX{};
		int m_MouseY{};
		bool m_MouseBtnDown[3]{};
		bool m_MouseBtnUp[3]{};
		bool m_MouseBtnIdle[3]{};
	};
};