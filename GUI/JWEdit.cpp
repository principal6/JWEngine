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

	UpdateCaret();
	UpdateSelection();
}

PRIVATE void JWEdit::UpdateCaret()
{
	wchar_t CurrCharacter = m_Text[m_SelStart];

	SLineData LineData = GetLineDataFromText(m_Text, m_SelStart);

	D3DXVECTOR2 NewSize = D3DXVECTOR2(0, static_cast<float>(m_pFont->GetFontSize()));

	D3DXVECTOR2 NewPosition = m_PositionClient;
	NewPosition.x += m_pFont->GetSelPositionXInLine(LineData.LineSelPosition, LineData.LineText);
	NewPosition.y += static_cast<float>(m_pFont->GetFontSize() * LineData.LineIndex);

	m_pCaret->SetLine(0, NewPosition, NewSize);
}

PRIVATE void JWEdit::UpdateSelection()
{
	if (m_Text.length())
	{
		m_pSelection->ClearAllRectangles();


		wchar_t CurrCharacter = m_Text[m_SelStart];

		if (CurrCharacter == L'\n')
			CurrCharacter = 0;

		SLineData LineData = GetLineDataFromText(m_Text, m_SelStart);

		D3DXVECTOR2 NewSize = m_pFont->GetCharSize(CurrCharacter);
		NewSize.y = static_cast<float>(m_pFont->GetFontSize());

		D3DXVECTOR2 NewPosition = m_PositionClient;
		NewPosition.x += m_pFont->GetSelPositionXInLine(LineData.LineSelPosition, LineData.LineText);
		NewPosition.y += static_cast<float>(m_pFont->GetFontSize() * LineData.LineIndex);

		
		m_pSelection->AddRectangle(NewSize, NewPosition);
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

	UpdateCaret();
	UpdateSelection();
}