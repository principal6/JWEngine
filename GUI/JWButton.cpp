#include "JWButton.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"

using namespace JWENGINE;

JWButton::JWButton()
{
	m_pImage = nullptr;
}

auto JWButton::Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(pWindow, BaseDir, Position, Size)))
		return EError::CONTROL_NOT_CREATED;
	
	if (m_pImage = new JWImage)
	{
		if (JW_FAILED(m_pImage->Create(ms_pWindow, ms_BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pImage->SetSize(m_Size);
		m_pImage->SetPosition(m_PositionClient);
		m_pImage->SetXRGB(DEFAULT_COLOR_NORMAL);
		m_pImage->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	// Set default font alignment
	m_pFont->SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_Type = EControlType::Button;

	return EError::OK;
}

void JWButton::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY_SMART(m_pImage);
}

void JWButton::Draw()
{
	switch (m_State)
	{
	case JWENGINE::Normal:
		m_pImage->SetXRGB(DEFAULT_COLOR_NORMAL);
		break;
	case JWENGINE::Hover:
		m_pImage->SetXRGB(DEFAULT_COLOR_HOVER);
		break;
	case JWENGINE::Pressed:
		m_pImage->SetXRGB(DEFAULT_COLOR_PRESSED);
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
	m_pFont->Draw();
}

void JWButton::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);
	m_pImage->SetPosition(Position);
}

void JWButton::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);
	m_pImage->SetSize(Size);
}