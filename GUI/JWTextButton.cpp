#include "JWTextButton.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"

using namespace JWENGINE;

JWTextButton::JWTextButton()
{
	m_pImage = nullptr;

	m_Color_Normal = DEFAULT_COLOR_NORMAL;
	m_Color_Hover = DEFAULT_COLOR_HOVER;
	m_Color_Pressed = DEFAULT_COLOR_PRESSED;
}

auto JWTextButton::Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(pWindow, BaseDir, Position, Size)))
		return EError::CONTROL_NOT_CREATED;
	
	if (m_pImage = new JWImage)
	{
		if (JW_FAILED(m_pImage->Create(ms_pWindow, ms_BaseDir)))
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

	return EError::OK;
}

void JWTextButton::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY_SMART(m_pImage);
}

void JWTextButton::Draw()
{
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

	JWControl::Draw();

	m_pImage->Draw();
	if (m_bShouldDrawBorder)
	{
		m_pImage->DrawBoundingBox();
	}

	// Draw text
	m_pFont->Draw();
}

void JWTextButton::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);
	m_pImage->SetPosition(Position);
}

void JWTextButton::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);
	m_pImage->SetSize(Size);
}

void JWTextButton::SetButtonColor(EControlState State, DWORD Color)
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