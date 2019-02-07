#include "JWEdit.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWRectangle.h"

using namespace JWENGINE;

// Static constant
const float JWEdit::EDIT_PADDING_X = 2.0f;
const float JWEdit::EDIT_PADDING_Y = 3.0f;

JWEdit::JWEdit()
{
	// An edit would normally have its border.
	m_bShouldDrawBorder = true;

	m_pCaret = nullptr;
	m_CaretSize = D3DXVECTOR2(0, 0);
	m_CaretPosition = D3DXVECTOR2(0, 0);
	m_pCaretSelPosition = &m_SelStart;
	m_PreviousCaretSelPosition = 0;

	m_pSelection = nullptr;
	m_SelStart = 0;
	m_SelEnd = 0;
	m_pCapturedSelPosition = &m_SelStart;
	
	m_IMETempSel = 0;
	m_CaretTickCount = 0;

	m_bUseMultiline = false;
	m_YSizeMultiline = 0;
	m_YSizeSingleline = 0;
}

auto JWEdit::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;
	
	// Create line for caret
	if (m_pCaret = new JWLine)
	{
		if (JW_FAILED(m_pCaret->Create(ms_pSharedData->pWindow->GetDevice())))
			return EError::LINE_NOT_CREATED;

		m_pCaret->AddLine(D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 10), DEFAULT_COLOR_CARET);
		m_pCaret->AddEnd();
	}
	else
	{
		return EError::LINE_NOT_CREATED;
	}

	// Create rectangle for selection
	if (m_pSelection = new JWRectangle)
	{
		if (JW_FAILED(m_pSelection->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir, m_pFont->GetMaximumLineCount())))
			return EError::RECTANGLE_NOT_CREATED;

		m_pSelection->SetRectangleAlpha(100);
		m_pSelection->SetRectangleXRGB(DEFAULT_COLOR_SELECTION);
	}
	else
	{
		return EError::RECTANGLE_NOT_CREATED;
	}

	// Create image for background
	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
			return EError::RECTANGLE_NOT_CREATED;

		m_pBackground->SetAlpha(DEFAULT_ALPHA_BACKGROUND_EDIT);
		m_pBackground->SetXRGB(DEFAULT_COLOR_BACKGROUND_EDIT);
	}
	else
	{
		return EError::RECTANGLE_NOT_CREATED;
	}

	// Set default color
	m_pFont->SetFontColor(DEFAULT_COLOR_FONT_EDIT);
	m_pFont->SetBoxColor(GetMixedColor(0, DEFAULT_COLOR_NORMAL));

	// Set borderline color
	m_pBorderLine->SetBoxColor(DEFAULT_COLOR_PRESSED, DEFAULT_COLOR_DARK_HIGHLIGHT);

	// Set multiline and singleline y size
	m_YSizeMultiline = Size.y;
	m_YSizeSingleline = m_pFont->GetLineHeight() + EDIT_PADDING_Y * 2;

	// Set caret size
	m_CaretSize = D3DXVECTOR2(0, m_pFont->GetLineHeight());

	// Set control type
	m_Type = EControlType::Edit;

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

	return EError::OK;
}

void JWEdit::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
	JW_DESTROY(m_pCaret);
	JW_DESTROY(m_pSelection);
}

void JWEdit::Draw()
{
	JWControl::BeginDrawing();

	m_pBackground->Draw();

	m_pFont->Draw();

	if (m_bHasFocus)
	{
		if (m_CaretTickCount <= DEFAULT_CARET_TICK)
		{
			m_CaretTickCount++;
			m_pCaret->Draw();
		}
		else
		{
			m_CaretTickCount++;
			if (m_CaretTickCount == DEFAULT_CARET_TICK * 2)
			{
				m_CaretTickCount = 0;
			}
		}
		m_pSelection->Draw();
	}

	JWControl::EndDrawing();
}

void JWEdit::Focus()
{
	JWControl::Focus();

	UpdateCaretAndSelection();
}

PRIVATE void JWEdit::SelectOrMoveCaretToLeft(size_t Stride)
{
	if (ms_pSharedData->pWindow->GetWindowInputState()->ShiftPressed)
	{
		// Select
		if (m_pCapturedSelPosition == &m_SelStart)
		{
			SIZE_T_MINUS(m_SelEnd, Stride);
		}
		else
		{
			SIZE_T_MINUS(m_SelStart, Stride);
		}

		if (m_SelEnd < m_SelStart)
		{
			Swap(m_SelStart, m_SelEnd);

			m_pCaretSelPosition = &m_SelStart;
			m_pCapturedSelPosition = &m_SelEnd;
		}
	}
	else
	{
		// Move
		MoveCaretToLeft(Stride);
	}
}

PRIVATE void JWEdit::SelectOrMoveCaretToRight(size_t Stride)
{
	if (ms_pSharedData->pWindow->GetWindowInputState()->ShiftPressed)
	{
		// Select
		if (m_pCapturedSelPosition == &m_SelStart)
		{
			SIZE_T_PLUS(m_SelEnd, Stride, GetTextLength());
		}
		else
		{
			SIZE_T_PLUS(m_SelStart, Stride, GetTextLength());
		}

		if (m_SelEnd < m_SelStart)
		{
			Swap(m_SelStart, m_SelEnd);

			m_pCaretSelPosition = &m_SelEnd;
			m_pCapturedSelPosition = &m_SelStart;
		}
	}
	else
	{
		// Move
		MoveCaretToRight(Stride);
	}
}

PRIVATE void JWEdit::MoveCaretToLeft(size_t Stride)
{
	if (m_pCaretSelPosition == &m_SelStart)
	{
		SIZE_T_MINUS(m_SelStart, Stride);
		m_SelEnd = m_SelStart;
	}
	else
	{
		SIZE_T_MINUS(m_SelEnd, Stride);
		m_SelStart = m_SelEnd;
	}
}

PRIVATE void JWEdit::MoveCaretToRight(size_t Stride)
{
	if (m_pCaretSelPosition == &m_SelStart)
	{
		SIZE_T_PLUS(m_SelStart, Stride, GetTextLength());
		m_SelEnd = m_SelStart;
	}
	else
	{
		SIZE_T_PLUS(m_SelEnd, Stride, GetTextLength());
		m_SelStart = m_SelEnd;
	}
}

PRIVATE void JWEdit::UpdateText()
{
	D3DXVECTOR2 text_position = m_PositionClient;
	text_position.x += EDIT_PADDING_X;
	text_position.y += EDIT_PADDING_Y;

	D3DXVECTOR2 text_size = m_Size;
	text_size.x -= EDIT_PADDING_X * 2;
	text_size.y -= EDIT_PADDING_Y * 2;

	m_pFont->SetText(m_Text, text_position, text_size, true);
}

PRIVATE void JWEdit::UpdateCaretAndSelection()
{
	UpdateCaretPosition();
	UpdateSelectionBox();
}

PRIVATE void JWEdit::UpdateCaretPosition()
{
	// Update caret's image position only when the caret has been moved
	if (m_PreviousCaretSelPosition != *m_pCaretSelPosition)
	{
		// If caret position is updated, the caret must be seen.
		// So, let's set tick count to 0
		m_CaretTickCount = 0;

		// Save the changed caret sel position for the next comparison
		m_PreviousCaretSelPosition = *m_pCaretSelPosition;

		m_pFont->UpdateCaretPosition(*m_pCaretSelPosition, &m_CaretPosition);

		m_pCaret->SetLine(0, m_CaretPosition, m_CaretSize);
	}
}

PRIVATE void JWEdit::UpdateSelectionBox()
{
	if (IsTextEmpty())
	{
		// No text => That is, no selection!
		m_pSelection->ClearAllRectangles();
	}
	else
	{
		if (m_SelEnd == m_SelStart)
		{
			// (SelStart == SelEnd) => That is, no selection!
			m_pSelection->ClearAllRectangles();
		}
		else
		{
			// There is selection
			m_pSelection->ClearAllRectangles();

			D3DXVECTOR2 SelectionPosition = D3DXVECTOR2(0, 0);
			D3DXVECTOR2 SelectionSize = D3DXVECTOR2(0, 0);

			size_t sel_start_line_index = m_pFont->GetLineIndexByCharIndex(m_SelStart);
			size_t sel_end_line_index = m_pFont->GetLineIndexByCharIndex(m_SelEnd);
			float SelStartXPosition = m_pFont->GetCharXPosition(m_SelStart);
			float SelEndXPosition = m_pFont->GetCharXPosition(m_SelEnd);

			if (sel_start_line_index == sel_end_line_index)
			{
				if (!m_bUseMultiline)
				{
					// This is a single-line edit
					SelStartXPosition = m_pFont->GetCharXPositionInBox(m_SelStart);
					SelEndXPosition = m_pFont->GetCharXPositionInBox(m_SelEnd);
				}

				// SelStart and SelEnd are in the same line (single-line selection)
				SelectionPosition.x = SelStartXPosition;
				SelectionPosition.y = m_pFont->GetLineYPositionByCharIndex(m_SelStart);

				SelectionSize.x = SelEndXPosition - SelStartXPosition;
				SelectionSize.y = m_pFont->GetLineHeight();

				m_pSelection->AddRectangle(SelectionSize, SelectionPosition);
			}
			else
			{
				// SelStart and SelEnd are in different lines (multiple-line selection)
				// Firstly, we must select the SelStart line
				SelectionPosition.x = SelStartXPosition;
				SelectionPosition.y = m_PositionClient.y + m_pFont->GetLineYPosition(sel_start_line_index) + EDIT_PADDING_Y;

				SelectionSize.x = m_pFont->GetLineWidthByCharIndex(m_SelStart) - SelStartXPosition + m_PositionClient.x + EDIT_PADDING_X;
				SelectionSize.y = m_pFont->GetLineHeight();

				m_pSelection->AddRectangle(SelectionSize, SelectionPosition);

				// Loop for selecting the rest of lines
				for (size_t iterator_line = sel_start_line_index + 1; iterator_line <= sel_end_line_index; iterator_line++)
				{
					if (iterator_line == sel_end_line_index)
					{
						// This is the last line
						// so, select from the first letter to SelEnd
						SelectionPosition.x = m_PositionClient.x;
						SelectionPosition.y = m_PositionClient.y + m_pFont->GetLineYPosition(sel_end_line_index) + EDIT_PADDING_Y;

						SelectionSize.x = SelEndXPosition - m_PositionClient.x;
						SelectionSize.y = m_pFont->GetLineHeight();

						m_pSelection->AddRectangle(SelectionSize, SelectionPosition);
					}
					else
					{
						// This isn't the last line
						// so, we must select the whole line
						SelectionPosition.x = m_PositionClient.x;
						SelectionPosition.y = m_PositionClient.y + m_pFont->GetLineYPosition(iterator_line) + EDIT_PADDING_Y;

						SelectionSize.x = m_pFont->GetLineWidth(iterator_line) + EDIT_PADDING_X;
						SelectionSize.y = m_pFont->GetLineHeight();

						// Add selection box only when it's inside the Edit control's visible region.
						if ((SelectionPosition.y >= m_PositionClient.y) && (SelectionPosition.y <= m_PositionClient.y + m_Size.y))
						{
							m_pSelection->AddRectangle(SelectionSize, SelectionPosition);
						}
					}
				}
			}
		}
	}
}

PRIVATE auto JWEdit::IsTextSelected() const->bool
{
	if (m_SelStart == m_SelEnd)
	{
		return false;
	}
	else
	{
		return true;
	}
}

PRIVATE auto JWEdit::IsTextEmpty() const->bool
{
	if (!m_Text.length())
	{
		return true;
	}

	return false;
}

PRIVATE auto JWEdit::GetTextLength() const->size_t
{
	return m_pFont->GetTextLength();
}

void JWEdit::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_PositionClient);

	UpdateViewport();
}

void JWEdit::SetSize(D3DXVECTOR2 Size)
{
	// If single line, set y size to single-line y size
	if (m_bUseMultiline)
	{
		m_YSizeMultiline = Size.y;
	}
	else
	{
		Size.y = m_YSizeSingleline;
	}

	JWControl::SetSize(Size);

	m_pBackground->SetSize(m_Size);

	UpdateViewport();
}

void JWEdit::SetUseMultiline(bool Value)
{
	m_pFont->SetUseMultiline(Value);
	m_bUseMultiline = Value;

	m_Size.y = m_YSizeMultiline;
	SetSize(m_Size);
	UpdateText();
}

// TODO: IME INPUT ERRORS NEED TO BE FIXED!
void JWEdit::CheckIMEInput()
{
	m_bIMECompleted = false;

	if (ms_pSharedData->pWindow->IsIMECompleted())
	{
		m_pIMECharacter = ms_pSharedData->pWindow->GetpIMEChar();

		// For debugging
		//std::cout << "[DEBUG] IsIMECompleted(): " << static_cast<size_t>(m_pIMECharacter[0]) << std::endl;

		// If the character is '\0', it's deleted.
		if (m_pIMECharacter[0])
		{
			m_bIMECompleted = true;
			InsertChar(m_pIMECharacter[0]);
		}
		else
		{
			// For debugging
			//std::cout << "[DEBUG] Zeroed.." << std::endl;
			UpdateText();
			UpdateCaretAndSelection();
		}
	}

	if (ms_pSharedData->pWindow->IsIMEWriting())
	{
		if (IsTextSelected())
		{
			EraseSelectedText();
		}

		m_pIMECharacter = ms_pSharedData->pWindow->GetpIMEChar();

		// For debugging
		//std::cout << "[DEBUG] IsIMEWriting(): " << static_cast<size_t>(m_pIMECharacter[0]) << std::endl;

		m_IMETempSel = m_SelStart;
		m_IMETempText = m_Text;

		InsertChar(m_pIMECharacter[0]);

		m_Text = m_IMETempText;
		m_SelStart = m_IMETempSel;
		m_SelEnd = m_IMETempSel;
	}
}

void JWEdit::OnKeyDown(WPARAM VirtualKeyCode)
{
	size_t selection_size = 0;

	size_t current_line_index = 0;
	size_t compare_line_index = 0;

	size_t current_line_sel_position = 0;
	size_t current_line_length = 0;
	size_t compare_line_length = 0;

	switch (VirtualKeyCode)
	{
	case VK_DELETE:
		if (IsTextSelected())
		{
			EraseSelectedText();
		}
		else
		{
			EraseAfter();
		}
		break;
	case VK_HOME:
		if (!IsTextSelected())
		{
			// If the text wasn't selected before,
			// capture SelEnd
			m_pCapturedSelPosition = &m_SelEnd;
			m_pCaretSelPosition = &m_SelStart;
		}

		SelectOrMoveCaretToLeft(m_pFont->GetLineSelPositionByCharIndex(*m_pCaretSelPosition));

		break;
	case VK_END:
		if (!IsTextSelected())
		{
			// If the text wasn't selected before,
			// capture SelStart
			m_pCapturedSelPosition = &m_SelStart;
			m_pCaretSelPosition = &m_SelEnd;
		}

		SelectOrMoveCaretToRight(m_pFont->GetLineLengthByCharIndex(*m_pCaretSelPosition)
			- m_pFont->GetLineSelPositionByCharIndex(*m_pCaretSelPosition) - 1);

		break;
	case VK_LEFT:
		if (!IsTextSelected())
		{
			// If text wasn't selected before,
			// capture SelEnd
			m_pCapturedSelPosition = &m_SelEnd;
			m_pCaretSelPosition = &m_SelStart;
		}

		SelectOrMoveCaretToLeft();

		break;
	case VK_RIGHT:
		if (!IsTextSelected())
		{
			// If text wasn't selected before,
			// capture SelStart
			m_pCapturedSelPosition = &m_SelStart;
			m_pCaretSelPosition = &m_SelEnd;
		}

		SelectOrMoveCaretToRight();

		break;
	case VK_DOWN:
		current_line_index = m_pFont->GetLineIndexByCharIndex(*m_pCaretSelPosition);

		compare_line_index = current_line_index + 1;

		if (compare_line_index < m_pFont->GetLineCount())
		{
			current_line_sel_position = m_pFont->GetLineSelPositionByCharIndex(*m_pCaretSelPosition);
			current_line_length = m_pFont->GetLineLength(current_line_index);
			compare_line_length = m_pFont->GetLineLength(compare_line_index);

			if (current_line_sel_position >= compare_line_length)
			{
				SelectOrMoveCaretToRight(current_line_length - current_line_sel_position + compare_line_length - 1);
			}
			else
			{
				SelectOrMoveCaretToRight(current_line_length);
			}

			if (!ms_pSharedData->pWindow->GetWindowInputState()->ShiftPressed)
			{
				m_SelStart = m_SelEnd;
			}
		}
		break;
	case VK_UP:
		current_line_index = m_pFont->GetLineIndexByCharIndex(*m_pCaretSelPosition);

		if (current_line_index)
		{
			compare_line_index = current_line_index - 1;
		}

		current_line_sel_position = m_pFont->GetLineSelPositionByCharIndex(*m_pCaretSelPosition);
		current_line_length = m_pFont->GetLineLength(current_line_index);
		compare_line_length = m_pFont->GetLineLength(compare_line_index);

		if (current_line_sel_position >= compare_line_length)
		{
			SelectOrMoveCaretToLeft(current_line_sel_position + 1);
		}
		else
		{
			SelectOrMoveCaretToLeft(compare_line_length);
		}

		if (!ms_pSharedData->pWindow->GetWindowInputState()->ShiftPressed)
		{
			m_SelEnd = m_SelStart;
		}
	}

	UpdateCaretAndSelection();
}

void JWEdit::OnCharKey(WPARAM Char)
{
	wchar_t wchar = static_cast<wchar_t>(Char);

	if (wchar == 1) // Ctrl + a
	{
		if (!IsTextSelected())
		{
			SelectAll();
		}
	}
	else if (wchar == 3) // Ctrl + c
	{
		CopySelectedText();
	}
	else if (wchar == 6) // Ctrl + f
	{
		
	}
	else if (wchar == 8) // Ctrl + h || Backspace
	{
		// Let's ignore <Ctrl + h>
		if (!ms_pSharedData->pWindow->GetWindowInputState()->ControlPressed)
		{
			if (IsTextSelected())
			{
				EraseSelectedText();
			}
			else
			{
				EraseBefore();
			}
		}
	}
	else if (wchar == 13) // Ctrl + m || Enter
	{
		// Let's ignore <Ctrl + m>
		if (!ms_pSharedData->pWindow->GetWindowInputState()->ControlPressed)
		{
			if (IsTextSelected())
			{
				EraseSelectedText();
			}
			InsertNewLine();
		}
	}
	else if (wchar == 22) // Ctrl + v
	{
		if (IsTextSelected())
		{
			EraseSelectedText();
		}

		PasteText();
	}
	else if (wchar == 24) // Ctrl + x
	{
		CopySelectedText();
		EraseSelectedText();
	}
	else if (wchar == 26) // Ctrl + z
	{
		
	}
	else if (wchar == 27) // Ctrl + [ || Escape
	{
		// Let's ignore <Ctrl + [>
		if (!ms_pSharedData->pWindow->GetWindowInputState()->ControlPressed)
		{
			m_SelStart = *m_pCaretSelPosition;
			m_SelEnd = *m_pCaretSelPosition;
		}
	}
	else if (wchar >= 32) // Characters
	{
		if (IsTextSelected())
		{
			EraseSelectedText();
		}

		// For debugging
		//std::cout << "[DEBUG] OnCharKey: " << wchar << std::endl;

		// To avoid duplicate input
		if (!m_bIMECompleted)
		{
			InsertChar(wchar);
		}
		else
		{
			if (m_pIMECharacter[0] != wchar)
			{
				InsertChar(wchar);
			}
		}
	}
}

PRIVATE void JWEdit::CopySelectedText()
{
	m_ClipText = m_Text.substr(m_SelStart, m_SelEnd - m_SelStart);

	if (!m_ClipText.length())
		return;

	const wchar_t* copy_text = m_ClipText.c_str();
	const size_t copy_length = (m_ClipText.length() + 1) * 2;

	OpenClipboard(0);
	EmptyClipboard();

	HGLOBAL hGlobal = nullptr;
	while (!hGlobal)
	{
		hGlobal = GlobalAlloc(GMEM_MOVEABLE, copy_length);
	}

	memcpy(GlobalLock(hGlobal), copy_text, copy_length);
	GlobalUnlock(hGlobal);
	SetClipboardData(CF_UNICODETEXT, hGlobal);
	CloseClipboard();
	GlobalFree(hGlobal);
}

PRIVATE void JWEdit::PasteText()
{
	LPCTSTR temp_string = nullptr;

	OpenClipboard(0);

	HGLOBAL hGlobal = GetClipboardData(CF_UNICODETEXT);
	if (hGlobal)
	{
		temp_string = static_cast<LPCTSTR>(GlobalLock(hGlobal));

		if (temp_string)
		{
			GlobalUnlock(hGlobal);
		}
	}
	CloseClipboard();

	m_ClipText = temp_string;

	InsertString(m_ClipText);
}

PRIVATE void JWEdit::InsertChar(wchar_t Char)
{
	m_SelEnd = m_SelStart;

	wchar_t current_sel_character = m_pFont->GetCharacter(m_SelStart);
	size_t adjusted_sel_position = m_pFont->GetAdjustedSelPosition(m_SelStart);

	// If this is a multi-line edit,
	// split line ends with '\0'
	// when we meet this value, we need to advance one our adjusted_sel_position
	if (current_sel_character == 0)
	{
		adjusted_sel_position++;
	}
	
	m_Text = m_Text.insert(adjusted_sel_position, 1, Char);

	// Update the text
	UpdateText();

	if (m_SelStart)
	{
		if (m_pFont->GetCharacter(m_SelStart) == 0)
		{
			// Splitted line's end
			MoveCaretToRight(2);
		}
		else
		{
			MoveCaretToRight();
		}
	}
	else
	{
		MoveCaretToRight();
	}

	// Update the caret position and selection
	UpdateCaretAndSelection();
}

PRIVATE void JWEdit::InsertString(WSTRING String)
{
	m_SelEnd = m_SelStart;

	wchar_t current_sel_character = m_pFont->GetCharacter(m_SelStart);
	size_t adjusted_sel_position = m_pFont->GetAdjustedSelPosition(m_SelStart);

	// If this is a multi-line edit,
	// split line ends with '\0'
	// when we meet this value, we need to advance one our adjusted_sel_position
	if (current_sel_character == 0)
	{
		adjusted_sel_position++;
	}

	// We don't use '\r', so remove it
	size_t find_r = String.find('\r');
	while (find_r != String.npos)
	{
		String.erase(find_r, 1);
		find_r = String.find('\r');
	}

	if (!m_bUseMultiline)
	{
		// If this is single-line edit,
		// we don't use '\n', so clip the string
		size_t find_r = String.find('\n');
		String = String.substr(0, find_r);
	}

	// Insert this new string into m_Text
	m_Text = m_Text.insert(adjusted_sel_position, String);

	// Set move stride
	size_t stride = String.length();

	// Update the text
	UpdateText();

	if (m_bUseMultiline)
	{
		if (m_SelStart)
		{
			if (m_pFont->GetCharacter(m_SelStart) == 0)
			{
				// Splitted line's end
				MoveCaretToRight(stride + 1);
			}
			else
			{
				MoveCaretToRight(stride);
			}
		}
		else
		{
			MoveCaretToRight(stride);
		}
	}
	else
	{
		MoveCaretToRight(stride);
	}

	// Update the caret position and selection
	UpdateCaretAndSelection();
}

PRIVATE void JWEdit::InsertNewLine()
{
	if (m_pFont->GetUseMultiline())
	{
		InsertChar(L'\n');
	}
}

PRIVATE void JWEdit::EraseSelectedText(bool bEraseAfter)
{
	wchar_t current_character = m_pFont->GetCharacter(m_SelStart);
	size_t erase_count = m_SelEnd - m_SelStart;
	size_t adjusted_sel_position = m_pFont->GetAdjustedSelPosition(m_SelStart);
	if ((current_character == 0) && (bEraseAfter))
	{
		adjusted_sel_position++;
	}
	m_Text = m_Text.erase(adjusted_sel_position, erase_count);

	// Update the text
	UpdateText();

	if ((current_character == 0) && (!bEraseAfter))
	{
		// If the current sel's character is '\0', it is a splitted line
		m_SelStart--;
	}

	// There has to be no selection
	m_SelEnd = m_SelStart;

	// Update the caret position and selection
	UpdateCaretAndSelection();
}

PRIVATE void JWEdit::EraseAfter()
{
	m_SelEnd = m_SelStart + 1;
	EraseSelectedText(true);
}

PRIVATE void JWEdit::EraseBefore()
{
	if (m_SelStart)
	{
		m_SelStart--;
		EraseSelectedText();
	}
}

PRIVATE void JWEdit::SelectAll()
{
	m_SelStart = 0;
	m_SelEnd = GetTextLength();
	m_pCaretSelPosition = &m_SelEnd;
	m_pCapturedSelPosition = &m_SelStart;

	// Update the caret position and selection
	UpdateCaretAndSelection();
}

void JWEdit::OnMouseDown(LPARAM MousePosition)
{
	JWControl::OnMouseDown(MousePosition);

	if (ms_pSharedData->pWindow->GetWindowInputState()->ShiftPressed)
	{
		*m_pCaretSelPosition = m_pFont->GetCharIndexByMousePosition(m_MousePosition);

		if (m_SelEnd < m_SelStart)
		{
			Swap(m_SelStart, m_SelEnd);
			SwapPointer(m_pCapturedSelPosition, m_pCaretSelPosition);
		}
	}
	else
	{
		m_SelStart = m_pFont->GetCharIndexByMousePosition(m_MousePosition);
		m_SelEnd = m_SelStart;
		m_pCapturedSelPosition = &m_SelStart;
		m_pCaretSelPosition = &m_SelEnd;
	}

	UpdateCaretAndSelection();
}

void JWEdit::OnMouseMove(LPARAM MousePosition)
{
	JWControl::OnMouseMove(MousePosition);

	if (ms_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
	{
		*m_pCaretSelPosition = m_pFont->GetCharIndexByMousePosition(m_MousePosition);

		if (m_SelEnd < m_SelStart)
		{
			Swap(m_SelStart, m_SelEnd);
			SwapPointer(m_pCapturedSelPosition, m_pCaretSelPosition);
		}

		UpdateCaretAndSelection();
	}
}