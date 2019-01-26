#include "JWControl.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWFont.h"

using namespace JWENGINE;

// Static member variable
WSTRING JWControl::ms_BaseDir;
JWWindow* JWControl::ms_pWindow;

auto JWControl::Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (nullptr == (ms_pWindow = pWindow))
		return EError::NULLPTR_WINDOW;

	ms_BaseDir = BaseDir;

	// Craete font
	if (m_pFont = new JWFont)
	{
		if (JW_SUCCEEDED(m_pFont->Create(ms_pWindow, ms_BaseDir)))
		{
			m_pFont->MakeFont(DEFAULT_FONT);
		}
		else
		{
			return EError::FONT_NOT_CREATED;
		}
	}
	else
	{
		return EError::FONT_NOT_CREATED;
	}
	
	// Set control position and size
	m_PositionClient = Position;
	m_Size = Size;

	// Set control rect
	CalculateRECT();

	// Set control state
	m_State = EControlState::Normal;

	// Should draw border?
	m_bShouldDrawBorder = true;

	return EError::OK;
}

PROTECTED void JWControl::CalculateRECT()
{
	m_Rect.left = static_cast<int>(m_PositionClient.x);
	m_Rect.right = static_cast<int>(m_Rect.left + m_Size.x);
	m_Rect.top = static_cast<int>(m_PositionClient.y);
	m_Rect.bottom = static_cast<int>(m_Rect.top + m_Size.y);
}

void JWControl::Destroy()
{
	JW_DESTROY(m_pFont);
}

void JWControl::Update(const SMouseData& MouseData)
{
	if (IsMouseInRECT(MouseData.MousePosition, m_Rect))
	{
		// Mouse position is inside RECT
		if (MouseData.bMouseLeftButtonPressed)
		{
			if (IsMouseInRECT(MouseData.MouseDownPosition, m_Rect))
			{
				m_State = EControlState::Pressed;
			}
			else
			{
				m_State = EControlState::Hover;
			}
		}
		else
		{
			if (m_State == EControlState::Pressed)
			{
				// If it was pressed before, it is now clicked
				m_State = EControlState::Clicked;
			}
			else
			{
				// If it wasn't pressed before, then it's just hovered
				m_State = EControlState::Hover;
			}
		}
	}
	else
	{
		// Mouse position is out of RECT
		m_State = EControlState::Normal;
	}

	m_pFont->ClearText();
	m_pFont->AddText(m_Text, m_PositionClient, m_Size, m_FontColor);
}

void JWControl::SetPosition(D3DXVECTOR2 Position)
{
	m_PositionClient = Position;
	CalculateRECT();
}

void JWControl::SetSize(D3DXVECTOR2 Size)
{
	m_Size = Size;
	CalculateRECT();
}

void JWControl::SetText(WSTRING Text, DWORD FontColor)
{
	m_Text = Text;
	m_FontColor = FontColor;
}

auto JWControl::GetPosition()->D3DXVECTOR2
{
	return m_PositionClient;
}

auto JWControl::GetSize()->D3DXVECTOR2
{
	return m_Size;
}

auto JWControl::GetText()->WSTRING
{
	return m_Text;
}

void JWControl::ShouldDrawBorder(bool Value)
{
	m_bShouldDrawBorder = Value;
}

auto JWControl::GetState()->EControlState const
{
	return m_State;
}