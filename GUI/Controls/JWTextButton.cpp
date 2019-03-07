#include "JWTextButton.h"
#include "../../CoreBase/JWImage.h"
#include "../../CoreBase/JWWindow.h"

using namespace JWENGINE;

void JWTextButton::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type
	m_ControlType = EControlType::TextButton;

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	m_Background.Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_Background.SetPosition(m_Position);
	m_Background.SetSize(m_Size);
	m_Background.SetXRGB(m_Color_Normal);
	m_Background.SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);
}

void JWTextButton::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept
{
	if (m_bShouleUseToggleSelection)
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

		if (p_input_state->MouseLeftFirstPressed)
		{
			if (b_mouse_down_in_rect)
			{
				if (ppControlWithMouse)
				{
					*ppControlWithMouse = this;
				}

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

				if (m_ControlState != EControlState::Pressed)
				{
					m_ControlState = EControlState::Pressed;
				}
				else
				{
					m_ControlState = EControlState::Normal;
				}
			}
		}
		else
		{
			if (b_mouse_in_rect)
			{
				if (ppControlWithMouse)
				{
					*ppControlWithMouse = this;
				}

				if (m_ControlState != EControlState::Pressed)
				{
					m_ControlState = EControlState::Hover;
				}
			}
			else
			{
				if (m_ControlState != EControlState::Pressed)
				{
					m_ControlState = EControlState::Normal;
				}
			}
		}
	}
	else
	{
		JWControl::UpdateControlState(ppControlWithMouse, ppControlWithFocus);
	}
}

void JWTextButton::Draw() noexcept
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_Background.SetXRGB(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		m_Background.SetXRGB(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		m_Background.SetXRGB(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	m_Background.Draw();

	JWControl::EndDrawing();
}

auto JWTextButton::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	JWControl::SetPosition(Position);

	m_Background.SetPosition(m_Position);

	return this;
}

auto JWTextButton::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl*
{
	JWControl::SetSize(Size);

	m_Background.SetSize(m_Size);

	return this;
}

auto JWTextButton::ShouldUseToggleSelection(bool Value) noexcept->JWControl*
{
	m_bShouleUseToggleSelection = Value;

	return this;
}

void JWTextButton::KillFocus() noexcept
{
	EControlState temp_current_state = m_ControlState;

	JWControl::KillFocus();

	if (m_bShouleUseToggleSelection)
	{
		// IF,
		// this is a toggle-able JWTextButton, and it is pressed,
		// it must keep pressed even when the focus is lost.

		if (temp_current_state == EControlState::Pressed)
		{
			m_ControlState = EControlState::Pressed;
		}
	}
}