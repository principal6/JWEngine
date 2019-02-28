#include "JWImageButton.h"
#include "../../CoreBase/JWImage.h"
#include "../../CoreBase/JWWindow.h"

using namespace JWENGINE;

JWImageButton::JWImageButton()
{
	// An image button would normally have its border.
	m_bShouldDrawBorder = true;

	m_pBackground = nullptr;
	m_pButtonImage = nullptr;

	m_ButtonImagePositionOffset = D3DXVECTOR2(0, 0);
	m_bHorzFlip = false;
	m_bVertFlip = false;

	m_ButtonSizeInTexture = D3DXVECTOR2(0, 0);
	m_NormalOffset = D3DXVECTOR2(0, 0);
	m_HoverOffset = D3DXVECTOR2(0, 0);
	m_PressedOffset = D3DXVECTOR2(0, 0);

	// Set default color for every control state.
	m_Color_Normal = DEFAULT_COLOR_NORMAL;
	m_Color_Hover = DEFAULT_COLOR_NORMAL;
	m_Color_Pressed = DEFAULT_COLOR_NORMAL;
}

auto JWImageButton::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pBackground->SetColor(DEFAULT_COLOR_NORMAL);
		m_pBackground->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	if (m_pButtonImage = new JWImage)
	{
		if (JW_FAILED(m_pButtonImage->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pButtonImage->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_ControlType = EControlType::ImageButton;

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);

	return EError::OK;
}

void JWImageButton::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
	JW_DESTROY(m_pButtonImage);
}

auto JWImageButton::MakeImageButton(const WSTRING& TextureAtlasFileName, const D3DXVECTOR2& ButtonSizeInTexture, const D3DXVECTOR2& NormalOffset,
	const D3DXVECTOR2& HoverOffset, const D3DXVECTOR2& PressedOffset)->JWControl*
{
	m_pButtonImage->SetTexture(TextureAtlasFileName);

	m_ButtonSizeInTexture = ButtonSizeInTexture;

	m_NormalOffset = NormalOffset;
	m_HoverOffset = HoverOffset;
	m_PressedOffset = PressedOffset;

	SetSize(m_Size);

	return this;
}

auto JWImageButton::MakeSystemArrowButton(const ESystemArrowDirection Direction)->JWControl*
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
	
	m_pButtonImage->SetTexture(m_pSharedData->Texture_GUI, &m_pSharedData->Texture_GUI_Info);

	m_ButtonSizeInTexture = GUI_BUTTON_SIZE;

	m_NormalOffset = D3DXVECTOR2(0, AtlasYOffset);
	m_HoverOffset = D3DXVECTOR2(GUI_BUTTON_SIZE.x, AtlasYOffset);
	m_PressedOffset = D3DXVECTOR2(GUI_BUTTON_SIZE.x * 2, AtlasYOffset);

	SetSize(m_Size);

	return this;
}

void JWImageButton::Draw()
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_pBackground->SetColor(m_Color_Normal);
		m_pButtonImage->SetAtlasUV(m_NormalOffset, m_ButtonSizeInTexture);
		break;
	case JWENGINE::Hover:
		m_pBackground->SetColor(m_Color_Hover);
		m_pButtonImage->SetAtlasUV(m_HoverOffset, m_ButtonSizeInTexture);
		break;
	case JWENGINE::Pressed:
		m_pBackground->SetColor(m_Color_Pressed);
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

	m_pBackground->Draw();

	m_pButtonImage->Draw();

	JWControl::EndDrawing();
}

auto JWImageButton::SetPosition(const D3DXVECTOR2& Position)->JWControl*
{
	JWControl::SetPosition(Position);

	if (m_pBackground)
	{
		m_pBackground->SetPosition(m_Position);
	}
	
	if (m_pButtonImage)
	{
		m_pButtonImage->SetPosition(m_Position + m_ButtonImagePositionOffset);
	}

	return this;
}

auto JWImageButton::SetSize(const D3DXVECTOR2& Size)->JWControl*
{
	D3DXVECTOR2 adjusted_size = Size;
	adjusted_size.x = max(adjusted_size.x, m_ButtonSizeInTexture.x);
	adjusted_size.y = max(adjusted_size.y, m_ButtonSizeInTexture.y);

	m_ButtonImagePositionOffset.x = (adjusted_size.x - m_ButtonSizeInTexture.x) / 2.0f;
	m_ButtonImagePositionOffset.y = (adjusted_size.y - m_ButtonSizeInTexture.y) / 2.0f;

	JWControl::SetSize(adjusted_size);

	if (m_pBackground)
	{
		m_pBackground->SetSize(m_Size);
	}

	if (m_pButtonImage)
	{
		m_pButtonImage->SetPosition(m_Position + m_ButtonImagePositionOffset);
	}

	return this;
}