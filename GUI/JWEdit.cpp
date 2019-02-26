#include "JWEdit.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWText.h"
#include "JWImageBox.h"

using namespace JWENGINE;

JWEdit::JWEdit()
{
	m_bShouldDrawBorder = true;

	m_pBackground = nullptr;
	m_pEditText = nullptr;

	m_FontColor = DEFAULT_COLOR_FONT;

	m_Watermark = DEFAULT_EDIT_WATERMARK;
	m_WatermarkColor = DEFAULT_COLOR_WATERMARK;

	// Set default color for every control state.
	m_Color_Normal = DEFAULT_COLOR_BACKGROUND_EDIT;
	m_Color_Hover = DEFAULT_COLOR_BACKGROUND_EDIT;
	m_Color_Pressed = DEFAULT_COLOR_BACKGROUND_EDIT;

	m_PaddedPosition = D3DXVECTOR2(0, 0);
	m_PaddedSize = D3DXVECTOR2(0, 0);

	m_CaretShowInterval = 0;

	m_bIMEInput = false;
	m_bIMECaretCaptured = false;

	m_bUseMultiline = false;
}

auto JWEdit::Create(const D3DXVECTOR2 Position, const D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
		return EError::CONTROL_NOT_CREATED;

	// Create a JWImageBox for background.
	if (m_pBackground = new JWImageBox)
	{
		if (JW_FAILED(m_pBackground->Create(Position, Size, pSharedData)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetPosition(Position);
		m_pBackground->SetSize(Size);
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Create non-instant JWText for JWEdit control.
	if (m_pEditText = new JWText)
	{
		if (JW_FAILED(m_pEditText->CreateNonInstantText(m_pSharedData->pWindow, &m_pSharedData->BaseDir, m_pSharedData->pText->GetFontTexturePtr())))
			return EError::TEXT_NOT_CREATED;
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Set control type.
	m_ControlType = EControlType::Edit;

	// Set control's size and position.
	SetPosition(Position);
	SetSize(Size);

	m_pEditText->SetNonInstantText(L"", m_PaddedPosition, m_PaddedSize);

	return EError::OK;
}

void JWEdit::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pEditText);
	
	JW_DESTROY(m_pBackground);
}

void JWEdit::Draw()
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_pBackground->SetBackgroundColor(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		m_pBackground->SetBackgroundColor(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		m_pBackground->SetBackgroundColor(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	m_pBackground->Draw();

	if (!m_Text.length())
	{
		if (m_Watermark.length())
		{
			if (m_bShouldUseViewport)
			{
				m_pSharedData->pWindow->GetDevice()->SetViewport(&m_PaddedViewport);
			}

			m_pSharedData->pText->DrawInstantText(m_Watermark, m_PaddedPosition, EHorizontalAlignment::Left, m_WatermarkColor);
		}
	}

	if (m_bShouldUseViewport)
	{
		m_pSharedData->pWindow->GetDevice()->SetViewport(&m_PaddedViewport);
	}

	m_pEditText->DrawNonInstantText();

	m_pEditText->DrawSelectionBox();

	// Caret blinks.
	if (m_bHasFocus)
	{
		if (m_CaretShowInterval < DEFAULT_CARET_INTERVAL)
		{
			m_pEditText->DrawCaret();
		}
		else
		{
			if (m_CaretShowInterval >= DEFAULT_CARET_INTERVAL * 2)
			{
				m_CaretShowInterval = 0;
			}
		}
		m_CaretShowInterval++;
	}

	if (m_bShouldUseViewport)
	{
		m_pSharedData->pWindow->GetDevice()->SetViewport(&m_ControlViewport);
	}

	JWControl::EndDrawing();
}

auto JWEdit::SetPosition(const D3DXVECTOR2 Position)->JWControl*
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_Position);

	UpdatePaddedViewport();

	return this;
}

auto JWEdit::SetSize(const D3DXVECTOR2 Size)->JWControl*
{
	D3DXVECTOR2 adjusted_size = Size;

	// Limit the minimum size.
	adjusted_size.y = max(adjusted_size.y, m_pEditText->GetLineHeight() + DEFAULT_EDIT_PADDING * 2);

	JWControl::SetSize(adjusted_size);
	
	m_pBackground->SetSize(m_Size);

	UpdatePaddedViewport();

	return this;
}

auto JWEdit::SetText(const WSTRING Text)->JWControl*
{
	WSTRING new_text = Text;

	if (!m_bUseMultiline)
	{
		// IF,
		// the text must be single-line,
		// clip the text when the iterator first meets '\n' in the text.

		for (size_t iterator_char = 0; iterator_char < new_text.length(); iterator_char++)
		{
			if (new_text[iterator_char] == '\n')
			{
				new_text = new_text.substr(0, iterator_char);
				break;
			}
		}
	}

	JWControl::SetText(new_text);

	m_pEditText->SetNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

	return this;
}

auto JWEdit::SetFontColor(const DWORD Color)->JWControl*
{
	m_FontColor = Color;

	m_pEditText->SetNonInstantTextColor(m_FontColor);

	return this;
}

auto JWEdit::SetWatermark(const WSTRING Text)->JWControl*
{
	m_Watermark = Text;

	return this;
}

auto JWEdit::SetWatermarkColor(const DWORD Color)->JWControl*
{
	m_WatermarkColor = Color;

	return this;
}

void JWEdit::Focus()
{
	JWControl::Focus();

	m_CaretShowInterval = 0;
}

auto JWEdit::ShouldUseMultiline(const bool Value) noexcept->JWControl*
{
	m_bUseMultiline = Value;

	if (m_bUseMultiline)
	{
		m_pEditText->ShouldUseAutomaticLineBreak(true);
	}

	return this;
}

auto JWEdit::ShouldUseAutomaticLineBreak(const bool Value) noexcept->JWControl*
{
	m_pEditText->ShouldUseAutomaticLineBreak(Value);

	return this;
}

PROTECTED void JWEdit::WindowMouseDown()
{
	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();
	
	if (Static_IsMouseInRECT(p_input_state->MouseDownPosition, m_ControlRect))
	{
		if (p_input_state->ShiftPressed)
		{
			m_pEditText->SelectTo(m_pEditText->GetMousePressedSelPosition(p_input_state->MouseDownPosition));
		}
		else
		{
			m_pEditText->MoveCaretTo(m_pEditText->GetMousePressedSelPosition(p_input_state->MouseDownPosition));
		}
	}
}

PROTECTED void JWEdit::WindowMouseMove()
{
	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	if (p_input_state->MouseLeftPressed)
	{
		m_pEditText->SelectTo(m_pEditText->GetMousePressedSelPosition(p_input_state->MousePosition));
	}
}

PROTECTED void JWEdit::WindowKeyDown(const WPARAM VirtualKeyCode)
{
	size_t curr_caret_sel_position = m_pEditText->GetCaretSelPosition();

	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	switch (VirtualKeyCode)
	{
	case VK_LEFT:
		if (p_input_state->ShiftPressed)
		{
			m_pEditText->SelectToLeft();
		}
		else
		{
			m_pEditText->MoveCaretToLeft();
			m_pEditText->ReleaseSelection();
		}
		
		m_CaretShowInterval = 0;
		break;
	case VK_RIGHT:
		if (p_input_state->ShiftPressed)
		{
			m_pEditText->SelectToRight();
		}
		else
		{
			m_pEditText->MoveCaretToRight();
			m_pEditText->ReleaseSelection();
		}

		m_CaretShowInterval = 0;
		break;
	case VK_UP:
		if (p_input_state->ShiftPressed)
		{
			m_pEditText->SelectUp();
		}
		else
		{
			m_pEditText->MoveCaretUp();
			m_pEditText->ReleaseSelection();
		}

		m_CaretShowInterval = 0;
		break;
	case VK_DOWN:
		if (p_input_state->ShiftPressed)
		{
			m_pEditText->SelectDown();
		}
		else
		{
			m_pEditText->MoveCaretDown();
			m_pEditText->ReleaseSelection();
		}

		m_CaretShowInterval = 0;
		break;
	case VK_HOME:
		if (p_input_state->ShiftPressed)
		{
			m_pEditText->SelectHome();
		}
		else
		{
			m_pEditText->MoveCaretHome();
			m_pEditText->ReleaseSelection();
		}

		m_CaretShowInterval = 0;
		break;
	case VK_END:
		if (p_input_state->ShiftPressed)
		{
			m_pEditText->SelectEnd();
		}
		else
		{
			m_pEditText->MoveCaretEnd();
			m_pEditText->ReleaseSelection();
		}

		m_CaretShowInterval = 0;
		break;
	case VK_DELETE:
		if (m_pEditText->IsTextSelected())
		{
			EraseSelection();
		}
		else
		{
			EraseCharacter(curr_caret_sel_position + 1);
		}
		break;
	default:
		break;
	}
}

PROTECTED void JWEdit::WindowCharKeyInput(const WPARAM Char)
{
	size_t curr_caret_sel_position = m_pEditText->GetCaretSelPosition();

	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	switch (Char)
	{
	case 1: // Ctrl + a
		if (!m_pEditText->IsTextSelected())
		{
			m_pEditText->SelectAll();
		}
		break;
	case 3: // Ctrl + c
		if (m_pEditText->IsTextSelected())
		{
			CopySelection();
		}
		break;
	case 8: // Backspace && !(Ctrl + h)
		if (p_input_state->ControlPressed)
		{
			break;
		}

		if (m_pEditText->IsTextSelected())
		{
			EraseSelection();
		}
		else
		{
			if (curr_caret_sel_position)
			{
				EraseCharacter(curr_caret_sel_position);

				if (curr_caret_sel_position <= m_Text.length())
				{
					m_pEditText->MoveCaretToLeft();
				}
			}
		}
		
		break;
	case 13: // Enter && !(Ctrl + m)
		if (p_input_state->ControlPressed)
		{
			break;
		}

		if (m_pEditText->IsTextSelected())
		{
			EraseSelection();
		}

		InsertCharacter(L'\n');
		break;
	case 22: // Ctrl + v
		if (m_pEditText->IsTextSelected())
		{
			EraseSelection();
		}
		PasteFromClipboard();
		break;
	case 24: // Ctrl + x
		if (m_pEditText->IsTextSelected())
		{
			CopySelection();
			EraseSelection();
		}
		break;
	default:
		break;
	}

	if (Char >= 32)
	{
		if (!m_bIMEInput)
		{
			if (m_pEditText->IsTextSelected())
			{
				EraseSelection();
			}

			InsertCharacter(static_cast<wchar_t>(Char));
		}
		else
		{
			m_bIMEInput = false;
		}
	}
}

PROTECTED void JWEdit::WindowIMEInput(const SGUIIMEInputInfo& IMEInfo)
{
	if (IMEInfo.bIMEWriting)
	{
		WSTRING temp_string = m_Text;

		if (IMEInfo.IMEWritingChar[0])
		{
			if (!m_bIMECaretCaptured)
			{
				m_IMECapturedCaret = m_pEditText->GetCaretSelPosition();
				m_bIMECaretCaptured = true;
			}

			m_pEditText->MoveCaretTo(m_IMECapturedCaret);

			InsertCharacter(static_cast<wchar_t>(IMEInfo.IMEWritingChar[0]));
		}
		else
		{
			// The character was erased.
			m_pEditText->MoveCaretToLeft();
			m_pEditText->SetNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

			m_bIMECaretCaptured = false;
		}

		m_Text = temp_string;
		m_pEditText->SetNonInstantInnerText(m_Text);
	}

	if (IMEInfo.bIMECompleted)
	{
		m_pEditText->MoveCaretToLeft();

		InsertCharacter(static_cast<wchar_t>(IMEInfo.IMECompletedChar[0]));

		m_bIMEInput = true;
		m_bIMECaretCaptured = false;
	}
}

PRIVATE void JWEdit::InsertCharacter(wchar_t Char)
{
	if (!m_bUseMultiline)
	{
		if (Char == '\n')
		{
			return;
		}
	}

	size_t curr_caret_sel_position = m_pEditText->GetCaretSelPosition();

	m_Text = m_Text.substr(0, curr_caret_sel_position) + Char + m_Text.substr(curr_caret_sel_position);

	WSTRING inserted_string;
	inserted_string += Char;
	m_pEditText->InsertInNonInstantText(inserted_string);

	//m_pEditText->SetNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

	m_pEditText->MoveCaretToRight();

	m_CaretShowInterval = 0;
}

PRIVATE void JWEdit::InsertString(WSTRING String)
{
	size_t curr_caret_sel_position = m_pEditText->GetCaretSelPosition();

	WSTRING string_without_return;
	for (size_t iterator_character = 0; iterator_character < String.length(); iterator_character++)
	{
		if (String[iterator_character] != '\r')
		{
			string_without_return += String[iterator_character];
		}
	}

	if (!m_bUseMultiline)
	{
		for (size_t iterator_character = 0; iterator_character < string_without_return.length(); iterator_character++)
		{
			if (string_without_return[iterator_character] == '\n')
			{
				string_without_return = string_without_return.substr(0, iterator_character);
			}
		}
	}

	m_Text = m_Text.substr(0, curr_caret_sel_position) + string_without_return + m_Text.substr(curr_caret_sel_position);

	m_pEditText->SetNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

	m_pEditText->MoveCaretTo(curr_caret_sel_position + string_without_return.length());

	m_CaretShowInterval = 0;
}

PRIVATE void JWEdit::EraseCharacter(size_t SelPosition)
{
	if (!m_Text.length())
	{
		return;
	}

	if (SelPosition > m_Text.length())
	{
		return;
	}

	m_Text = m_Text.substr(0, SelPosition - 1) + m_Text.substr(SelPosition);
	
	m_pEditText->SetNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

	m_CaretShowInterval = 0;
}

PRIVATE void JWEdit::EraseSelection()
{
	size_t sel_start = m_pEditText->GetSelectionStart();
	size_t sel_end = m_pEditText->GetSelectionEnd();

	m_Text = m_Text.substr(0, sel_start) + m_Text.substr(sel_end);

	m_pEditText->SetNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

	m_pEditText->MoveCaretTo(sel_start);

	m_CaretShowInterval = 0;
}

PRIVATE void JWEdit::CopySelection()
{
	size_t sel_start = m_pEditText->GetSelectionStart();
	size_t sel_end = m_pEditText->GetSelectionEnd();

	WSTRING copied_text = m_Text.substr(sel_start, sel_end - sel_start);

	WSTRING string_with_return;
	for (size_t iterator_character = 0; iterator_character < copied_text.length(); iterator_character++)
	{
		if (copied_text[iterator_character] == '\n')
		{
			string_with_return += '\r';
		}

		string_with_return += copied_text[iterator_character];
	}

	size_t copied_length = string_with_return.length();
	
	HGLOBAL h_memory = GlobalAlloc(GMEM_MOVEABLE, (copied_length + 1) * sizeof(TCHAR));

	LPTSTR lptstrCopy = (LPTSTR)(GlobalLock(h_memory));
		memcpy(lptstrCopy, &string_with_return.c_str()[0], copied_length * sizeof(TCHAR));
		lptstrCopy[copied_length] = (TCHAR)0;
	GlobalUnlock(h_memory);

	if (OpenClipboard(m_pSharedData->pWindow->GethWnd()))
	{
		EmptyClipboard();

		SetClipboardData(CF_UNICODETEXT, h_memory);

		CloseClipboard();
	}
	
	//GlobalFree(h_memory);
}

PRIVATE void JWEdit::PasteFromClipboard()
{
	if (OpenClipboard(m_pSharedData->pWindow->GethWnd()))
	{
		HGLOBAL h_global = nullptr;
		LPTSTR string = nullptr;

		if (h_global = GetClipboardData(CF_UNICODETEXT))
		{
			if (string = (LPTSTR)GlobalLock(h_global))
			{
				GlobalUnlock(h_global);

				InsertString(string);
			}
		}

		CloseClipboard();
	}
}

PRIVATE void JWEdit::UpdatePaddedViewport()
{
	m_PaddedPosition.x = m_Position.x + DEFAULT_EDIT_PADDING;
	m_PaddedPosition.y = m_Position.y + DEFAULT_EDIT_PADDING;

	m_PaddedSize.x = m_Size.x - DEFAULT_EDIT_PADDING * 2;
	m_PaddedSize.y = m_Size.y - DEFAULT_EDIT_PADDING * 2;

	m_PaddedViewport = m_ControlViewport;
	m_PaddedViewport.X = static_cast<DWORD>(m_PaddedPosition.x);
	m_PaddedViewport.Y = static_cast<DWORD>(m_PaddedPosition.y);
	m_PaddedViewport.Width = static_cast<DWORD>(m_PaddedSize.x);
	m_PaddedViewport.Height = static_cast<DWORD>(m_PaddedSize.y);
}