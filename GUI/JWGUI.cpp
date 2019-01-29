#include "JWGUI.h"

using namespace JWENGINE;

JWGUI::JWGUI()
{
	m_pWindow = nullptr;
	m_pControlWithFocus = nullptr;
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

	std::wcout << m_BaseDir.c_str() << std::endl;

	AddControl(EControlType::Button, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"ABCDE");

	AddControl(EControlType::Label, D3DXVECTOR2(100, 80), D3DXVECTOR2(150, 50), L"레이블입니다");

	AddControl(EControlType::Edit, D3DXVECTOR2(100, 120), D3DXVECTOR2(150, 60), L"이번엔abcd말고\n한글 ttTT!!ㅋ\nThe third line");

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

			HandleMessage();
		}
		else
		{
			MainLoop();
		}
	}

	Destroy();
}

auto JWGUI::AddControl(EControlType Type, D3DXVECTOR2 Position, D3DXVECTOR2 Size, WSTRING Text)->EError
{
	switch (Type)
	{
	case JWENGINE::Button:
		m_Controls.push_back(new JWButton);
		break;
	case JWENGINE::Label:
		m_Controls.push_back(new JWLabel);
		break;
	case JWENGINE::Edit:
		m_Controls.push_back(new JWEdit);
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
				//pControlWithMouse->SetPosition(pControlWithMouse->GetPosition() + D3DXVECTOR2(2, 2));
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
		switch (m_MSG.message)
		{
		case WM_KEYDOWN:
			m_pControlWithFocus->OnKeyDown(m_MSG.wParam);
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