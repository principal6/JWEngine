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

PRIVATE auto JWGUIWindow::CreateTexture(const WSTRING& Filename, LPDIRECT3DTEXTURE9* ppTexture, D3DXIMAGE_INFO* pInfo)->EError
{
	WSTRING texture_filename;
	texture_filename = m_SharedData.BaseDir;
	texture_filename += ASSET_DIR;
	texture_filename += Filename;

	if (FAILED(D3DXCreateTextureFromFileEx(m_SharedData.pWindow->GetDevice(), texture_filename.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, pInfo, nullptr, ppTexture)))
		return EError::TEXTURE_NOT_CREATED;

	return EError::OK;
}

void JWGUIWindow::Destroy()
{
	if (m_pControls.size())
	{
		for (JWControl* iterator : m_pControls)
		{
			JW_DESTROY(iterator);
		}

		m_pControls.clear();
	}
	
	JW_DESTROY(m_SharedData.pText);
	JW_RELEASE(m_SharedData.Texture_GUI);
	JW_DESTROY(m_SharedData.pWindow);
}

auto JWGUIWindow::AddControl(const WSTRING ControlName, const EControlType Type, const D3DXVECTOR2 Position, const D3DXVECTOR2 Size,
	const WSTRING Text)->JWControl*
{
	D3DXVECTOR2 adjusted_position = Position;

	// If this GUIWindow has a MenuBar, we need to offset all the other controls' position.
	if (m_bHasMenuBar)
	{
		if (Type == EControlType::MenuBar)
		{
			// If JWGUIWindow already has a menubar, you can't add another one.
			return nullptr;
		}
		else
		{
			// Adjust y position of every other control.
			adjusted_position.y += m_pMenuBar->GetSize().y;
		}
	}

	switch (Type)
	{
	case JWENGINE::TextButton:
		m_pControls.push_back(new JWTextButton);
		break;
	case JWENGINE::ImageButton:
		m_pControls.push_back(new JWImageButton);
		break;
	case JWENGINE::Label:
		m_pControls.push_back(new JWLabel);
		break;
	case JWENGINE::Edit:
		m_pControls.push_back(new JWEdit);
		break;
	case JWENGINE::CheckBox:
		m_pControls.push_back(new JWCheckBox);
		break;
	case JWENGINE::RadioBox:
		m_pControls.push_back(new JWRadioBox);
		break;
	case JWENGINE::ScrollBar:
		m_pControls.push_back(new JWScrollBar);
		break;
	case JWENGINE::ListBox:
		m_pControls.push_back(new JWListBox);
		break;
	case JWENGINE::MenuBar:
		m_pControls.push_back(new JWMenuBar);
		m_bHasMenuBar = true;
		m_pMenuBar = m_pControls[m_pControls.size() - 1];
		break;
	case JWENGINE::ImageBox:
		m_pControls.push_back(new JWImageBox);
		break;
	default:
		return nullptr;
	}

	if (JW_FAILED(m_pControls[m_pControls.size() - 1]->Create(adjusted_position, Size, &m_SharedData)))
	{
		JW_DESTROY(m_pControls[m_pControls.size() - 1]);
		m_pControls.pop_back();

		return nullptr;
	}

	if (Text.length())
	{
		m_pControls[m_pControls.size() - 1]->SetText(Text);
	}

	// Control is successfully added.
	m_ControlsMap.insert(MAKE_PAIR(ControlName, m_pControls.size() - 1));

	return m_pControls[m_pControls.size() - 1];
}

auto JWGUIWindow::GetControlPtr(const WSTRING ControlName)->JWControl*
{
	auto found_control = m_ControlsMap.find(ControlName);

	if (found_control != m_ControlsMap.end())
	{
		return m_pControls[found_control->second];
	}
	else
	{
		return nullptr;
	}
}

void JWGUIWindow::Update(const MSG& Message, const SGUIIMEInputInfo& IMEInfo, const HWND QuitWindowHWND, const HWND ActiveWindowHWND)
{
	// Update JWWindow's input state.
	m_SharedData.pWindow->UpdateWindowInputState(Message);

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

		KillAllFocus();

		return;
	}

	JWControl* p_control_with_mouse = nullptr;

	// We must update control's state in the opposite order from the creation order,
	// because lately added controls' position is on top of formerly added controls.
	// @warning: MenuBar must be the first one to be updated, if it exists,
	// becuase it's on top of any other controls.
	if (m_pControls.size())
	{
		if (m_pMenuBar)
		{
			m_pMenuBar->UpdateControlState(&p_control_with_mouse, &m_pControlWithFocus);
		}

		for (size_t iterator_index = m_pControls.size(); iterator_index > 0; iterator_index--)
		{
			// In order not to do duplicate update.
			if (m_pControls[iterator_index - 1] != m_pMenuBar)
			{
				m_pControls[iterator_index - 1]->UpdateControlState(&p_control_with_mouse, &m_pControlWithFocus);
			}
		}
	}

	// TODO: Tab stop needed! (keyboard Tab key)

	if (m_pControlWithFocus)
	{		
		//std::cout << "[DEBUG] Control with focus: " << m_pControlWithFocus->GetControlType() << " / " << m_pControlWithFocus << std::endl;

		if (m_pControlWithFocus->GetControlType() == EControlType::RadioBox)
		{
			for (JWControl* iterator : m_pControls)
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

	if (m_pControls.size())
	{
		pFocusedControl->Focus();

		for (JWControl* iterator : m_pControls)
		{
			if (iterator != pFocusedControl)
			{
				iterator->KillFocus();
			}
		}
	}
}

void JWGUIWindow::BeginRender() const
{
	m_SharedData.pWindow->BeginRender();
}

void JWGUIWindow::DrawAllControls()
{
	if (m_pControls.size())
	{
		for (JWControl* iterator : m_pControls)
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

auto JWGUIWindow::IsDestroyed() const->const bool
{
	return m_bDestroyed;
}

void JWGUIWindow::KillAllFocus()
{
	m_pControlWithFocus = nullptr;

	if (m_pControls.size())
	{
		for (JWControl* iterator : m_pControls)
		{
			iterator->KillFocus();
		}
	}
}