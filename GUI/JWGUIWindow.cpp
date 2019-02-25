#include "JWGUIWindow.h"

using namespace JWENGINE;

JWGUIWindow::JWGUIWindow()
{
	m_pMenuBar = nullptr;
	m_bHasMenuBar = false;

	m_pControlWithFocus = nullptr;

	m_bDestroyed = false;
}

auto JWGUIWindow::Create(const SWindowCreationData& WindowCreationData)->EError
{
	// Create JWWindow(win32 api window).
	if (!m_SharedData.pWindow)
	{
		if (m_SharedData.pWindow = new JWWindow)
		{
			if (JW_FAILED(m_SharedData.pWindow->CreateGUIWindow(WindowCreationData)))
			{
				return EError::WINAPIWINDOW_NOT_CREATED;
			}
		}
		else
		{
			return EError::ALLOCATION_FAILURE;
		}
	}

	// Set base directory
	wchar_t tempDir[MAX_FILE_LEN]{};
	GetCurrentDirectoryW(MAX_FILE_LEN, tempDir);
	m_SharedData.BaseDir = tempDir;
	m_SharedData.BaseDir = m_SharedData.BaseDir.substr(0, m_SharedData.BaseDir.find(PROJECT_FOLDER));

	// For debugging
	// Print base directory in console window
	//std::wcout << m_SharedData.BaseDir.c_str() << std::endl;

	// Create GUI texture that will be shared in every control.
	if (JW_FAILED(CreateTexture(GUI_TEXTURE_FILENAME, &m_SharedData.Texture_GUI, &m_SharedData.Texture_GUI_Info)))
	{
		return EError::TEXTURE_NOT_CREATED;
	}

	// Create JWFont object that will be shared in every control.
	if (m_SharedData.pText = new JWText)
	{
		if (JW_FAILED(m_SharedData.pText->CreateInstantText(m_SharedData.pWindow, &m_SharedData.BaseDir)))
		{
			return EError::TEXT_NOT_CREATED;
		}
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	return EError::OK;
}

PRIVATE auto JWGUIWindow::CreateTexture(const WSTRING& Filename, LPDIRECT3DTEXTURE9* pTexture, D3DXIMAGE_INFO* pInfo)->EError
{
	WSTRING texture_filename;
	texture_filename = m_SharedData.BaseDir;
	texture_filename += ASSET_DIR;
	texture_filename += Filename;

	if (FAILED(D3DXCreateTextureFromFileEx(m_SharedData.pWindow->GetDevice(), texture_filename.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, pInfo, nullptr, pTexture)))
		return EError::TEXTURE_NOT_CREATED;

	return EError::OK;
}

void JWGUIWindow::Destroy()
{
	if (m_Controls.size())
	{
		for (JWControl* iterator : m_Controls)
		{
			JW_DESTROY(iterator);
		}

		m_Controls.clear();
	}
	
	JW_DESTROY(m_SharedData.pText);
	JW_RELEASE(m_SharedData.Texture_GUI);
	JW_DESTROY(m_SharedData.pWindow);
}

auto JWGUIWindow::AddControl(EControlType Type, D3DXVECTOR2 Position, D3DXVECTOR2 Size, WSTRING Text)->const THandle
{
	THandle Result = 0;

	if (m_bHasMenuBar)
	{
		if (Type == EControlType::MenuBar)
		{
			// If JWGUIWindow already has a menubar, you can't add another one.
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
	case JWENGINE::ImageBox:
		m_Controls.push_back(new JWImageBox);
		break;
	default:
		return THandle_Null;
		break;
	}

	if (JW_FAILED(m_Controls[m_Controls.size() - 1]->Create(Position, Size, &m_SharedData)))
	{
		return THandle_Null;
	}

	if (Text.length())
	{
		m_Controls[m_Controls.size() - 1]->SetText(Text);
	}

	Result = static_cast<THandle>(m_Controls.size() - 1);
	return Result;
}

auto JWGUIWindow::GetControlPtr(const THandle ControlHandle)->JWControl*
{
	if (ControlHandle == THandle_Null)
	{
		return nullptr;
	}
	else
	{
		size_t safe_index = ControlHandle;
		safe_index = min(safe_index, m_Controls.size() - 1);

		return m_Controls[safe_index];
	}
}

void JWGUIWindow::Update(MSG& Message, SGUIIMEInputInfo& IMEInfo, HWND QuitWindowHWND, HWND ActiveWindowHWND)
{
	// Update JWWindow's input state.
	m_SharedData.pWindow->UpdateInputState(Message);

	if (QuitWindowHWND == m_SharedData.pWindow->GethWnd())
	{
		// IF,
		// win32-api-window is destroyed,
		// so JWGUIWindow must do!

		m_bDestroyed = true;

		return;
	}

	if (ActiveWindowHWND != m_SharedData.pWindow->GethWnd())
	{
		// IF,
		// this JWGUIWindow is not an active window,
		// kill the focus of controls.

		KillFocus();

		return;
	}

	JWControl* p_control_with_mouse = nullptr;

	// We must update control's state in the opposite order from the creation order,
	// because lately added controls' position is on top of formerly added controls.
	// @warning: MenuBar must be the first one to be updated, if it exists,
	// becuase it's on top of any other controls.
	if (m_Controls.size())
	{
		if (m_pMenuBar)
		{
			m_pMenuBar->UpdateControlState(&p_control_with_mouse, &m_pControlWithFocus);
		}

		for (size_t iterator_index = m_Controls.size(); iterator_index > 0; iterator_index--)
		{
			// In order not to do duplicate update.
			if (m_Controls[iterator_index - 1] != m_pMenuBar)
			{
				m_Controls[iterator_index - 1]->UpdateControlState(&p_control_with_mouse, &m_pControlWithFocus);
			}
		}
	}

	// TODO: Tab stop needed! (keyboard Tab key)

	if (m_pControlWithFocus)
	{		
		//std::cout << "[DEBUG] Control with focus: " << m_pControlWithFocus->GetControlType() << " / " << m_pControlWithFocus << std::endl;

		if (m_pControlWithFocus->GetControlType() == EControlType::RadioBox)
		{
			for (JWControl* iterator : m_Controls)
			{
				if (iterator->GetControlType() == EControlType::RadioBox)
				{
					iterator->SetCheckState(false);
				}
			}

			m_pControlWithFocus->SetCheckState(true);
		}

		// Call event handlers of the control with focus
		m_pControlWithFocus->WindowIMEInput(IMEInfo);

		// Update only when HWND of the message matches this window's HWND.
		if (Message.hwnd == m_SharedData.pWindow->GethWnd())
		{
			switch (Message.message)
			{
			case WM_LBUTTONDOWN:
				m_pControlWithFocus->WindowMouseDown();
				break;
			case WM_MOUSEMOVE:
				m_pControlWithFocus->WindowMouseMove();
				break;
			case WM_KEYDOWN:
				m_pControlWithFocus->WindowKeyDown(Message.wParam);
				break;
			case WM_CHAR:
				m_pControlWithFocus->WindowCharKeyInput(Message.wParam);
				break;
			case WM_KEYUP:
				m_pControlWithFocus->WindowKeyUp(Message.wParam);
				break;
			}
		}
	}
}

PRIVATE void JWGUIWindow::SetFocusOnControl(JWControl* pFocusedControl)
{
	if (!pFocusedControl)
	{
		return;
	}

	if (m_Controls.size())
	{
		pFocusedControl->Focus();

		for (JWControl* iterator : m_Controls)
		{
			if (iterator != pFocusedControl)
			{
				iterator->KillFocus();
			}
		}
	}
}

void JWGUIWindow::BeginRender()
{
	m_SharedData.pWindow->BeginRender();
}

void JWGUIWindow::DrawAllControls()
{
	if (m_Controls.size())
	{
		for (JWControl* iterator : m_Controls)
		{
			if (iterator != m_pMenuBar)
			{
				iterator->Draw();
			}
		}
	}

	// If there is a menu bar, it should be drawn on top of any other controls.
	if (m_pMenuBar)
	{
		m_pMenuBar->Draw();
	}
}

void JWGUIWindow::EndRender()
{
	m_SharedData.pWindow->EndRender();
}

auto JWGUIWindow::IsDestroyed()->bool
{
	return m_bDestroyed;
}

void JWGUIWindow::KillFocus()
{
	m_pControlWithFocus = nullptr;

	if (m_Controls.size())
	{
		for (JWControl* iterator : m_Controls)
		{
			iterator->KillFocus();
		}
	}
}