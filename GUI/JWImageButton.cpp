#include "JWImageButton.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"

using namespace JWENGINE;

JWImageButton::JWImageButton()
{
	m_pBackground = nullptr;
	m_pButtonImage = nullptr;

	m_ButtonImagePositionOffset = D3DXVECTOR2(0, 0);
	m_bHorzFlip = false;
	m_bVertFlip = false;

	m_ButtonSizeInTexture = D3DXVECTOR2(0, 0);
	m_NormalOffset = D3DXVECTOR2(0, 0);
	m_HoverOffset = D3DXVECTOR2(0, 0);
	m_PressedOffset = D3DXVECTOR2(0, 0);
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

void JWImageButton::MakeImageButton(WSTRING TextureAtlasFileName, D3DXVECTOR2 ButtonSizeInTexture, D3DXVECTOR2 NormalOffset,
	D3DXVECTOR2 HoverOffset, D3DXVECTOR2 PressedOffset)
{
	m_pButtonImage->SetTexture(GUI_TEXTURE_FILENAME);

	m_ButtonSizeInTexture = ButtonSizeInTexture;

	m_NormalOffset = NormalOffset;
	m_HoverOffset = HoverOffset;
	m_PressedOffset = PressedOffset;

	SetSize(m_Size);
}

void JWImageButton::MakeSystemArrowButton(ESystemArrowDirection Direction)
{
	float AtlasYOffset = 0;

	switch (Direction)
	{
	case JWENGINE::ESystemArrowDirection::Left:
		AtlasYOffset = GUI_BUTTON_SIZE.y;
		m_bHorzFlip = false;
		break;
	case JWENGINE::ESystemArrowDirection::Right:
		AtlasYOffset = GUI_BUTTON_SIZE.y;
		m_bHorzFlip = true;
		break;
	case JWENGINE::ESystemArrowDirection::Up:
		m_bVertFlip = false;
		break;
	case JWENGINE::ESystemArrowDirection::Down:
		m_bVertFlip = true;
		break;
	}

	MakeImageButton(GUI_TEXTURE_FILENAME, GUI_BUTTON_SIZE, D3DXVECTOR2(0, AtlasYOffset),
		D3DXVECTOR2(GUI_BUTTON_SIZE.x, AtlasYOffset), D3DXVECTOR2(GUI_BUTTON_SIZE.x * 2, AtlasYOffset));
}

void JWImageButton::Draw()
{
	switch (m_State)
	{
	case JWENGINE::Normal:
		m_pButtonImage->SetAtlasUV(m_NormalOffset, m_ButtonSizeInTexture);
		break;
	case JWENGINE::Hover:
		m_pButtonImage->SetAtlasUV(m_HoverOffset, m_ButtonSizeInTexture);
		break;
	case JWENGINE::Pressed:
		m_pButtonImage->SetAtlasUV(m_PressedOffset, m_ButtonSizeInTexture);
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
	m_pButtonImage->SetPosition(Position + m_ButtonImagePositionOffset);
}

void JWImageButton::SetSize(D3DXVECTOR2 Size)
{
	if (Size.x <= m_ButtonSizeInTexture.x)
		Size.x = m_ButtonSizeInTexture.x;

	if (Size.y <= m_ButtonSizeInTexture.y)
		Size.y = m_ButtonSizeInTexture.y;

	m_ButtonImagePositionOffset.x = (Size.x - m_ButtonSizeInTexture.x) / 2.0f;
	m_ButtonImagePositionOffset.y = (Size.y - m_ButtonSizeInTexture.y) / 2.0f;

	JWControl::SetSize(Size);
	m_pBackground->SetSize(Size);
	m_pButtonImage->SetPosition(m_PositionClient + m_ButtonImagePositionOffset);
}