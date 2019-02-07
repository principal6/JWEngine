#include "JWGUI.h"

using namespace JWENGINE;

static auto CreateTexture(const WSTRING& Filename, LPDIRECT3DDEVICE9 pDevice,
	LPDIRECT3DTEXTURE9* pTexture, D3DXIMAGE_INFO* pInfo)->EError
{
	if (FAILED(D3DXCreateTextureFromFileEx(pDevice, Filename.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, pInfo, nullptr, pTexture)))
		return EError::TEXTURE_NOT_CREATED;

	return EError::OK;
}

JWGUI::JWGUI()
{
	m_pControlWithFocus = nullptr;
	m_pMenuBar = nullptr;

	m_pfMainLoop = nullptr;

	m_bHasMenuBar = false;
}

auto JWGUI::Create(JWWindow* pWindow)->EError
{
	if (nullptr == (m_SharedData.pWindow = pWindow))
		return EError::NULLPTR_WINDOW;

	// Set base directory
	wchar_t tempDir[MAX_FILE_LEN]{};
	GetCurrentDirectoryW(MAX_FILE_LEN, tempDir);
	m_SharedData.BaseDir = tempDir;
	m_SharedData.BaseDir = m_SharedData.BaseDir.substr(0, m_SharedData.BaseDir.find(PROJECT_FOLDER));

	// Output base directory in console window
	std::wcout << m_SharedData.BaseDir.c_str() << std::endl;

	// Create shared texture
	WSTRING texture_filename;
	texture_filename = m_SharedData.BaseDir;
	texture_filename += ASSET_DIR;
	texture_filename += GUI_TEXTURE_FILENAME;
	if (JW_FAILED(CreateTexture(texture_filename, m_SharedData.pWindow->GetDevice(),
		&m_SharedData.Texture_GUI, &m_SharedData.Texture_GUI_Info)))
		return EError::TEXTURE_NOT_CREATED;

	JWControl temp_control_to_set_shared_data;
	temp_control_to_set_shared_data.SetSharedData(&m_SharedData);

	return EError::OK;
}

void JWGUI::Destroy()
{
	for (JWControl* iterator : m_Controls)
	{
		JW_DESTROY(iterator);
	}
	m_Controls.clear();
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

auto JWGUI::AddControl(EControlType Type, D3DXVECTOR2 Position, D3DXVECTOR2 Size, WSTRING Text)->const THandle
{
	THandle Result = 0;

	if (m_bHasMenuBar)
	{
		if (Type == EControlType::MenuBar)
		{
			// If JWGUI already has a menubar, you can't add another one.
			return THandle_Null;
		}
		else
		{
			// Adjust y position of every other control.
			Position.y += m_pMenuBar->GetSize().y;
		}
	}

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
	case JWENGINE::ListBox:
		m_Controls.push_back(new JWListBox);
		break;
	case JWENGINE::MenuBar:
		m_Controls.push_back(new JWMenuBar);
		m_bHasMenuBar = true;
		m_pMenuBar = m_Controls[m_Controls.size() - 1];
		break;
	default:
		return THandle_Null;
		break;
	}
	
	if (JW_FAILED(m_Controls[m_Controls.size() - 1]->Create(Position, Size)))
		return THandle_Null;

	if (Text.length())
	{
		m_Controls[m_Controls.size() - 1]->SetText(Text);
	}

	Result = static_cast<THandle>(m_Controls.size() - 1);
	return Result;
}

auto JWGUI::GetControlPtr(const THandle ControlHandle)->JWControl*
{
	if (ControlHandle == THandle_Null)
	{
		return nullptr;
	}
	else
	{
		return m_Controls[ControlHandle];
	}
}

PRIVATE void JWGUI::HandleMessage()
{
	m_MouseData.MouseWheeled = 0;

	switch (m_MSG.message)
	{
	case WM_MOUSEMOVE:
		m_MouseData.MousePosition.x = GET_X_LPARAM(m_MSG.lParam);
		m_MouseData.MousePosition.y = GET_Y_LPARAM(m_MSG.lParam);
		break;
	case WM_LBUTTONDOWN:
		m_MouseData.MouseDownPosition.x = GET_X_LPARAM(m_MSG.lParam);
		m_MouseData.MouseDownPosition.y = GET_Y_LPARAM(m_MSG.lParam);
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEWHEEL:
		m_MouseData.MouseWheeled = static_cast<__int16>(HIWORD(m_MSG.wParam));
		break;
	default:
		break;
	}
}

PRIVATE void JWGUI::MainLoop()
{
	m_SharedData.pWindow->UpdateInputState();

	m_SharedData.pWindow->BeginRender();

	JWControl* pControlWithMouse = nullptr;
	JWControl* pControlWithNewFocus = nullptr;

	if (m_pMenuBar)
	{
		if (m_pMenuBar->IsMouseOver(m_MouseData))
		{
			pControlWithMouse = m_pMenuBar;

			for (JWControl* iterator : m_Controls)
			{
				if (iterator != m_pMenuBar)
				{
					iterator->SetState(EControlState::Normal);
				}
			}
		}
		else
		{
			m_pMenuBar->UpdateControlState(m_MouseData);

			for (JWControl* iterator : m_Controls)
			{
				if (iterator != m_pMenuBar)
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
						iterator->UpdateControlState(m_MouseData); // To give mouse data to the controls
					}
				}
			}
		}
	}
	else
	{
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
				iterator->UpdateControlState(m_MouseData); // To give mouse data to the controls
			}
		}
	}
	

	// Mouse cursor is on a control
	if (pControlWithMouse)
	{
		pControlWithMouse->UpdateControlState(m_MouseData);

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
		m_pControlWithFocus->CheckIMEInput();

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
		// Call the outter main loop.
		m_pfMainLoop();
	}
	else
	{
		// If the main loop function is not asigned,
		// just draw all the controls.
		DrawAllControls();
	}

	m_SharedData.pWindow->EndRender();
}

void JWGUI::DrawAllControls()
{
	for (JWControl* iterator : m_Controls)
	{
		if (iterator != m_pMenuBar)
		{
			iterator->Draw();
		}
	}

	// If there is a menu bar, it should be drawn on top of any other controls.
	if (m_pMenuBar)
	{
		m_pMenuBar->Draw();
	}
}

void JWGUI::SetMainLoopFunction(PF_MAINLOOP pfMainLoop)
{
	m_pfMainLoop = pfMainLoop;
}