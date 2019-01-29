#include "JWEdit.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWLine.h"
#include "../CoreBase/JWRectangle.h"

using namespace JWENGINE;

JWEdit::JWEdit()
{
	m_pCaret = nullptr;
	m_pSelection = nullptr;

	m_SelStart = 0;
	m_SelEnd = 0;
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

	// Set default background color
	m_pFont->SetBoxAlpha(DEFUALT_ALPHA_BACKGROUND);
	m_pFont->SetBoxXRGB(DEFAULT_COLOR_BACKGROUND);

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
	JWControl::Draw();

	m_pFont->Draw();

	if (m_bHasFocus)
	{
		m_pCaret->Draw();
		m_pSelection->Draw();
	}
}

void JWEdit::Focus()
{
	JWControl::Focus();

	GetSelStartAndEndData();
	UpdateCaret();
	UpdateSelection();
}

PRIVATE void JWEdit::GetSelStartAndEndData()
{
	m_SelStartLineData = GetLineDataFromSelPosition(m_Text, m_SelStart);
	m_SelEndLineData = GetLineDataFromSelPosition(m_Text, m_SelEnd);
}

PRIVATE void JWEdit::UpdateCaret()
{
	if (m_Text.length())
	{
		D3DXVECTOR2 CaretSize = D3DXVECTOR2(0, m_pFont->GetLineHeight());

		D3DXVECTOR2 CaretPosition = m_PositionClient;
		CaretPosition.x += m_pFont->GetCharXPositionInLine(m_SelEndLineData.LineSelPosition, m_SelEndLineData.LineText);
		CaretPosition.y += m_pFont->GetLineYPosition(m_SelEndLineData.LineIndex);

		m_pCaret->SetLine(0, CaretPosition, CaretSize);
	}
}

PRIVATE void JWEdit::UpdateSelection()
{
	if (m_Text.length())
	{
		if (m_SelEnd > m_SelStart)
		{
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
		else
		{
			// (SelStart == SelEnd) => That is, no selection!
			m_pSelection->ClearAllRectangles();
		}
	}
}

void JWEdit::OnKeyDown(WPARAM VirtualKeyCode)
{
	JWControl::OnKeyDown(VirtualKeyCode);

	switch (VirtualKeyCode)
	{
	case VK_LEFT:
		if (m_SelStart)
		{
			m_SelStart--;
			Sleep(50);
		}
		break;
	case VK_RIGHT:
		if (m_SelStart < static_cast<UINT>(m_Text.length()))
		{
			m_SelStart++;
			Sleep(50);
		}
		break;
	}

	m_SelEnd = m_SelStart;

	GetSelStartAndEndData();
	UpdateCaret();
	UpdateSelection();
}

void JWEdit::OnMouseDown(LPARAM MousePosition)
{
	if (m_MouseLeftDown == false)
	{
		POINT NewPosition;
		NewPosition.x = m_MousePosition.x - static_cast<LONG>(m_PositionClient.x);
		NewPosition.y = m_MousePosition.y - static_cast<LONG>(m_PositionClient.y);

		m_SelStartLineData = GetLineDataFromMousePosition(m_Text, NewPosition, m_pFont->GetLineHeight());
		m_SelStartLineData.LineSelPosition = m_pFont->GetCharIndexInLine(NewPosition.x, m_SelStartLineData.LineText);
		m_SelStart = ConvertLineSelPositionToSelPosition(m_Text, m_SelStartLineData.LineSelPosition, m_SelStartLineData.LineIndex);
		m_SelEnd = m_SelStart;

		GetSelStartAndEndData();
		UpdateCaret();
		UpdateSelection();
	}

	JWControl::OnMouseDown(MousePosition);
}

void JWEdit::OnMouseMove(LPARAM MousePosition)
{
	JWControl::OnMouseMove(MousePosition);

	if (m_MouseLeftDown)
	{
		POINT NewPosition;
		NewPosition.x = m_MousePosition.x - static_cast<LONG>(m_PositionClient.x);
		NewPosition.y = m_MousePosition.y - static_cast<LONG>(m_PositionClient.y);

		m_SelEndLineData = GetLineDataFromMousePosition(m_Text, NewPosition, m_pFont->GetLineHeight());
		m_SelEndLineData.LineSelPosition = m_pFont->GetCharIndexInLine(NewPosition.x, m_SelEndLineData.LineText);
		m_SelEnd = ConvertLineSelPositionToSelPosition(m_Text, m_SelEndLineData.LineSelPosition, m_SelEndLineData.LineIndex);

		std::cout << "S: " << m_SelStart << "  E: " << m_SelEnd << std::endl;

		if (m_SelEnd < m_SelStart)
		{
			Swap(m_SelStart, m_SelEnd);

			GetSelStartAndEndData();
			UpdateCaret();
			UpdateSelection();

			std::cout << "SWAPED = S: " << m_SelStart << "  E: " << m_SelEnd << std::endl;

			Swap(m_SelStart, m_SelEnd);
		}
		else
		{
			GetSelStartAndEndData();
			UpdateCaret();
			UpdateSelection();
		}
	}
}