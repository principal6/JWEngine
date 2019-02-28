#include "JWControl.h"
#include "../../CoreBase/JWWindow.h"
#include "../../CoreBase/JWText.h"
#include "../../CoreBase/JWLine.h"
#include "../JWGUIWindow.h"
#include "JWScrollBar.h"

using namespace JWENGINE;

JWControl::JWControl()
{
	m_pBorderLine = nullptr;
	m_pAttachedScrollBar = nullptr;
	m_pParentControl = nullptr;

	// Set default color for each control state.
	m_Color_Normal = DEFAULT_COLOR_NORMAL;
	m_Color_Hover = DEFAULT_COLOR_HOVER;
	m_Color_Pressed = DEFAULT_COLOR_PRESSED;

	// Control type is decided when sub-class calls Create().
	m_ControlType = EControlType::NotDefined;

	// Default control state is Normal.
	m_ControlState = EControlState::Normal;

	m_ControlRect = { 0, 0, 0, 0 };
	m_Position = D3DXVECTOR2(0, 0);
	m_AbsolutePosition = D3DXVECTOR2(0, 0);
	m_Size = D3DXVECTOR2(0, 0);

	m_FontColor = DEFAULT_COLOR_FONT;

	m_bShouldDrawBorder = false;
	m_bShouldUseViewport = true;
	m_bShouldBeOffsetByMenuBar = true;
	m_bHasFocus = false;
}

auto JWControl::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->EError
{
	// Set shared data pointer.
	m_pSharedData = pSharedData;

	// Craete line for border
	if (m_pBorderLine = new JWLine)
	{
		if (JW_SUCCEEDED(m_pBorderLine->Create(m_pSharedData->pWindow->GetDevice())))
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
	m_Position = Position;
	m_AbsolutePosition = Position;
	m_Size = Size;

	// Set text's default position.
	m_CalculatedTextPosition = m_Position;

	// Update border position and size.
	UpdateBorderPositionAndSize();

	// Set control rect.
	CalculateControlRect();

	// Get the original viewport to reset it later
	m_pSharedData->pWindow->GetDevice()->GetViewport(&m_OriginalViewport);

	return EError::OK;
}

PROTECTED void JWControl::CalculateControlRect()
{
	m_ControlRect.left = static_cast<int>(m_Position.x);
	m_ControlRect.right = static_cast<int>(m_ControlRect.left + m_Size.x);
	m_ControlRect.top = static_cast<int>(m_Position.y);
	m_ControlRect.bottom = static_cast<int>(m_ControlRect.top + m_Size.y);
}

void JWControl::Destroy()
{
	JW_DESTROY(m_pBorderLine);
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

void JWControl::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus)
{
	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	bool b_mouse_in_rect = Static_IsMouseInRECT(p_input_state->MousePosition, m_ControlRect);
	bool b_mouse_down_in_rect = Static_IsMouseInRECT(p_input_state->MouseDownPosition, m_ControlRect);

	if (ppControlWithMouse)
	{
		// If a control already has mouse cursor on it,
		// no other controls can have it.
		if (*ppControlWithMouse)
		{
			b_mouse_in_rect = false;
			b_mouse_down_in_rect = false;
		}
	}

	if (b_mouse_in_rect)
	{
		// IF,
		// mouse cursor is inside the control's rect.

		if (ppControlWithMouse)
		{
			*ppControlWithMouse = this;
		}

		if (p_input_state->MouseLeftPressed)
		{
			// IF,
			// mouse left button is pressed.

			if (b_mouse_down_in_rect)
			{
				m_ControlState = EControlState::Pressed;

				if (ppControlWithFocus)
				{
					if (*ppControlWithFocus)
					{
						if ((*ppControlWithFocus) != this)
						{
							(*ppControlWithFocus)->KillFocus();
						}
					}

					*ppControlWithFocus = this;
					(*ppControlWithFocus)->Focus();
				}
			}
			else
			{
				m_ControlState = EControlState::Hover;
			}
		}
		else
		{
			// IF,
			// mouse left button is NOT pressed.

			if (m_ControlState == EControlState::Pressed)
			{
				// If it was pressed before, it is now clicked
				m_ControlState = EControlState::Clicked;

				if (ppControlWithFocus)
				{
					if (*ppControlWithFocus)
					{
						if ((*ppControlWithFocus) != this)
						{
							(*ppControlWithFocus)->KillFocus();
						}
					}

					*ppControlWithFocus = this;
					(*ppControlWithFocus)->Focus();
				}
			}
			else
			{
				// If it wasn't pressed before, then it's just hovered
				m_ControlState = EControlState::Hover;
			}
		}

		// Check mouse wheel
		if (p_input_state->MouseWheeled)
		{
			if (m_pAttachedScrollBar)
			{
				// IF,
				// this control has an attached ScrollBar.

				long long current_scroll_position = m_pAttachedScrollBar->GetScrollPosition();

				current_scroll_position -= (p_input_state->MouseWheeled / WHEEL_DELTA);
				current_scroll_position = max(current_scroll_position, 0);

				m_pAttachedScrollBar->SetScrollPosition(current_scroll_position);

				m_pAttachedScrollBar->SetControlState(EControlState::Hover);
			}
		}

	}
	else
	{
		// IF,
		// mouse cursor is out of the control's rect.

		m_ControlState = EControlState::Normal;

		if ((m_ControlType == EControlType::ImageButton) || (m_ControlType == EControlType::TextButton))
		{
			if (p_input_state->MouseLeftPressed)
			{
				if (b_mouse_down_in_rect)
				{
					m_ControlState = EControlState::Hover;
				}
			}
		}
	}
}

auto JWControl::AttachScrollBar(const JWControl* pScrollBar)->JWControl*
{
	if (pScrollBar == nullptr)
	{
		// There must be scroll-bar to attach.
	}
	else if (this == pScrollBar)
	{
		// You can't attach yourself!
	}
	else if (pScrollBar->GetControlType() != EControlType::ScrollBar)
	{
		// Only scroll-bars can be attached.
	}
	else
	{
		JWControl* p_scrollbar_control = const_cast<JWControl*>(pScrollBar);
		m_pAttachedScrollBar = static_cast<JWScrollBar*>(p_scrollbar_control);
	}

	return this;
}

auto JWControl::DetachScrollBar()->JWControl*
{
	m_pAttachedScrollBar = nullptr;

	return this;
}

void JWControl::BeginDrawing()
{
	if (m_bShouldUseViewport)
	{
		m_pSharedData->pWindow->GetDevice()->SetViewport(&m_ControlViewport);
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
			m_pSharedData->pText->DrawInstantText(m_Text, m_CalculatedTextPosition, m_HorizontalAlignment, m_FontColor);
		}
	}

	if (m_bShouldUseViewport)
	{
		m_pSharedData->pWindow->GetDevice()->SetViewport(&m_OriginalViewport);
	}
}

PROTECTED void JWControl::UpdateBorderPositionAndSize()
{
	D3DXVECTOR2 border_position = m_Position;

	D3DXVECTOR2 border_size = m_Size;
	border_size.x -= 1.0f;
	border_size.y -= 1.0f;

	m_pBorderLine->SetBox(border_position, border_size);
}

PROTECTED void JWControl::UpdateViewport()
{
	m_ControlViewport = m_OriginalViewport;
	m_ControlViewport.X = static_cast<DWORD>(m_Position.x);
	m_ControlViewport.Y = static_cast<DWORD>(m_Position.y);
	m_ControlViewport.Width = static_cast<DWORD>(m_Size.x);
	m_ControlViewport.Height = static_cast<DWORD>(m_Size.y);
}

auto JWControl::SetPosition(const D3DXVECTOR2& Position)->JWControl*
{
	m_Position = Position;

	if (m_pSharedData->pGUIWindow->HasMenuBar())
	{
		if (m_bShouldBeOffsetByMenuBar)
		{
			m_Position.y += m_pSharedData->pGUIWindow->GetMenuBarHeight();
		}
	}

	// Set the text's default position.
	m_CalculatedTextPosition = m_Position;

	// Update the text's alignment.
	SetTextAlignment(m_HorizontalAlignment, m_VerticalAlignment);

	UpdateBorderPositionAndSize();

	CalculateControlRect();

	UpdateViewport();

	return this;
}

auto JWControl::SetSize(const D3DXVECTOR2& Size)->JWControl*
{
	D3DXVECTOR2 adjusted_size = Size;

	// Limit minimum size.
	adjusted_size.x = max(adjusted_size.x, 0);
	adjusted_size.y = max(adjusted_size.y, 0);

	m_Size = adjusted_size;

	// Update the text's alignment.
	SetTextAlignment(m_HorizontalAlignment, m_VerticalAlignment);

	UpdateBorderPositionAndSize();

	CalculateControlRect();

	UpdateViewport();

	return this;
}

void JWControl::SetControlState(const EControlState State)
{
	m_ControlState = State;
}

void JWControl::SetControlStateColor(const EControlState State, const DWORD Color)
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

auto JWControl::SetBorderColor(const DWORD Color)->JWControl*
{
	m_pBorderLine->SetBoxColor(Color);

	return this;
}

auto JWControl::SetBorderColor(const DWORD ColorA, const DWORD ColorB)->JWControl*
{
	m_pBorderLine->SetBoxColor(ColorA, ColorB);

	return this;
}

auto JWControl::SetBackgroundColor(const DWORD Color)->JWControl*
{
	m_Color_Normal = Color;
	m_Color_Hover = Color;
	m_Color_Pressed = Color;

	return this;
}

auto JWControl::SetParentControl(const JWControl* pParentControl)->JWControl*
{
	m_pParentControl = pParentControl;

	return this;
}

auto JWControl::HasParentControl()->bool
{
	if (m_pParentControl)
	{
		return true;
	}

	return false;
}

void JWControl::Focus()
{
	m_bHasFocus = true;

	m_pBorderLine->SetBoxColor(DEFAULT_COLOR_BORDER_ACTIVE);
}

void JWControl::KillFocus()
{
	m_bHasFocus = false;

	m_ControlState = EControlState::Normal;

	m_pBorderLine->SetBoxColor(DEFAULT_COLOR_BORDER);
}

auto JWControl::SetText(const WSTRING& Text)->JWControl*
{
	m_Text = Text;

	SetTextAlignment(m_HorizontalAlignment, m_VerticalAlignment);

	return this;
}

auto JWControl::GetText(WSTRING& OutText)->JWControl*
{
	OutText = m_Text;

	return this;
}

auto JWControl::SetTextAlignment(const EHorizontalAlignment HorizontalAlignment, const EVerticalAlignment VerticalAlignment)->JWControl*
{
	SetTextHorizontalAlignment(HorizontalAlignment);
	SetTextVerticalAlignment(VerticalAlignment);

	return this;
}

auto JWControl::SetTextHorizontalAlignment(const EHorizontalAlignment Alignment)->JWControl*
{
	m_HorizontalAlignment = Alignment;

	switch (m_HorizontalAlignment)
	{
	case JWENGINE::EHorizontalAlignment::Left:
		m_CalculatedTextPosition.x = m_Position.x;
		break;
	case JWENGINE::EHorizontalAlignment::Center:
		m_CalculatedTextPosition.x = m_Position.x + m_Size.x / 2.0f;
		break;
	case JWENGINE::EHorizontalAlignment::Right:
		m_CalculatedTextPosition.x = m_Position.x + m_Size.x;
		break;
	default:
		break;
	}

	return this;
}

auto JWControl::SetTextVerticalAlignment(const EVerticalAlignment Alignment)->JWControl*
{
	m_VerticalAlignment = Alignment;

	switch (m_VerticalAlignment)
	{
	case JWENGINE::EVerticalAlignment::Top:
		m_CalculatedTextPosition.y = m_Position.y;
		break;
	case JWENGINE::EVerticalAlignment::Middle:
		m_CalculatedTextPosition.y = m_Position.y + ((m_Size.y - m_pSharedData->pText->GetLineHeight()) / 2.0f);
		break;
	case JWENGINE::EVerticalAlignment::Bottom:
		m_CalculatedTextPosition.y = m_Position.y + m_Size.y - m_pSharedData->pText->GetLineHeight();
		break;
	default:
		break;
	}

	return this;
}

auto JWControl::SetFontColor(const DWORD Color)->JWControl*
{
	m_FontColor = Color;

	return this;
}

auto JWControl::GetControlState() const->const EControlState
{
	return m_ControlState;
}

auto JWControl::GetPosition() const->const D3DXVECTOR2
{
	return m_Position;
}

auto JWControl::GetAbsolutePosition() const->const D3DXVECTOR2
{
	return m_AbsolutePosition;
}

auto JWControl::GetSize() const->const D3DXVECTOR2
{
	return m_Size;
}

auto JWControl::GetControlType() const->const EControlType
{
	return m_ControlType;
}

auto JWControl::ShouldDrawBorder(const bool Value)->JWControl*
{
	m_bShouldDrawBorder = Value;

	return this;
}

auto JWControl::ShouldUseViewport(const bool Value)->JWControl*
{
	m_bShouldUseViewport = Value;

	return this;
}

auto JWControl::ShouldBeOffsetByMenuBar(const bool Value)->JWControl*
{
	m_bShouldBeOffsetByMenuBar = Value;

	return this;
}