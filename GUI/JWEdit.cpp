#include "JWEdit.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWLine.h"
#include "../CoreBase/JWRectangle.h"

using namespace JWENGINE;

JWEdit::JWEdit()
{
	m_pCaret = nullptr;
	m_CaretSize = D3DXVECTOR2(0, 0);
	m_CaretPosition = D3DXVECTOR2(0, 0);

	m_pSelection = nullptr;

	m_SelStart = 0;
	m_SelEnd = 0;
	m_pCapturedSelPosition = &m_SelStart;
	m_pCaretSelPosition = &m_SelStart;
	m_IMETempSel = 0;
	m_CaretTickCount = 0;

	m_bUseMultiline = false;
	m_YSizeMultiline = 0;
	m_YSizeSingleline = 0;
}

auto JWEdit::Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(pWindow, BaseDir, Position, Size)))
		return EError::CONTROL_NOT_CREATED;
	
	// Create line for caret
	if (m_pCaret = new JWLine)
	{
		if (JW_FAILED(m_pCaret->Create(pWindow->GetDevice())))
			return EError::LINE_NOT_CREATED;

		m_pCaret->AddLine(D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 10), D3DCOLOR_XRGB(0, 0, 0));
		m_pCaret->AddEnd();
	}
	else
	{
		return EError::LINE_NOT_CREATED;
	}

	// Create rectangle for selection
	if (m_pSelection = new JWRectangle)
	{
		if (JW_FAILED(m_pSelection->Create(pWindow, BaseDir, MAX_TEXT_LEN)))
			return EError::RECTANGLE_NOT_CREATED;

		m_pSelection->SetRectangleAlpha(100);
		m_pSelection->SetRectangleXRGB(D3DCOLOR_XRGB(0, 100, 255));
	}
	else
	{
		return EError::RECTANGLE_NOT_CREATED;
	}

	// Set default color
	m_pFont->SetFontXRGB(DEFAULT_COLOR_EDIT_FONT);
	m_pFont->SetBoxAlpha(DEFUALT_ALPHA_BACKGROUND);
	m_pFont->SetBoxXRGB(DEFAULT_COLOR_BACKGROUND);

	// Set multiline and singleline y size
	m_YSizeMultiline = Size.y;
	m_YSizeSingleline = m_pFont->GetLineHeight();

	// Set edit size
	SetSize(m_Size);

	// Set caret size
	m_CaretSize = D3DXVECTOR2(0, m_pFont->GetLineHeight());

	// Set control type
	m_Type = EControlType::Edit;

	return EError::OK;
}

void JWEdit::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pSelection);
}

void JWEdit::Draw()
{
	if (!ms_pWindow->IsIMEWriting())
	{
		JWControl::Draw();
	}

	m_pFont->Draw();

	if (m_bHasFocus)
	{
		if (m_CaretTickCount <= DEFAULT_CARET_TICK)
		{
			m_pCaret->Draw();
			m_CaretTickCount++;
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
}

void JWEdit::Focus()
{
	JWControl::Focus();

	GetSelStartAndEndData();
	UpdateCaretPosition();
	UpdateSelectionBox();
}

PRIVATE void JWEdit::GetSelStartAndEndData()
{
	m_SelStartLineData = GetLineDataFromSelPosition(m_Text, m_SelStart);
	m_SelEndLineData = GetLineDataFromSelPosition(m_Text, m_SelEnd);
}

PRIVATE void JWEdit::UpdateCaretPosition()
{
	m_CaretPosition = m_PositionClient;

	SLineData* pCaretLineData = nullptr;
	if (m_pCaretSelPosition == &m_SelStart)
	{
		pCaretLineData = &m_SelStartLineData;
	}
	else
	{
		pCaretLineData = &m_SelEndLineData;
	}

	m_CaretPosition.x += m_pFont->GetCharXPositionInLine(pCaretLineData->LineSelPosition, pCaretLineData->LineText);

	if (m_bUseMultiline)
	{
		m_CaretPosition.y += m_pFont->GetLineYPosition(pCaretLineData->LineIndex);
	}
	
	m_pCaret->SetLine(0, m_CaretPosition, m_CaretSize);
	
	// If caret position is updated, the caret must be seen
	// so set tick count to 0
	m_CaretTickCount = 0;
}

PRIVATE void JWEdit::UpdateSelectionBox()
{
	if (m_Text.length())
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

			if (m_SelStartLineData.LineIndex == m_SelEndLineData.LineIndex)
			{
				// SelStart and SelEnd are in the same line
				float SelStartXPosition = m_pFont->GetCharXPositionInLine(m_SelStartLineData.LineSelPosition,
					m_SelStartLineData.LineText);
				float SelEndXPosition = m_pFont->GetCharXPositionInLine(m_SelEndLineData.LineSelPosition,
					m_SelEndLineData.LineText);

				SelectionPosition.x = m_PositionClient.x + SelStartXPosition;
				SelectionPosition.y = m_PositionClient.y + m_pFont->GetLineYPosition(m_SelStartLineData.LineIndex);

				SelectionSize.x = SelEndXPosition - SelStartXPosition;
				SelectionSize.y = m_pFont->GetLineHeight();

				m_pSelection->AddRectangle(SelectionSize, SelectionPosition);
			}
			else
			{
				// SelStart and SelEnd are in different lines (multiple-line selection)
				// Firstly, we must select the whole line of SelStart
				float SelStartXPosition = m_pFont->GetCharXPositionInLine(m_SelStartLineData.LineSelPosition,
					m_SelStartLineData.LineText);
				
				SelectionPosition.x = m_PositionClient.x + SelStartXPosition;
				SelectionPosition.y = m_PositionClient.y + m_pFont->GetLineYPosition(m_SelStartLineData.LineIndex);

				SelectionSize.x = m_pFont->GetLineLength(m_SelStartLineData.LineText) - SelStartXPosition;
				SelectionSize.y = m_pFont->GetLineHeight();

				m_pSelection->AddRectangle(SelectionSize, SelectionPosition);

				// Loop for selecting the rest of lines
				for (size_t iterator_line = m_SelStartLineData.LineIndex + 1;
					iterator_line <= m_SelEndLineData.LineIndex;
					iterator_line++)
				{
					if (iterator_line == m_SelEndLineData.LineIndex)
					{
						// This is the last line
						// so, select from the first letter to SelEnd
						float SelEndXPosition = m_pFont->GetCharXPositionInLine(m_SelEndLineData.LineSelPosition,
							m_SelEndLineData.LineText);

						SelectionPosition.x = m_PositionClient.x;
						SelectionPosition.y = m_PositionClient.y + m_pFont->GetLineYPosition(m_SelEndLineData.LineIndex);

						SelectionSize.x = SelEndXPosition;
						SelectionSize.y = m_pFont->GetLineHeight();

						m_pSelection->AddRectangle(SelectionSize, SelectionPosition);
					}
					else
					{
						// This isn't the last line
						// so, we must select the whole line
						SLineData TempData = GetLineDataFromLineIndex(m_Text, iterator_line);

						SelectionPosition.x = m_PositionClient.x;
						SelectionPosition.y = m_PositionClient.y + m_pFont->GetLineYPosition(TempData.LineIndex);

						SelectionSize.x = m_pFont->GetLineLength(TempData.LineText);
						SelectionSize.y = m_pFont->GetLineHeight();

						m_pSelection->AddRectangle(SelectionSize, SelectionPosition);
					}
				}
			}
		}
	}
	else
	{
		// No text => That is, no selection!
		m_pSelection->ClearAllRectangles();
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

PRIVATE void JWEdit::SelectionToLeft(size_t Stride)
{
	if (ms_WindowKeySate.ShiftPressed)
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
}

PRIVATE void JWEdit::SelectionToRight(size_t Stride)
{
	if (ms_WindowKeySate.ShiftPressed)
	{
		// Select
		if (m_pCapturedSelPosition == &m_SelStart)
		{
			SIZE_T_PLUS(m_SelEnd, Stride, m_Text.length());
		}
		else
		{
			SIZE_T_PLUS(m_SelStart, Stride, m_Text.length());
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
		if (m_pCaretSelPosition == &m_SelStart)
		{
			SIZE_T_PLUS(m_SelStart, Stride, m_Text.length());
			m_SelEnd = m_SelStart;
		}
		else
		{
			SIZE_T_PLUS(m_SelEnd, Stride, m_Text.length());
			m_SelStart = m_SelEnd;
		}
	}
}

void JWEdit::SetSize(D3DXVECTOR2 Size)
{
	// If single line, set y size to single
	if (m_bUseMultiline)
	{
		Size.y = m_YSizeMultiline;
	}
	else
	{
		Size.y = m_YSizeSingleline;
	}

	JWControl::SetSize(Size);
}

void JWEdit::OnKeyDown(WPARAM VirtualKeyCode)
{
	size_t selection_size = 0;
	SLineData current_line_data;
	SLineData upper_line_data;
	SLineData lower_line_data;

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

		current_line_data = GetLineDataFromSelPosition(m_Text, *m_pCaretSelPosition);
		SelectionToLeft(current_line_data.LineSelPosition);
		break;
	case VK_END:
		// <End> selection
		if (!IsTextSelected())
		{
			// If the text wasn't selected before,
			// capture SelStart
			m_pCapturedSelPosition = &m_SelStart;
			m_pCaretSelPosition = &m_SelEnd;
		}

		current_line_data = GetLineDataFromSelPosition(m_Text, *m_pCaretSelPosition);
		SelectionToRight(current_line_data.LineLength - current_line_data.LineSelPosition);
		break;
	case VK_LEFT:
		if (!IsTextSelected())
		{
			// If text wasn't selected before,
			// capture SelEnd
			m_pCapturedSelPosition = &m_SelEnd;
			m_pCaretSelPosition = &m_SelStart;
		}

		SelectionToLeft();
		break;
	case VK_RIGHT:
		if (!IsTextSelected())
		{
			// If text wasn't selected before,
			// capture SelStart
			m_pCapturedSelPosition = &m_SelStart;
			m_pCaretSelPosition = &m_SelEnd;
		}

		SelectionToRight();
		break;
	case VK_DOWN:
		// Current line's data
		current_line_data = GetLineDataFromSelPosition(m_Text, *m_pCaretSelPosition);

		// Lower line's data
		lower_line_data = GetLineDataFromSelPosition(m_Text, *m_pCaretSelPosition +
			(current_line_data.LineLength - current_line_data.LineSelPosition + 1));
		
		if (lower_line_data.LineIndex)
		{
			if (current_line_data.LineSelPosition > lower_line_data.LineLength)
			{
				SelectionToRight(lower_line_data.LineLength + 1);
			}
			else
			{
				SelectionToRight(current_line_data.LineLength + 1);
			}

			if (!ms_WindowKeySate.ShiftPressed)
			{
				m_SelStart = m_SelEnd;
			}
		}
		break;
	case VK_UP:
		// Current line's data
		current_line_data = GetLineDataFromSelPosition(m_Text, *m_pCaretSelPosition);

		// Upper line's data
		upper_line_data = GetLineDataFromSelPosition(m_Text, *m_pCaretSelPosition - (current_line_data.LineSelPosition + 1));
		
		if (current_line_data.LineSelPosition > upper_line_data.LineLength)
		{
			SelectionToLeft(current_line_data.LineSelPosition + 1);
		}
		else
		{
			SelectionToLeft(upper_line_data.LineLength + 1);
		}
		
		if (!ms_WindowKeySate.ShiftPressed)	
		{
			m_SelEnd = m_SelStart;
		}
		break;
	}

	GetSelStartAndEndData();
	UpdateCaretPosition();
	UpdateSelectionBox();
}

void JWEdit::CheckIMEInput()
{
	if (ms_pWindow->IsIMEWriting())
	{
		if (IsTextSelected())
		{
			EraseSelectedText();
			UpdateText();

			GetSelStartAndEndData();
			UpdateCaretPosition();
			UpdateSelectionBox();
		}

		TCHAR* pTCHAR = ms_pWindow->GetpIMEChar();

		m_IMETempSel = m_SelStart;
		m_IMETempText = m_Text;

		m_Text = m_Text.insert(m_SelStart, 1, pTCHAR[0]);

		UpdateText();

		m_SelStart++;
		m_SelEnd++;

		GetSelStartAndEndData();
		UpdateCaretPosition();
		UpdateSelectionBox();

		m_Text = m_IMETempText;
		m_SelStart = m_IMETempSel;
		m_SelEnd = m_IMETempSel;
	}

	if (ms_pWindow->IsIMECompleted())
	{
		TCHAR* pTCHAR = ms_pWindow->GetpIMEChar();

		if (pTCHAR[0])
		{
			m_Text = m_Text.insert(m_SelStart, 1, pTCHAR[0]);

			UpdateText();

			m_SelStart++;
			m_SelEnd++;
		}

		GetSelStartAndEndData();
		UpdateCaretPosition();
		UpdateSelectionBox();
	}
}

void CopyToClipboard(WSTRING Text)
{
	if (!Text.length())
		return;

	const wchar_t* copy_text = Text.c_str();
	const size_t copy_length = (Text.length() + 1) * 2;

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

void PasteFromClipboard(WSTRING& Text)
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

	Text = temp_string;
}

PRIVATE void JWEdit::UpdateText()
{
	m_pFont->SetText(m_Text, m_PositionClient, m_Size);
}

PRIVATE void JWEdit::UpdateCaretAndSelection()
{
	GetSelStartAndEndData();
	UpdateCaretPosition();
	UpdateSelectionBox();
}

void JWEdit::OnCharKey(WPARAM Char)
{
	wchar_t wchar = static_cast<wchar_t>(Char);

	if (wchar == 1) // Ctrl + a
	{
		if (!IsTextSelected())
		{
			m_SelStart = 0;
			m_SelEnd = m_Text.length();
			m_pCaretSelPosition = &m_SelEnd;

			UpdateCaretAndSelection();
		}
		return;
	}
	else if (wchar == 3) // Ctrl + c
	{
		m_ClipText = m_Text.substr(m_SelStart, m_SelEnd - m_SelStart);
		CopyToClipboard(m_ClipText);
		return;
	}
	else if (wchar == 22) // Ctrl + v
	{
		PasteFromClipboard(m_ClipText);
		for (size_t iterator = 0; iterator < m_ClipText.size(); iterator++)
		{
			InsertChar(static_cast<wchar_t>(m_ClipText[iterator]));
		}

		UpdateText();
		UpdateCaretAndSelection();
		return;
	}
	else if (wchar == 24) // Ctrl + x
	{
		m_ClipText = m_Text.substr(m_SelStart, m_SelEnd - m_SelStart);
		CopyToClipboard(m_ClipText);
		EraseSelectedText();

		UpdateText();
		UpdateCaretAndSelection();
		return;
	}
	else if (wchar == 26) // Ctrl + z
	{

		return;
	}
	else if (wchar == 27) // Escape
	{
		m_SelStart = *m_pCaretSelPosition;
		m_SelEnd = *m_pCaretSelPosition;

		UpdateCaretAndSelection();
		return;
	}

	if (IsTextSelected())
	{
		if (wchar == 8) // Backspace
		{
			EraseSelectedText();
		}
		else if (wchar == 13) // Enter
		{
			EraseSelectedText();
			InsertNewLine();
		}
		else
		{
			// Char input
			EraseSelectedText();
			InsertChar(wchar);
		}
	}
	else
	{
		// No selection
		if (wchar == 8) // Backspace
		{
			EraseBefore();
		}
		else if (wchar == 13) // Enter
		{
			InsertNewLine();
		}
		else
		{
			// Char input
			InsertChar(wchar);
		}
	}

	UpdateText();
	UpdateCaretAndSelection();
}

PRIVATE void JWEdit::EraseSelectedText()
{
	size_t erase_count = m_SelEnd - m_SelStart;
	m_Text.erase(m_SelStart, erase_count);
	m_SelEnd = m_SelStart;
}

PRIVATE void JWEdit::EraseAfter()
{
	m_Text.erase(m_SelStart, 1);
}

PRIVATE void JWEdit::EraseBefore()
{
	if (m_SelStart)
	{
		m_Text.erase(m_SelStart - 1, 1);

		m_SelStart--;
		m_SelEnd = m_SelStart;
	}
}

PRIVATE void JWEdit::InsertNewLine()
{
	if (m_pFont->GetUseMultiline())
	{
		m_Text = m_Text.insert(m_SelEnd, 1, L'\n');
		m_SelStart++;
		m_SelEnd = m_SelStart;
	}
}

PRIVATE void JWEdit::InsertChar(wchar_t Char)
{
	m_Text = m_Text.insert(m_SelStart, 1, Char);

	m_SelStart++;
	m_SelEnd = m_SelStart;
}

void JWEdit::OnMouseDown(LPARAM MousePosition)
{
	JWControl::OnMouseDown(MousePosition);

	POINT NewPosition;
	NewPosition.x = m_MousePosition.x - static_cast<LONG>(m_PositionClient.x);
	NewPosition.y = m_MousePosition.y - static_cast<LONG>(m_PositionClient.y);

	m_SelStartLineData = GetLineDataFromMousePosition(m_Text, NewPosition, m_pFont->GetLineHeight());
	m_SelStartLineData.LineSelPosition = m_pFont->GetCharIndexInLine(NewPosition.x, m_SelStartLineData.LineText);
	m_SelStart = ConvertLineSelPositionToSelPosition(m_Text, m_SelStartLineData.LineSelPosition, m_SelStartLineData.LineIndex);
	m_SelEnd = m_SelStart;
	m_pCapturedSelPosition = &m_SelStart;

	GetSelStartAndEndData();
	UpdateCaretPosition();
	UpdateSelectionBox();
}

void JWEdit::OnMouseMove(LPARAM MousePosition)
{
	JWControl::OnMouseMove(MousePosition);

	if (m_MouseLeftDown)
	{
		POINT NewPosition;
		NewPosition.x = m_MousePosition.x - static_cast<LONG>(m_PositionClient.x);
		NewPosition.y = m_MousePosition.y - static_cast<LONG>(m_PositionClient.y);

		SLineData temp_data = GetLineDataFromMousePosition(m_Text, NewPosition, m_pFont->GetLineHeight());
		temp_data.LineSelPosition = m_pFont->GetCharIndexInLine(NewPosition.x, temp_data.LineText);
		size_t temp_sel_position = ConvertLineSelPositionToSelPosition(m_Text, temp_data.LineSelPosition, temp_data.LineIndex);

		if (temp_sel_position >= *m_pCapturedSelPosition)
		{
			m_SelEndLineData = GetLineDataFromMousePosition(m_Text, NewPosition, m_pFont->GetLineHeight());
			m_SelEndLineData.LineSelPosition = m_pFont->GetCharIndexInLine(NewPosition.x, m_SelEndLineData.LineText);
			m_SelEnd = ConvertLineSelPositionToSelPosition(m_Text, m_SelEndLineData.LineSelPosition, m_SelEndLineData.LineIndex);
		}
		else
		{
			m_SelStartLineData = GetLineDataFromMousePosition(m_Text, NewPosition, m_pFont->GetLineHeight());
			m_SelStartLineData.LineSelPosition = m_pFont->GetCharIndexInLine(NewPosition.x, m_SelStartLineData.LineText);
			m_SelStart = ConvertLineSelPositionToSelPosition(m_Text, m_SelStartLineData.LineSelPosition, m_SelStartLineData.LineIndex);
		}

		GetSelStartAndEndData();
		UpdateCaretPosition();
		UpdateSelectionBox();
	}
}