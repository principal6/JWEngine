#include "JWGUI.h"

using namespace JWENGINE;

// Static member variables
SGUIIMEInputInfo JWGUI::ms_IMEInfo;
HWND JWGUI::ms_QuitWindowHWND;

// Base window procedure for GUI window
LRESULT CALLBACK JWENGINE::GUIWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// Handle to the context for the Input Method Manager(IMM), which is needed for Input Method Editor(IME)
	HIMC hIMC = nullptr;

	switch (Message)
	{
	case WM_SYSCOMMAND:
		if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) // Disable Alt key
		{
			return 0;
		}
		break;
	case WM_IME_COMPOSITION:
		hIMC = ImmGetContext(hWnd);

		if (lParam & GCS_COMPSTR)
		{
			// For debugging
			//std::cout << "COMPSTR: " << std::endl;

			memset(JWGUI::ms_IMEInfo.IMEWritingChar, 0, MAX_FILE_LEN);
			ImmGetCompositionString(hIMC, GCS_COMPSTR, JWGUI::ms_IMEInfo.IMEWritingChar, MAX_FILE_LEN);
			JWGUI::ms_IMEInfo.bIMEWriting = true;
			JWGUI::ms_IMEInfo.bIMECompleted = false;
		}
		else if (lParam & GCS_RESULTSTR)
		{
			// For debugging
			//std::cout << "GCS_RESULTSTR: " << std::endl;

			memset(JWGUI::ms_IMEInfo.IMECompletedChar, 0, MAX_FILE_LEN);
			ImmGetCompositionString(hIMC, GCS_RESULTSTR, JWGUI::ms_IMEInfo.IMECompletedChar, MAX_FILE_LEN);
			JWGUI::ms_IMEInfo.bIMEWriting = false;
			JWGUI::ms_IMEInfo.bIMECompleted = true;
		}

		ImmReleaseContext(hWnd, hIMC);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		JWGUI::ms_QuitWindowHWND = hWnd;
		return 0;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

JWGUI::JWGUI()
{
	m_pMainGUIWindow = nullptr;
}

auto JWGUI::Create(SWindowCreationData& WindowCreationData)->EError
{
	if (m_pMainGUIWindow = new JWGUIWindow)
	{
		WindowCreationData.proc = GUIWindowProc;
		m_pMainGUIWindow->Create(WindowCreationData);

		m_ppGUIWindows.push_back(&m_pMainGUIWindow);

		return EError::OK;
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}
}

void JWGUI::Destroy()
{
	if (m_ppGUIWindows.size())
	{
		for (JWGUIWindow** iterator : m_ppGUIWindows)
		{
			(*iterator)->Destroy();
			iterator = nullptr;
		}
	}
}

void JWGUI::AddGUIWindow(SWindowCreationData& WindowCreationData, JWGUIWindow** ppGUIWindow)
{
	*ppGUIWindow = new JWGUIWindow;

	WindowCreationData.proc = GUIWindowProc;
	(*ppGUIWindow)->Create(WindowCreationData);

	m_ppGUIWindows.push_back(ppGUIWindow);
}

auto JWGUI::GetMainGUIWindowPtr()->JWGUIWindow*
{
	if (m_ppGUIWindows.size())
	{
		return *m_ppGUIWindows[0];
	}

	return nullptr;
}

void JWGUI::SetMainLoopFunction(PF_MAINLOOP pfMainLoop)
{
	m_pfMainLoop = pfMainLoop;
}

void JWGUI::Run()
{
	bool b_gui_window_destroyed = false;
	size_t destroyed_gui_window_index = 0;
	size_t iterator_index = 0;

	m_bIsGUIRunning = true;

	while (m_bIsGUIRunning)
	{
		b_gui_window_destroyed = false;
		destroyed_gui_window_index = 0;

		if (PeekMessage(&m_MSG, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&m_MSG);
			DispatchMessage(&m_MSG);
		}

		HWND hActiveWindow = GetActiveWindow();

		//std::cout << "[DEBUG] ACTIVE WINDOW HWND: " << hActiveWindow << std::endl;

		// Update all JWGUIWindows.
		if (m_ppGUIWindows.size())
		{
			for (JWGUIWindow** iterator : m_ppGUIWindows)
			{
				(*iterator)->Update(m_MSG, ms_IMEInfo, ms_QuitWindowHWND, hActiveWindow);
			}
		}

		// Check if a JWGUIWindow is destroyed,
		// If so, erase it from the dynamic array,
		// and if not, BeginRender().
		if (m_ppGUIWindows.size())
		{
			iterator_index = 0;
			for (JWGUIWindow** iterator : m_ppGUIWindows)
			{
				if ((*iterator)->IsDestroyed())
				{
					b_gui_window_destroyed = true;
					destroyed_gui_window_index = iterator_index;
				}
					
				(*iterator)->BeginRender();

				iterator_index++;
			}
		}

		// Draw all the controls in the JWGUIWindow.
		if (m_ppGUIWindows.size())
		{
			for (JWGUIWindow** iterator : m_ppGUIWindows)
			{
				(*iterator)->DrawAllControls();
			}
		}

		if (m_pfMainLoop)
		{
			// Call the outter main loop.
			m_pfMainLoop();
		}

		if (m_ppGUIWindows.size())
		{
			for (JWGUIWindow** iterator : m_ppGUIWindows)
			{
				(*iterator)->EndRender();
			}
		}

		// @Warning:
		// We must empty m_MSG to avoid duplicate messages!
		memset(&m_MSG, 0, sizeof(m_MSG));

		ms_IMEInfo.clear();

		// Check window destruction.
		if (b_gui_window_destroyed)
		{
			if (destroyed_gui_window_index == 0)
			{
				// IF,
				// main JWGUIWindow is destroyed,
				// all JWGUIWindows must be destroyed,
				// and the program must exit.

				iterator_index = 0;
				for (JWGUIWindow** iterator : m_ppGUIWindows)
				{
					JW_DESTROY((*m_ppGUIWindows[iterator_index]));
					m_ppGUIWindows[iterator_index] = nullptr;

					iterator_index++;
				}

				m_ppGUIWindows.clear();
			}
			else
			{
				JW_DESTROY((*m_ppGUIWindows[destroyed_gui_window_index]));
				m_ppGUIWindows[destroyed_gui_window_index] = nullptr;

				m_ppGUIWindows.erase(destroyed_gui_window_index);
			}
		}

		if (!m_ppGUIWindows.size())
		{
			m_bIsGUIRunning = false;
		}

		ms_QuitWindowHWND = nullptr;
	}

	Destroy();
}