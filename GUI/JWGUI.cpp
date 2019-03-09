#include "JWGUI.h"

using namespace JWENGINE;

// Static member variables
SGUIIMEInputInfo JWGUI::ms_IMEInfo;
VECTOR<HWND> JWGUI::ms_hWndQuitStack;

// Base window procedure for GUI window
LRESULT CALLBACK JWENGINE::GUIWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// Handle to the context for the Input Method Manager(IMM), which is needed for Input Method Editor(IME)
	HIMC hIMC{ nullptr };

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
		JWGUI::ms_hWndQuitStack.push_back(hWnd);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

void JWGUI::Create(SWindowCreationData& WindowCreationData, JWGUIWindow*& OutPtrMainGUIWindow) noexcept
{
	m_pGUIWindows.push_back(MAKE_UNIQUE_AND_MOVE(JWGUIWindow)());
	m_ppGUIWindows.push_back(&OutPtrMainGUIWindow);

	OutPtrMainGUIWindow = m_pGUIWindows[m_pGUIWindows.size() - 1].get();
	WindowCreationData.proc = GUIWindowProc;
	OutPtrMainGUIWindow->Create(WindowCreationData);
}

void JWGUI::AddGUIWindow(SWindowCreationData& WindowCreationData, JWGUIWindow*& OutPtrGUIWindow) noexcept
{
	m_pGUIWindows.push_back(MAKE_UNIQUE_AND_MOVE(JWGUIWindow)());
	m_ppGUIWindows.push_back(&OutPtrGUIWindow);

	OutPtrGUIWindow = m_pGUIWindows[m_pGUIWindows.size() - 1].get();
	WindowCreationData.proc = GUIWindowProc;
	OutPtrGUIWindow->Create(WindowCreationData);
}

void JWGUI::DestroyGUIWindow(const JWGUIWindow* pGUIWindow) noexcept
{
	if (m_pGUIWindows.size())
	{
		size_t iterator_window_index{};

		for (auto& iterator : m_pGUIWindows)
		{
			if (iterator.get() == pGUIWindow)
			{
				EraseGUIWindow(iterator_window_index);
				break;
			}

			iterator_window_index++;
		}
	}
}

void JWGUI::SetMainLoopFunction(const PF_MAINLOOP pfMainLoop) noexcept
{
	m_pfMainLoop = pfMainLoop;
}

void JWGUI::Run() noexcept
{
	HWND hwnd_active{ nullptr };

	bool b_gui_window_destroyed{ false };
	size_t destroyed_gui_window_index{};
	size_t iterator_index{};

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

		hwnd_active = GetActiveWindow();

		//std::cout << "[DEBUG] ACTIVE WINDOW HWND: " << hActiveWindow << std::endl;

		// Update all JWGUIWindows.
		if (m_pGUIWindows.size())
		{
			for (auto& iterator : m_pGUIWindows)
			{
				iterator->Update(m_MSG, ms_IMEInfo, ms_hWndQuitStack, hwnd_active);
			}
		}

		// Check if a JWGUIWindow is destroyed,
		// If so, erase it from the dynamic array,
		// and if not, BeginRender().
		if (m_pGUIWindows.size())
		{
			iterator_index = 0;
			for (auto& iterator : m_pGUIWindows)
			{
				if (iterator->IsDestroyed())
				{
					b_gui_window_destroyed = true;
					destroyed_gui_window_index = iterator_index;
				}
					
				iterator->BeginRender();

				iterator_index++;
			}
		}

		// Draw all the controls in the JWGUIWindow.
		if (m_pGUIWindows.size())
		{
			for (auto& iterator : m_pGUIWindows)
			{
				iterator->DrawAllControls();
			}
		}

		if (m_pfMainLoop)
		{
			// Call the outter main loop.
			m_pfMainLoop();
		}

		if (m_pGUIWindows.size())
		{
			for (auto& iterator : m_pGUIWindows)
			{
				iterator->EndRender();
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

				ClearGUIWindow();
			}
			else
			{
				EraseGUIWindow(destroyed_gui_window_index);
			}
		}

		if (!m_pGUIWindows.size())
		{
			m_bIsGUIRunning = false;
		}
	}
}

void JWGUI::EraseGUIWindow(size_t index) noexcept
{
	if (m_pGUIWindows.size())
	{
		m_pGUIWindows.erase(index);
	}

	if (m_ppGUIWindows.size())
	{
		*m_ppGUIWindows[index] = nullptr;
		m_ppGUIWindows.erase(index);
	}
}

void JWGUI::ClearGUIWindow() noexcept
{
	if (m_pGUIWindows.size())
	{
		m_pGUIWindows.clear();
	}

	if (m_ppGUIWindows.size())
	{
		for (auto& iterator : m_ppGUIWindows)
		{
			*iterator = nullptr;
		}
		m_ppGUIWindows.clear();
	}
}