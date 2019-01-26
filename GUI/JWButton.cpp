#include "JWButton.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWFont.h"

using namespace JWENGINE;

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

	return EError::OK;
}

void JWButton::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY_SMART(m_pImage);
}

void JWButton::Update(const SMouseData& MouseData)
{
	JWControl::Update(MouseData);

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
}

void JWButton::Draw()
{
	m_pImage->Draw();
	if (m_bShouldDrawBorder)
	{
		m_pImage->DrawBoundingBox();
	}
	m_pFont->Draw();
}