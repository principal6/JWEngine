#include "JWGUI.h"

using namespace JWENGINE;

// Base window procedure for GUI window
LRESULT CALLBACK GUIWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_SYSCOMMAND:
		if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) // Disable Alt key
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

auto JWGUI::Create()->EError
{
	// Create base (window and initialize Direct3D9)
	if (m_Window = MAKE_UNIQUE(JWWindow)())
	{
		m_Window->CreateGUIWindow(0, 100, 800, 600, D3DCOLOR_XRGB(0, 120, 255), GUIWindowProc);
	}

	return EError::OK;
}

void JWGUI::Destroy()
{
	JW_DESTROY_SMART(m_Window);
}

void JWGUI::Run()
{
	while (m_MSG.message != WM_QUIT)
	{
		if (PeekMessage(&m_MSG, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&m_MSG);
			DispatchMessage(&m_MSG);

			HandleMessage();
		}
		else
		{
			MainLoop();
		}
	}

	Destroy();
}

void JWGUI::HandleMessage()
{
	switch (m_MSG.message)
	{
	case WM_LBUTTONDOWN:

		break;
	default:
		break;
	}
}

void JWGUI::MainLoop()
{

}