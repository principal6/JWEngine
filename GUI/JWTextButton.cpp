#include "JWTextButton.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWWindow.h"

using namespace JWENGINE;

JWTextButton::JWTextButton()
{
	// A text button would normally have its border.
	m_bShouldDrawBorder = true;

	m_pBackground = nullptr;
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

void JWTextButton::UpdateControlState(const SMouseData& MouseData)
{
	m_UpdatedMousedata = MouseData;

	if (m_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
	{
		// Mouse pressed

		if (Static_IsMouseInRECT(MouseData.MouseDownPosition, m_ControlRect))
		{
			// Mouse down position is inside RECT
			if (Static_IsMouseInRECT(MouseData.MousePosition, m_ControlRect))
			{
				m_ControlState = EControlState::Pressed;
			}
			else
			{
				m_ControlState = EControlState::Hover;
			}
		}
		else if (Static_IsMouseInRECT(MouseData.MousePosition, m_ControlRect))
		{
			// Mouse position is inside RECT
			m_ControlState = EControlState::Hover;
		}
		else
		{
			// Mouse position is out of RECT
			m_ControlState = EControlState::Normal;
		}
	}
	else
	{
		// Mouse released

		if (Static_IsMouseInRECT(MouseData.MousePosition, m_ControlRect))
		{
			// Mouse position is inside RECT

			if (m_ControlState == EControlState::Pressed)
			{
				// IF:
				// the button was pressed before,
				// it is now clicked.
				m_ControlState = EControlState::Clicked;
			}
			else
			{
				// IF:
				// the button wasn't pressed before,
				// it's just hovered.
				m_ControlState = EControlState::Hover;
			}
		}
		else
		{
			// Mouse position is out of RECT
			m_ControlState = EControlState::Normal;
		}
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