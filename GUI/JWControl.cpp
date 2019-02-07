#include "JWControl.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWLine.h"

using namespace JWENGINE;

// Static member variable
const SGUISharedData* JWControl::ms_pSharedData;

JWControl::JWControl()
{
	m_pFont = nullptr;

	// Set default color for each control state.
	m_Color_Normal = DEFAULT_COLOR_NORMAL;
	m_Color_Hover = DEFAULT_COLOR_HOVER;
	m_Color_Pressed = DEFAULT_COLOR_PRESSED;

	m_PositionClient = D3DXVECTOR2(0, 0);
	m_Size = D3DXVECTOR2(0, 0);
	m_Rect = { 0, 0, 0, 0 };

	m_Type = EControlType::NotDefined;
	m_ControlState = EControlState::Normal;

	m_bShouldDrawBorder = false;
	m_bShouldUseViewport = true;
	m_bHasFocus = false;

	m_MousePosition = { 0, 0 };
}

void JWControl::SetSharedData(const SGUISharedData* SharedData)
{
	ms_pSharedData = SharedData;
}

auto JWControl::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	// Craete font
	if (m_pFont = new JWFont)
	{
		if (JW_SUCCEEDED(m_pFont->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
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

	// Craete line for border
	if (m_pBorderLine = new JWLine)
	{
		if (JW_SUCCEEDED(m_pBorderLine->Create(ms_pSharedData->pWindow->GetDevice())))
		{
			m_pBorderLine->AddBox(D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 0), DEFAULT_COLOR_BORDER);
			m_pBorderLine->AddEnd();
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

	// Set control position and size.
	m_PositionClient = Position;
	m_Size = Size;

	// Update border position and size.
	UpdateBorderPositionAndSize();

	// Set control rect.
	CalculateRECT();

	// Get the original viewport to reset it later
	ms_pSharedData->pWindow->GetDevice()->GetViewport(&m_OriginalViewport);

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
	JW_DESTROY(m_pBorderLine);
}

auto JWControl::IsMouseOver(const SMouseData& MouseData)->bool
{
	return Static_IsMouseInRECT(MouseData.MousePosition, m_Rect);
}

auto JWControl::IsMousePressed(const SMouseData& MouseData)->bool
{
	return Static_IsMouseInRECT(MouseData.MouseDownPosition, m_Rect);
}

void JWControl::OnMouseMove(LPARAM MousePosition)
{
	m_MousePosition.x = GET_X_LPARAM(MousePosition);
	m_MousePosition.y = GET_Y_LPARAM(MousePosition);
}

void JWControl::OnMouseDown(LPARAM MousePosition)
{
	m_MousePosition.x = GET_X_LPARAM(MousePosition);
	m_MousePosition.y = GET_Y_LPARAM(MousePosition);
}

void JWControl::OnMouseUp(LPARAM MousePosition)
{
	m_MousePosition.x = GET_X_LPARAM(MousePosition);
	m_MousePosition.y = GET_Y_LPARAM(MousePosition);
}

void JWControl::UpdateControlState(const SMouseData& MouseData)
{
	m_UpdatedMousedata = MouseData;

	if (Static_IsMouseInRECT(MouseData.MousePosition, m_Rect))
	{
		// Mouse position is inside RECT
		if (ms_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
		{
			if (Static_IsMouseInRECT(MouseData.MouseDownPosition, m_Rect))
			{
				m_ControlState = EControlState::Pressed;
			}
			else
			{
				m_ControlState = EControlState::Hover;
			}
		}
		else
		{
			if (m_ControlState == EControlState::Pressed)
			{
				// If it was pressed before, it is now clicked
				m_ControlState = EControlState::Clicked;
			}
			else
			{
				// If it wasn't pressed before, then it's just hovered
				m_ControlState = EControlState::Hover;
			}
		}
	}
	else
	{
		// Mouse position is out of RECT
		m_ControlState = EControlState::Normal;
	}
}

void JWControl::BeginDrawing()
{
	if (m_bShouldUseViewport)
	{
		ms_pSharedData->pWindow->GetDevice()->SetViewport(&m_ControlViewport);
	}
}

void JWControl::EndDrawing()
{
	if (m_bShouldDrawBorder)
	{
		m_pBorderLine->Draw();
	}

	if (m_bShouldUseViewport)
	{
		ms_pSharedData->pWindow->GetDevice()->SetViewport(&m_OriginalViewport);
	}
}

PROTECTED void JWControl::UpdateBorderPositionAndSize()
{
	D3DXVECTOR2 border_position = m_PositionClient;

	D3DXVECTOR2 border_size = m_Size;
	border_size.x -= 1.0f;
	border_size.y -= 1.0f;

	m_pBorderLine->SetBox(border_position, border_size);
}

PROTECTED void JWControl::UpdateText()
{
	m_pFont->ClearText();
	m_pFont->SetText(m_Text, m_PositionClient, m_Size);
}

PROTECTED void JWControl::UpdateViewport()
{
	m_ControlViewport = m_OriginalViewport;
	m_ControlViewport.X = static_cast<DWORD>(m_PositionClient.x);
	m_ControlViewport.Y = static_cast<DWORD>(m_PositionClient.y);
	m_ControlViewport.Width = static_cast<DWORD>(m_Size.x);
	m_ControlViewport.Height = static_cast<DWORD>(m_Size.y);
}

void JWControl::SetState(EControlState State)
{
	m_ControlState = State;
}

void JWControl::SetStateColor(EControlState State, DWORD Color)
{
	switch (State)
	{
	case JWENGINE::Normal:
		m_Color_Normal = Color;
		break;
	case JWENGINE::Hover:
		m_Color_Hover = Color;
		break;
	case JWENGINE::Pressed:
		m_Color_Pressed = Color;
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}
}

void JWControl::SetPosition(D3DXVECTOR2 Position)
{
	m_PositionClient = Position;

	UpdateBorderPositionAndSize();

	CalculateRECT();

	UpdateViewport();
}

void JWControl::SetSize(D3DXVECTOR2 Size)
{
	m_Size = Size;

	UpdateBorderPositionAndSize();

	CalculateRECT();

	UpdateViewport();
}

void JWControl::SetText(WSTRING Text)
{
	m_Text = Text;

	UpdateText();
}

void JWControl::SetBorderColor(DWORD Color)
{
	m_pBorderLine->SetBoxColor(Color);
}

void JWControl::SetBorderColor(DWORD ColorA, DWORD ColorB)
{
	m_pBorderLine->SetBoxColor(ColorA, ColorB);
}

void JWControl::Focus()
{
	m_bHasFocus = true;
}

void JWControl::KillFocus()
{
	m_bHasFocus = false;
}

void JWControl::SetAlignment(EHorizontalAlignment HorizontalAlignment, EVerticalAlignment VerticalAlignment)
{
	m_pFont->SetAlignment(HorizontalAlignment, VerticalAlignment);
}

void JWControl::SetHorizontalAlignment(EHorizontalAlignment Alignment)
{
	m_pFont->SetHorizontalAlignment(Alignment);
}

void JWControl::SetVerticalAlignment(EVerticalAlignment Alignment)
{
	m_pFont->SetVerticalAlignment(Alignment);
}

void JWControl::SetFontAlpha(BYTE Alpha)
{
	m_pFont->SetFontAlpha(Alpha);
}

void JWControl::SetFontXRGB(DWORD XRGB)
{
	m_pFont->SetFontXRGB(XRGB);
}

auto JWControl::GetState() const->EControlState
{
	return m_ControlState;
}

auto JWControl::GetPosition()->D3DXVECTOR2
{
	D3DXVECTOR2 Result = m_PositionClient;

	return Result;
}

auto JWControl::GetSize()->D3DXVECTOR2
{
	return m_Size;
}

auto JWControl::GetText()->WSTRING
{
	return m_Text;
}

auto JWControl::GetControlType() const->EControlType
{
	return m_Type;
}

auto JWControl::GetClientMouseDownPosition() const->POINT
{
	POINT Result{ 0, 0 };
	Result.x = m_UpdatedMousedata.MouseDownPosition.x - static_cast<LONG>(m_PositionClient.x);
	Result.y = m_UpdatedMousedata.MouseDownPosition.y - static_cast<LONG>(m_PositionClient.y);
	return Result;
}

void JWControl::ShouldDrawBorder(bool Value)
{
	m_bShouldDrawBorder = Value;
}

void JWControl::ShouldUseViewport(bool Value)
{
	m_bShouldUseViewport = Value;
}