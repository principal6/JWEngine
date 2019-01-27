#include "JWGUI.h"

using namespace JWENGINE;

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
	AddControl(EControlType::Button, D3DXVECTOR2(20, 0), D3DXVECTOR2(100, 50), L"가나다라");
	AddControl(EControlType::Button, D3DXVECTOR2(40, 0), D3DXVECTOR2(100, 50), L"가나다라");
	AddControl(EControlType::Button, D3DXVECTOR2(60, 0), D3DXVECTOR2(100, 50), L"가나다라");
	AddControl(EControlType::Button, D3DXVECTOR2(80, 0), D3DXVECTOR2(100, 50), L"가나다라");
	AddControl(EControlType::Button, D3DXVECTOR2(100, 0), D3DXVECTOR2(100, 50), L"가나다라");

	AddControl(EControlType::Label, D3DXVECTOR2(100, 80), D3DXVECTOR2(150, 50), L"레이블입니다");

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
		m_MouseData.MouseUpPosition.x = GET_X_LPARAM(m_MSG.lParam);
		m_MouseData.MouseUpPosition.y = GET_Y_LPARAM(m_MSG.lParam);
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

	if (pControlWithMouse)
	{
		pControlWithMouse->UpdateState(m_MouseData);

		if ((pControlWithMouse->GetState() == EControlState::Clicked) || (pControlWithMouse->GetState() == EControlState::Pressed))
		{
			if (pControlWithMouse->GetState() == EControlState::Clicked)
			{
				pControlWithMouse->SetPosition(pControlWithMouse->GetPosition() + D3DXVECTOR2(2, 2));
			}
			
			// Set focus of all controls
			pControlWithMouse->Focus();
			for (JWControl* iterator : m_Controls)
			{
				if (iterator != pControlWithMouse)
					iterator->KillFocus();
			}
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