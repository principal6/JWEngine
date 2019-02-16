#include "JWTextButton.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWWindow.h"

using namespace JWENGINE;

JWTextButton::JWTextButton()
{
	// A text button would normally have its border.
	m_bShouldDrawBorder = true;

	m_pBackground = nullptr;

	m_bShouleUseToggleSelection = false;
}

auto JWTextButton::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
		return EError::CONTROL_NOT_CREATED;
	
	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pBackground->SetSize(m_Size);
		m_pBackground->SetPosition(m_Position);
		m_pBackground->SetXRGB(m_Color_Normal);
		m_pBackground->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_ControlType = EControlType::TextButton;

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

	return EError::OK;
}

void JWTextButton::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
}

void JWTextButton::UpdateControlState(JWControl** ppControlWithFocus)
{
	if (m_bShouleUseToggleSelection)
	{
		const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

		if (p_input_state->MouseLeftFirstPressed)
		{
			if (Static_IsMouseInRECT(p_input_state->MouseDownPosition, m_ControlRect))
			{
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
			if (Static_IsMouseInRECT(p_input_state->MousePosition, m_ControlRect))
			{
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
		JWControl::UpdateControlState(ppControlWithFocus);
	}
}

void JWTextButton::Draw()
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_pBackground->SetXRGB(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		m_pBackground->SetXRGB(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		m_pBackground->SetXRGB(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	m_pBackground->Draw();

	JWControl::EndDrawing();
}

void JWTextButton::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	if (m_pBackground)
	{
		m_pBackground->SetPosition(m_Position);
	}
}

void JWTextButton::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);

	if (m_pBackground)
	{
		m_pBackground->SetSize(m_Size);
	}
}

void JWTextButton::ShouldUseToggleSelection(bool Value)
{
	m_bShouleUseToggleSelection = Value;
}