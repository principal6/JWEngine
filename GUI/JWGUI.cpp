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

	m_pfMainLoop = nullptr;
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
	default:
		return NULL_HANDLE;
		break;
	}
	
	if (JW_FAILED(m_Controls[m_Controls.size() - 1]->Create(Position, Size)))
		return NULL_HANDLE;

	if (Text.length())
	{
		m_Controls[m_Controls.size() - 1]->SetText(Text);
	}

	Result = static_cast<THandle>(m_Controls.size() - 1);
	return Result;
}

auto JWGUI::GetControlPointer(const THandle ControlHandle)->JWControl*
{
	if (ControlHandle == NULL_HANDLE)
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
			iterator->SetState(EControlState::Normal);
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
		iterator->Draw();
	}
}

void JWGUI::SetMainLoopFunction(PF_MAINLOOP pfMainLoop)
{
	m_pfMainLoop = pfMainLoop;
}