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

	// FOR DEBUGGING!!!
	m_bShouldUseViewport = false;

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

auto JWEdit::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError
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

	JWControl::EndDrawing();
}

void JWEdit::SetText(WSTRING Text)
{
	if (!m_bUseMultiline)
	{
		// IF,
		// the text must be single-line,
		// clip the text when the iterator first meets '\n' in the text.

		for (size_t iterator_char = 0; iterator_char < Text.length(); iterator_char++)
		{
			if (Text[iterator_char] == '\n')
			{
				Text = Text.substr(0, iterator_char);
				break;
			}
		}
	}

	JWControl::SetText(Text);

	m_pEditText->UpdateNonInstantText(Text, m_PaddedPosition, m_PaddedSize);
}

void JWEdit::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(Position);

	m_PaddedPosition.x = Position.x + DEFAULT_EDIT_PADDING;
	m_PaddedPosition.y = Position.y + DEFAULT_EDIT_PADDING;
}

void JWEdit::SetSize(D3DXVECTOR2 Size)
{
	// Limit minimum size.
	Size.y = max(Size.y, m_pEditText->GetLineHeight() + DEFAULT_EDIT_PADDING * 2);

	JWControl::SetSize(Size);
	
	m_pBackground->SetSize(Size);

	m_PaddedSize.x = Size.x - DEFAULT_EDIT_PADDING * 2;
	m_PaddedSize.y = Size.y - DEFAULT_EDIT_PADDING * 2;
}

void JWEdit::Focus()
{
	JWControl::Focus();

	m_CaretShowInterval = 0;
}

void JWEdit::ShouldUseMultiline(bool Value)
{
	m_bUseMultiline = Value;
}

void JWEdit::ShouldUseAutomaticLineBreak(bool Value)
{
	m_pEditText->ShouldUseAutomaticLineBreak(Value);
}

PROTECTED void JWEdit::WindowKeyDown(WPARAM VirtualKeyCode)
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
	case VK_DELETE:
		if (m_pEditText->IsTextSelected())
		{
			EraseSelection();
			m_pEditText->MoveCaretToLeft();
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

PROTECTED void JWEdit::WindowCharKeyInput(WPARAM Char)
{
	size_t curr_caret_sel_position = m_pEditText->GetCaretSelPosition();

	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	switch (Char)
	{
	case 8: // Backspace
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
	case 13: // Enter
		if (m_bUseMultiline)
		{
			if (m_pEditText->IsTextSelected())
			{
				EraseSelection();
				m_pEditText->MoveCaretToLeft();
			}

			InsertCharacter(L'\n');
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
				m_pEditText->MoveCaretToLeft();
			}

			InsertCharacter(static_cast<wchar_t>(Char));
		}
		else
		{
			m_bIMEInput = false;
		}
	}
}

PROTECTED void JWEdit::WindowIMEInput(SGUIIMEInputInfo& IMEInfo)
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
			m_pEditText->UpdateNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

			m_bIMECaretCaptured = false;
		}

		m_Text = temp_string;
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
	size_t curr_caret_sel_position = m_pEditText->GetCaretSelPosition();

	m_Text = m_Text.substr(0, curr_caret_sel_position) + Char + m_Text.substr(curr_caret_sel_position);

	m_pEditText->UpdateNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

	m_pEditText->MoveCaretToRight();

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
	
	m_pEditText->UpdateNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

	m_CaretShowInterval = 0;
}

PRIVATE void JWEdit::EraseSelection()
{
	size_t sel_start = m_pEditText->GetSelectionStart();
	size_t sel_end = m_pEditText->GetSelectionEnd();

	m_Text = m_Text.substr(0, sel_start) + m_Text.substr(sel_end);

	m_pEditText->UpdateNonInstantText(m_Text, m_PaddedPosition, m_PaddedSize);

	m_pEditText->MoveCaretTo(sel_start + 1);

	m_CaretShowInterval = 0;
}