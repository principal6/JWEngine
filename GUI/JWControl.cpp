#include "JWControl.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWLine.h"
#include "JWScrollBar.h"

using namespace JWENGINE;

// Static member variable
const SGUISharedData* JWControl::ms_pSharedData;
JWFont* JWControl::ms_pFont = nullptr;

JWControl::JWControl()
{
	m_pBorderLine = nullptr;
	m_pAttachedScrollBar = nullptr;

	// Set default color for each control state.
	m_Color_Normal = DEFAULT_COLOR_NORMAL;
	m_Color_Hover = DEFAULT_COLOR_HOVER;
	m_Color_Pressed = DEFAULT_COLOR_PRESSED;

	m_PositionClient = D3DXVECTOR2(0, 0);
	m_Size = D3DXVECTOR2(0, 0);
	m_ControlRect = { 0, 0, 0, 0 };

	m_ControlType = EControlType::NotDefined;
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
	// Create static font, if it doens't exist.
	if (ms_pFont == nullptr)
	{
		if (ms_pFont = new JWFont)
		{
			if (JW_SUCCEEDED(ms_pFont->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
			{
				ms_pFont->MakeFont(DEFAULT_FONT);
			}
			else
			{
				return EError::FONT_NOT_CREATED;
			}
		}
		else
		{
			return EError::ALLOCATION_FAILURE;
		}
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
			return EError::LINE_NOT_CREATED;
		}
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Set control position and size.
	m_PositionClient = Position;
	m_Size = Size;

	// Set text's default position.
	m_TextPosition = m_PositionClient;

	// Update border position and size.
	UpdateBorderPositionAndSize();

	// Set control rect.
	CalculateControlRect();

	// Get the original viewport to reset it later
	ms_pSharedData->pWindow->GetDevice()->GetViewport(&m_OriginalViewport);

	return EError::OK;
}

PROTECTED void JWControl::CalculateControlRect()
{
	m_ControlRect.left = static_cast<int>(m_PositionClient.x);
	m_ControlRect.right = static_cast<int>(m_ControlRect.left + m_Size.x);
	m_ControlRect.top = static_cast<int>(m_PositionClient.y);
	m_ControlRect.bottom = static_cast<int>(m_ControlRect.top + m_Size.y);
}

void JWControl::Destroy()
{
	JW_DESTROY(ms_pFont);
	JW_DESTROY(m_pBorderLine);
}

auto JWControl::IsMouseOver(const SMouseData& MouseData)->bool
{
	return Static_IsMouseInRECT(MouseData.MousePosition, m_ControlRect);
}

auto JWControl::IsMousePressed(const SMouseData& MouseData)->bool
{
	return Static_IsMouseInRECT(MouseData.MouseDownPosition, m_ControlRect);
}

auto JWControl::OnMouseHover() const->bool
{
	if (m_ControlState == EControlState::Hover)
	{
		return true;
	}
	return false;
}

auto JWControl::OnMousePressed() const->bool
{
	if (m_ControlState == EControlState::Pressed)
	{
		return true;
	}
	return false;
}

auto JWControl::OnMouseCliked() const->bool
{
	if (m_ControlState == EControlState::Clicked)
	{
		return true;
	}
	return false;
}

void JWControl::WindowMouseMove(LPARAM MousePosition)
{
	m_MousePosition.x = GET_X_LPARAM(MousePosition);
	m_MousePosition.y = GET_Y_LPARAM(MousePosition);
}

void JWControl::WindowMouseDown(LPARAM MousePosition)
{
	m_MousePosition.x = GET_X_LPARAM(MousePosition);
	m_MousePosition.y = GET_Y_LPARAM(MousePosition);
}

void JWControl::WindowMouseUp(LPARAM MousePosition)
{
	m_MousePosition.x = GET_X_LPARAM(MousePosition);
	m_MousePosition.y = GET_Y_LPARAM(MousePosition);
}

void JWControl::UpdateControlState(const SMouseData& MouseData)
{
	m_UpdatedMousedata = MouseData;

	if (Static_IsMouseInRECT(MouseData.MousePosition, m_ControlRect))
	{
		// Mouse position is inside control's RECT
		if (ms_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
		{
			if (Static_IsMouseInRECT(MouseData.MouseDownPosition, m_ControlRect))
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

		// Check mouse wheel
		if (MouseData.MouseWheeled)
		{
			if (m_pAttachedScrollBar)
			{
				// IF,
				// this control has an attached ScrollBar.

				long long current_scroll_position = m_pAttachedScrollBar->GetScrollPosition();

				if (MouseData.MouseWheeled > 0)
				{
					current_scroll_position--;
					current_scroll_position = max(current_scroll_position, 0);

					m_pAttachedScrollBar->SetScrollPosition(current_scroll_position);
				}
				else
				{
					current_scroll_position++;

					m_pAttachedScrollBar->SetScrollPosition(current_scroll_position);
				}

				m_pAttachedScrollBar->SetState(EControlState::Hover);
			}
		}

	}
	else
	{
		// Mouse position is out of control's RECT
		m_ControlState = EControlState::Normal;
	}
}

void JWControl::AttachScrollBar(JWControl* pScrollBar)
{
	if (this == pScrollBar)
	{
		// You can't attach yourself!
		return;
	}

	m_pAttachedScrollBar = static_cast<JWScrollBar*>(pScrollBar);
}

void JWControl::DetachScrollBar()
{
	m_pAttachedScrollBar = nullptr;
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

	if (m_Text.length())
	{
		if (m_ControlType != EControlType::Edit)
		{
			ms_pFont->DrawInstantText(m_Text, m_TextPosition, m_HorizontalAlignment);
		}
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

	// Set text's default position.
	m_TextPosition = m_PositionClient;

	SetAlignment(m_HorizontalAlignment, m_VerticalAlignment);

	UpdateBorderPositionAndSize();

	CalculateControlRect();

	UpdateViewport();
}

void JWControl::SetSize(D3DXVECTOR2 Size)
{
	m_Size = Size;

	SetAlignment(m_HorizontalAlignment, m_VerticalAlignment);

	UpdateBorderPositionAndSize();

	CalculateControlRect();

	UpdateViewport();
}

void JWControl::SetText(WSTRING Text)
{
	m_Text = Text;

	SetAlignment(m_HorizontalAlignment, m_VerticalAlignment);
}

void JWControl::SetBorderColor(DWORD Color)
{
	m_pBorderLine->SetBoxColor(Color);
}

void JWControl::SetBorderColor(DWORD ColorA, DWORD ColorB)
{
	m_pBorderLine->SetBoxColor(ColorA, ColorB);
}

void JWControl::SetBackgroundColor(DWORD Color)
{
	m_Color_Normal = Color;
	m_Color_Hover = Color;
	m_Color_Pressed = Color;
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
	SetHorizontalAlignment(HorizontalAlignment);
	SetVerticalAlignment(VerticalAlignment);
}

void JWControl::SetHorizontalAlignment(EHorizontalAlignment Alignment)
{
	m_HorizontalAlignment = Alignment;

	switch (m_HorizontalAlignment)
	{
	case JWENGINE::EHorizontalAlignment::Left:
		m_TextPosition.x = m_PositionClient.x;
		break;
	case JWENGINE::EHorizontalAlignment::Center:
		m_TextPosition.x = m_PositionClient.x + m_Size.x / 2.0f;
		break;
	case JWENGINE::EHorizontalAlignment::Right:
		m_TextPosition.x = m_PositionClient.x + m_Size.x;
		break;
	default:
		break;
	}
}

void JWControl::SetVerticalAlignment(EVerticalAlignment Alignment)
{
	m_VerticalAlignment = Alignment;

	switch (m_VerticalAlignment)
	{
	case JWENGINE::EVerticalAlignment::Top:
		m_TextPosition.y = m_PositionClient.y;
		break;
	case JWENGINE::EVerticalAlignment::Middle:
		m_TextPosition.y = m_PositionClient.y + ((m_Size.y - ms_pFont->GetLineHeight()) / 2.0f);
		break;
	case JWENGINE::EVerticalAlignment::Bottom:
		m_TextPosition.y = m_PositionClient.y + m_Size.y - ms_pFont->GetLineHeight();
		break;
	default:
		break;
	}
}

void JWControl::SetFontColor(DWORD Color)
{
	ms_pFont->SetFontColor(Color);
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
	return m_ControlType;
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