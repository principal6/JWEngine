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

void JWGUI::CreateMainWindow(SWindowCreationData& WindowCreationData) noexcept
{
	m_IsMainWindowCreated = true;

	CreateAdditionalWindow(WindowCreationData);
}

PRIVATE void JWGUI::AssertValidObject() const noexcept
{
	if (!m_IsMainWindowCreated)
	{
		// @warning:
		// You must call CreateMainWindow()
		// before calling any other functions
		// in order to make this JWGUI a valid object.

		assert(false);
	}
}

void JWGUI::CreateAdditionalWindow(SWindowCreationData& WindowCreationData) noexcept
{
	AssertValidObject();

	m_pGUIWindows.push_back(MAKE_UNIQUE_AND_MOVE(JWGUIWindow)());

	WindowCreationData.proc = GUIWindowProc;
	m_pGUIWindows[m_pGUIWindows.size() - 1]->Create(WindowCreationData);
}

void JWGUI::DestroyAdditionalWindow(size_t AdditionalWindowIndex) noexcept
{
	AssertValidObject();

	size_t iterator_window_index{};

	for (auto& iterator : m_pGUIWindows)
	{
		if (iterator_window_index == AdditionalWindowIndex + 1)
		{
			EraseGUIWindow(iterator_window_index);
			break;
		}

		iterator_window_index++;
	}
}

auto JWGUI::IsAdditionalWindowAlive(size_t AdditionalWindowIndex) const->bool
{
	AssertValidObject();

	if (AdditionalWindowIndex + 1 <= m_pGUIWindows.size() - 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

auto JWGUI::MainWindow() const->JWGUIWindow&
{
	AssertValidObject();
	
	return *m_pGUIWindows[0].get();
}

auto JWGUI::AdditionalWindow(size_t AdditionalWindowIndex) const->JWGUIWindow&
{
	AssertValidObject();

	if (AdditionalWindowIndex > m_pGUIWindows.size() - 2)
	{
		AdditionalWindowIndex = m_pGUIWindows.size() - 1;
	}
	else
	{
		++AdditionalWindowIndex;
	}

	return *m_pGUIWindows[AdditionalWindowIndex].get();
}

void JWGUI::SetMainLoopFunction(const PF_MAINLOOP pfMainLoop) noexcept
{
	m_pfMainLoop = pfMainLoop;
}

void JWGUI::Run() noexcept
{
	AssertValidObject();

	HWND hwnd_active{ nullptr };

	bool b_gui_window_destroyed{ false };
	size_t destroyed_gui_window_index{};
	size_t iterator_index{};

	m_IsGUIRunning = true;

	while (m_IsGUIRunning)
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
			m_IsGUIRunning = false;
		}
	}
}

PRIVATE void JWGUI::EraseGUIWindow(size_t index) noexcept
{
	if (m_pGUIWindows.size())
	{
		m_pGUIWindows.erase(index);
	}
}

PRIVATE void JWGUI::ClearGUIWindow() noexcept
{
	if (m_pGUIWindows.size())
	{
		m_pGUIWindows.clear();
	}
}