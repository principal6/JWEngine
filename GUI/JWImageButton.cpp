#include "JWImageButton.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"

using namespace JWENGINE;

JWImageButton::JWImageButton()
{
	m_pBackground = nullptr;
	m_pButtonImage = nullptr;

	m_ButtonImageOffset = D3DXVECTOR2(0, 0);
	m_OffsetInAtlas = D3DXVECTOR2(0, 0);
	m_bHorzFlip = false;
	m_bVertFlip = false;
}

auto JWImageButton::Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(pWindow, BaseDir, Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pWindow, ms_BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pBackground->SetXRGB(DEFAULT_COLOR_NORMAL);
		m_pBackground->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	if (m_pButtonImage = new JWImage)
	{
		if (JW_FAILED(m_pButtonImage->Create(ms_pWindow, ms_BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pButtonImage->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
		m_pButtonImage->SetTexture(GUI_TEXTURE_FILENAME);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	SetSize(m_Size);
	SetPosition(m_PositionClient);
	
	// Set default font alignment
	m_pFont->SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_Type = EControlType::ImageButton;

	return EError::OK;
}

void JWImageButton::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
	JW_DESTROY(m_pButtonImage);
}

void JWImageButton::Draw()
{
	switch (m_State)
	{
	case JWENGINE::Normal:
		m_pButtonImage->SetAtlasUV(D3DXVECTOR2(0, m_OffsetInAtlas.y), GUI_BUTTON_SIZE);
		break;
	case JWENGINE::Hover:
		m_pButtonImage->SetAtlasUV(D3DXVECTOR2(GUI_BUTTON_SIZE.x, m_OffsetInAtlas.y), GUI_BUTTON_SIZE);
		break;
	case JWENGINE::Pressed:
		m_pButtonImage->SetAtlasUV(D3DXVECTOR2(GUI_BUTTON_SIZE.x * 2, m_OffsetInAtlas.y), GUI_BUTTON_SIZE);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	if (m_bHorzFlip)
	{
		m_pButtonImage->FlipHorizontal();
	}
	else if (m_bVertFlip)
	{
		m_pButtonImage->FlipVertical();
	}

	JWControl::Draw();

	m_pBackground->Draw();

	m_pButtonImage->Draw();
	if (m_bShouldDrawBorder)
	{
		m_pBackground->DrawBoundingBox();
	}
	m_pFont->Draw();
}

void JWImageButton::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);
	m_pBackground->SetPosition(Position);
	m_pButtonImage->SetPosition(Position + m_ButtonImageOffset);
}

void JWImageButton::SetSize(D3DXVECTOR2 Size)
{
	if (Size.x <= GUI_BUTTON_SIZE.x)
		Size.x = GUI_BUTTON_SIZE.x;

	if (Size.y <= GUI_BUTTON_SIZE.y)
		Size.y = GUI_BUTTON_SIZE.y;

	m_ButtonImageOffset.x = (Size.x - GUI_BUTTON_SIZE.x) / 2.0f;
	m_ButtonImageOffset.y = (Size.y - GUI_BUTTON_SIZE.y) / 2.0f;

	JWControl::SetSize(Size);
	m_pBackground->SetSize(Size);
	m_pButtonImage->SetPosition(m_PositionClient + m_ButtonImageOffset);
}

void JWImageButton::SetImageButtonDireciton(EImageButtonDirection Direction)
{
	m_bHorzFlip = false;
	m_bVertFlip = false;

	switch (Direction)
	{
	case JWENGINE::EImageButtonDirection::Left:
		m_OffsetInAtlas = D3DXVECTOR2(0, GUI_BUTTON_SIZE.y);
		break;
	case JWENGINE::EImageButtonDirection::Right:
		m_OffsetInAtlas = D3DXVECTOR2(0, GUI_BUTTON_SIZE.y);
		m_bHorzFlip = true;
		break;
	case JWENGINE::EImageButtonDirection::Up:
		m_OffsetInAtlas = D3DXVECTOR2(0, 0);
		break;
	case JWENGINE::EImageButtonDirection::Down:
		m_OffsetInAtlas = D3DXVECTOR2(0, 0);
		m_bVertFlip = true;
		break;
	default:
		break;
	}
}