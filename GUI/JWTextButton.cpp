#include "JWTextButton.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWWindow.h"

using namespace JWENGINE;

JWTextButton::JWTextButton()
{
	// A text button would normally have its border.
	m_bShouldDrawBorder = true;

	m_pImage = nullptr;
}

auto JWTextButton::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;
	
	if (m_pImage = new JWImage)
	{
		if (JW_FAILED(m_pImage->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pImage->SetSize(m_Size);
		m_pImage->SetPosition(m_PositionClient);
		m_pImage->SetXRGB(m_Color_Normal);
		m_pImage->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	// Set default font alignment
	m_pFont->SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_Type = EControlType::TextButton;

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

	return EError::OK;
}

void JWTextButton::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY_SMART(m_pImage);
}

void JWTextButton::UpdateControlState(const SMouseData& MouseData)
{
	m_UpdatedMousedata = MouseData;

	if (ms_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
	{
		// Mouse pressed

		if (Static_IsMouseInRECT(MouseData.MouseDownPosition, m_Rect))
		{
			// Mouse down position is inside RECT
			if (Static_IsMouseInRECT(MouseData.MousePosition, m_Rect))
			{
				m_ControlState = EControlState::Pressed;
			}
			else
			{
				m_ControlState = EControlState::Hover;
			}
		}
		else if (Static_IsMouseInRECT(MouseData.MousePosition, m_Rect))
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

		if (Static_IsMouseInRECT(MouseData.MousePosition, m_Rect))
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
		m_pImage->SetXRGB(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		m_pImage->SetXRGB(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		m_pImage->SetXRGB(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	m_pImage->Draw();

	// Draw text
	m_pFont->Draw();

	JWControl::EndDrawing();
}

void JWTextButton::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	if (m_pImage)
	{
		m_pImage->SetPosition(m_PositionClient);
	}
}

void JWTextButton::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);

	if (m_pImage)
	{
		m_pImage->SetSize(m_Size);
	}
}