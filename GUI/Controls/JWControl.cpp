#include "JWControl.h"
#include "../../CoreBase/JWWindow.h"
#include "../../CoreBase/JWText.h"
#include "../../CoreBase/JWLine.h"
#include "../JWGUIWindow.h"
#include "JWScrollBar.h"

using namespace JWENGINE;

PROTECTED void JWControl::CalculateControlRect() noexcept
{
	m_ControlRect.left = static_cast<int>(m_Position.x);
	m_ControlRect.right = static_cast<int>(m_ControlRect.left + m_Size.x);
	m_ControlRect.top = static_cast<int>(m_Position.y);
	m_ControlRect.bottom = static_cast<int>(m_ControlRect.top + m_Size.y);
}

void JWControl::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData)
{
	m_Position = Position;
	m_AbsolutePosition = Position;
	m_CalculatedTextPosition = Position;
	m_Size = Size;

	m_pSharedData = &SharedData;

	// Craete line for border
	m_pBorderLine.Create(m_pSharedData->pWindow->GetDevice());
	m_pBorderLine.AddBox(D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 0), DEFAULT_COLOR_BORDER);
	m_pBorderLine.AddEnd();

	// Update border position and size.
	UpdateBorderPositionAndSize();

	// Set control rect.
	CalculateControlRect();

	// Get the original viewport to reset it later
	m_pSharedData->pWindow->GetDevice()->GetViewport(&m_OriginalViewport);
}

void JWControl::BeginDrawing() noexcept
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
		if (m_bShouldUseViewport)
		{
			m_pSharedData->pWindow->GetDevice()->SetViewport(&m_ControlViewport);
		}

		m_pBorderLine.Draw();
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

auto JWControl::SetText(const WSTRING& Text) noexcept->JWControl&
{
	m_Text = Text;

	SetTextAlignment(m_HorizontalAlignment, m_VerticalAlignment);

	return *this;
}

auto JWControl::GetText(WSTRING& OutText) noexcept->JWControl&
{
	OutText = m_Text;

	return *this;
}

auto JWControl::SetTextAlignment(EHorizontalAlignment HorizontalAlignment, EVerticalAlignment VerticalAlignment) noexcept->JWControl&
{
	SetTextHorizontalAlignment(HorizontalAlignment);
	SetTextVerticalAlignment(VerticalAlignment);

	return *this;
}

auto JWControl::SetTextHorizontalAlignment(EHorizontalAlignment Alignment) noexcept->JWControl&
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

	return *this;
}

auto JWControl::SetTextVerticalAlignment(EVerticalAlignment Alignment) noexcept->JWControl&
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

	return *this;
}

auto JWControl::SetFontColor(DWORD Color) noexcept->JWControl&
{
	m_FontColor = Color;

	return *this;
}

auto JWControl::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl&
{
	m_AbsolutePosition = Position;

	m_Position = m_AbsolutePosition;

	if (m_pParentControl)
	{
		// If parent control position is offset, menubar height is included!

		if (m_bShouldBeOffsetByParent)
		{
			m_Position += m_pParentControl->GetPosition();
		}
	}
	else
	{
		if (m_pSharedData->pGUIWindow->HasMenuBar())
		{
			if (m_bShouldBeOffsetByMenuBar)
			{
				m_Position.y += m_pSharedData->pGUIWindow->GetMenuBarHeight();
			}
		}
	}

	// Set the text's default position.
	m_CalculatedTextPosition = m_Position;

	// Update the text's alignment.
	SetTextAlignment(m_HorizontalAlignment, m_VerticalAlignment);

	UpdateBorderPositionAndSize();

	CalculateControlRect();

	UpdateViewport();

	return *this;
}

auto JWControl::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl&
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

	return *this;
}

auto JWControl::SetBorderColor(DWORD Color) noexcept->JWControl&
{
	m_pBorderLine.SetBoxColor(Color);
	
	ShouldDrawBorder(true);

	m_bBorderColorSet = true;

	return *this;
}

auto JWControl::SetBorderColor(DWORD ColorA, DWORD ColorB) noexcept->JWControl&
{
	m_pBorderLine.SetBoxColor(ColorA, ColorB);

	m_bBorderColorSet = true;

	return *this;
}

auto JWControl::SetBackgroundColor(DWORD Color) noexcept->JWControl&
{
	m_Color_Normal = Color;
	m_Color_Hover = Color;
	m_Color_Pressed = Color;

	return *this;
}

auto JWControl::GetPosition() const noexcept->const D3DXVECTOR2
{
	return m_Position;
}

auto JWControl::GetAbsolutePosition() const noexcept->const D3DXVECTOR2
{
	return m_AbsolutePosition;
}

auto JWControl::GetSize() const noexcept->const D3DXVECTOR2
{
	return m_Size;
}

auto JWControl::GetControlType() const noexcept->EControlType
{
	return m_ControlType;
}

auto JWControl::GetControlState() const noexcept->EControlState
{
	return m_ControlState;
}

auto JWControl::SetParentControl(JWControl* pParentControl) noexcept->JWControl&
{
	m_pParentControl = pParentControl;

	return *this;
}

auto JWControl::HasParentControl() noexcept->bool
{
	if (m_pParentControl)
	{
		return true;
	}

	return false;
}

auto JWControl::OnMouseHover() const noexcept->bool
{
	if (m_ControlState == EControlState::Hover)
	{
		return true;
	}

	return false;
}

auto JWControl::OnMousePressed() const noexcept->bool
{
	if (m_ControlState == EControlState::Pressed)
	{
		return true;
	}
	return false;
}

auto JWControl::OnMouseCliked() const noexcept->bool
{
	if (m_ControlState == EControlState::Clicked)
	{
		return true;
	}
	return false;
}

PROTECTED void JWControl::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept
{
	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	bool b_mouse_in_rect = Static_IsMouseInViewPort(p_input_state->MousePosition, m_ControlViewport);
	bool b_mouse_down_in_rect = Static_IsMouseInViewPort(p_input_state->MouseDownPosition, m_ControlViewport);

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

				m_pAttachedScrollBar->SetScrollPosition(static_cast<size_t>(current_scroll_position));

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

auto JWControl::AttachScrollBar(const JWControl& ScrollBar)->JWControl&
{
	if (this == &ScrollBar)
	{
		// You can't attach yourself!
		return *this;
	}
	else if (ScrollBar.GetControlType() != EControlType::ScrollBar)
	{
		// Only scroll-bars can be attached.
		return *this;
	}
	else
	{
		JWControl* p_scrollbar_control = const_cast<JWControl*>(&ScrollBar);
		m_pAttachedScrollBar = static_cast<JWScrollBar*>(p_scrollbar_control);
	}

	return *this;
}

auto JWControl::DetachScrollBar() noexcept->JWControl&
{
	m_pAttachedScrollBar = nullptr;

	return *this;
}

PROTECTED void JWControl::UpdateBorderPositionAndSize() noexcept
{
	D3DXVECTOR2 border_position = m_Position;

	D3DXVECTOR2 border_size = m_Size;
	border_size.x -= 1.0f;
	border_size.y -= 1.0f;

	m_pBorderLine.SetBox(border_position, border_size);
}

PROTECTED void JWControl::UpdateViewport() noexcept
{
	m_ControlViewport = m_OriginalViewport;
	
	float negative_offset_x = 0;
	float negative_offset_y = 0;
	if (m_Position.x < 0)
	{
		negative_offset_x = m_Position.x;
	}
	if (m_Position.y < 0)
	{
		negative_offset_y = m_Position.y;
	}

	m_ControlViewport.X = static_cast<DWORD>(max(m_Position.x, 0));
	m_ControlViewport.Y = static_cast<DWORD>(max(m_Position.y, 0));

	m_ControlViewport.Width = static_cast<DWORD>(m_Size.x + negative_offset_x);
	m_ControlViewport.Height = static_cast<DWORD>(m_Size.y + negative_offset_y);

	if (m_pParentControl)
	{
		m_pParentControl->UpdateChildViewport(m_ControlViewport);
	}
}

PROTECTED void JWControl::UpdateChildViewport(D3DVIEWPORT9& Viewport)
{
	Viewport.X = max(Viewport.X, m_ControlViewport.X);
	Viewport.Y = max(Viewport.Y, m_ControlViewport.Y);

	auto bottom = Viewport.Y + Viewport.Height;
	auto right = Viewport.X + Viewport.Width;

	auto bottom_cmp = m_ControlViewport.Y + m_ControlViewport.Height;
	auto right_cmp = m_ControlViewport.X + m_ControlViewport.Width;

	right = min(right, right_cmp);
	bottom = min(bottom, bottom_cmp);

	Viewport.Width = right - Viewport.X;
	Viewport.Height = bottom - Viewport.Y;
}

void JWControl::SetControlState(EControlState State) noexcept
{
	m_ControlState = State;
}

void JWControl::SetControlStateColor(EControlState State, DWORD Color) noexcept
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

void JWControl::Focus() noexcept
{
	m_bHasFocus = true;

	if (!m_bBorderColorSet)
	{
		m_pBorderLine.SetBoxColor(DEFAULT_COLOR_BORDER_ACTIVE);
	}
}

void JWControl::KillFocus() noexcept
{
	m_bHasFocus = false;

	m_ControlState = EControlState::Normal;

	if (!m_bBorderColorSet)
	{
		m_pBorderLine.SetBoxColor(DEFAULT_COLOR_BORDER);
	}
}

auto JWControl::ShouldDrawBorder(bool Value) noexcept->JWControl&
{
	m_bShouldDrawBorder = Value;

	return *this;
}

auto JWControl::ShouldUseViewport(bool Value) noexcept->JWControl&
{
	m_bShouldUseViewport = Value;

	return *this;
}

auto JWControl::ShouldBeOffsetByMenuBar(bool Value) noexcept->JWControl&
{
	m_bShouldBeOffsetByMenuBar = Value;

	SetPosition(m_AbsolutePosition);
	SetSize(m_Size);

	return *this;
}

auto JWControl::ShouldBeOffsetByParent(bool Value) noexcept->JWControl&
{
	m_bShouldBeOffsetByParent = Value;

	SetPosition(m_AbsolutePosition);
	SetSize(m_Size);

	return *this;
}