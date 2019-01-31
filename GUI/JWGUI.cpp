#include "JWGUI.h"

using namespace JWENGINE;

JWGUI::JWGUI()
{
	m_pWindow = nullptr;
	m_pControlWithFocus = nullptr;

	m_pfMainLoop = nullptr;
}

auto JWGUI::Create(JWWindow* pWindow)->EError
{
	if (nullptr == (m_pWindow = pWindow))
		return EError::NULLPTR_WINDOW;

	// Set base directory
	wchar_t tempDir[MAX_FILE_LEN]{};
	GetCurrentDirectoryW(MAX_FILE_LEN, tempDir);
	m_BaseDir = tempDir;
	m_BaseDir = m_BaseDir.substr(0, m_BaseDir.find(PROJECT_FOLDER));

	// Output base directory in console window
	std::wcout << m_BaseDir.c_str() << std::endl;

	return EError::OK;
}

void JWGUI::Destroy()
{
	for (JWControl* iterator : m_Controls)
	{
		JW_DESTROY(iterator);
	}
	m_Controls.clear();
	
	m_pWindow = nullptr;
}

void JWGUI::Run()
{
	while (m_MSG.message != WM_QUIT)
	{
		if (PeekMessage(&m_MSG, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&m_MSG);
			DispatchMessage(&m_MSG);
		}
		else
		{
			HandleMessage();

			MainLoop();

			// @Warning:
			// We must empty m_MSG to avoid duplicate messages!
			memset(&m_MSG, 0, sizeof(m_MSG));
		}
	}

	Destroy();
}

auto JWGUI::AddControl(EControlType Type, D3DXVECTOR2 Position, D3DXVECTOR2 Size, WSTRING Text)->EError
{
	switch (Type)
	{
	case JWENGINE::TextButton:
		m_Controls.push_back(new JWTextButton);
		break;
	case JWENGINE::ImageButton:
		m_Controls.push_back(new JWImageButton);
		break;
	case JWENGINE::Label:
		m_Controls.push_back(new JWLabel);
		break;
	case JWENGINE::Edit:
		m_Controls.push_back(new JWEdit);
		break;
	case JWENGINE::CheckBox:
		m_Controls.push_back(new JWCheckBox);
		break;
	case JWENGINE::RadioBox:
		m_Controls.push_back(new JWRadioBox);
		break;
	case JWENGINE::ScrollBar:
		m_Controls.push_back(new JWScrollBar);
		break;
	default:
		return EError::INVALID_CONTROL_TYPE;
		break;
	}
	
	if (JW_FAILED(m_Controls[m_Controls.size() - 1]->Create(m_pWindow, m_BaseDir, Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	if (Text.length())
	{
		m_Controls[m_Controls.size() - 1]->SetText(Text);
	}

	return EError::OK;
}

auto JWGUI::GetControlPointer(size_t ControlIndex)->JWControl*
{
	ControlIndex = min(ControlIndex, m_Controls.size());

	return m_Controls[ControlIndex];
}

PRIVATE void JWGUI::HandleMessage()
{
	switch (m_MSG.message)
	{
	case WM_MOUSEMOVE:
		m_MouseData.MousePosition.x = GET_X_LPARAM(m_MSG.lParam);
		m_MouseData.MousePosition.y = GET_Y_LPARAM(m_MSG.lParam);
		break;
	case WM_LBUTTONDOWN:
		m_MouseData.MouseDownPosition.x = GET_X_LPARAM(m_MSG.lParam);
		m_MouseData.MouseDownPosition.y = GET_Y_LPARAM(m_MSG.lParam);
		m_MouseData.bMouseLeftButtonPressed = true;
		break;
	case WM_LBUTTONUP:
		m_MouseData.bMouseLeftButtonPressed = false;
		break;
	default:
		break;
	}
}

PRIVATE void JWGUI::MainLoop()
{
	m_pWindow->BeginRender();

	JWControl* pControlWithMouse = nullptr;
	JWControl* pControlWithNewFocus = nullptr;

	for (JWControl* iterator : m_Controls)
	{
		// A control that has mouse pointer on must be only one

		if (iterator->IsMouseOver(m_MouseData))
		{
			if (pControlWithMouse)
			{
				pControlWithMouse->SetState(EControlState::Normal);
			}
			pControlWithMouse = iterator;
		}
		else
		{
			iterator->UpdateState(m_MouseData); // To give mouse data to the controls
			iterator->SetState(EControlState::Normal);
		}
	}

	// Mouse cursor is on a control
	if (pControlWithMouse)
	{
		pControlWithMouse->UpdateState(m_MouseData);

		if ((pControlWithMouse->GetState() == EControlState::Clicked) || (pControlWithMouse->GetState() == EControlState::Pressed))
		{
			pControlWithNewFocus = pControlWithMouse;

			if (pControlWithMouse->GetState() == EControlState::Clicked)
			{
				// RadioBox Check!
				if (pControlWithMouse->GetControlType() == EControlType::RadioBox)
				{
					for (JWControl* iterator : m_Controls)
					{
						if (iterator->GetControlType() == EControlType::RadioBox)
						{
							iterator->SetCheckState(false);
						}
					}

					pControlWithMouse->SetCheckState(true);
				}
			}
		}
	}


	// TODO: Tab stop needed! (keyboard Tab key)


	// Set focus on pControlWithFocus (because the focus is changed)
	// and kill focus of all the other controls
	if (pControlWithNewFocus)
	{
		// New focus event
		if (m_pControlWithFocus != pControlWithNewFocus)
		{
			// Focus has been moved to another control
			m_pControlWithFocus = pControlWithNewFocus;
			m_pControlWithFocus->Focus();
			for (JWControl* iterator : m_Controls)
			{
				if (iterator != m_pControlWithFocus)
					iterator->KillFocus();
			}
		}
	}

	// Call event handlers of the control with focus
	if (m_pControlWithFocus)
	{
		m_pControlWithFocus->CheckIME();

		switch (m_MSG.message)
		{
		case WM_KEYDOWN:
			m_pControlWithFocus->OnKeyDown(m_MSG.wParam);
			break;
		case WM_CHAR:
			m_pControlWithFocus->OnCharKey(m_MSG.wParam);
			break;
		case WM_KEYUP:
			m_pControlWithFocus->OnKeyUp(m_MSG.wParam);
			break;
		case WM_MOUSEMOVE:
			m_pControlWithFocus->OnMouseMove(m_MSG.lParam);
			break;
		case WM_LBUTTONDOWN:
			m_pControlWithFocus->OnMouseDown(m_MSG.lParam);
			break;
		case WM_LBUTTONUP:
			m_pControlWithFocus->OnMouseUp(m_MSG.lParam);
			break;
		}
	}

	if (m_pfMainLoop)
	{
		m_pfMainLoop();
	}

	Draw();

	m_pWindow->EndRender();
}

PRIVATE void JWGUI::Draw()
{
	for (JWControl* iterator : m_Controls)
	{
		iterator->Draw();
	}
}

void JWGUI::SetMainLoopFunction(PF_MAINLOOP pfMainLoop)
{
	m_pfMainLoop = pfMainLoop;
}