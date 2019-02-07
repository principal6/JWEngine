#include "JWImageButton.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWWindow.h"

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

auto JWImageButton::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
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
		if (JW_FAILED(m_pButtonImage->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pButtonImage->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	// Set default font alignment
	m_pFont->SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_Type = EControlType::ImageButton;

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

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
	m_pButtonImage->SetTexture(TextureAtlasFileName);

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
	
	m_pButtonImage->SetTexture(ms_pSharedData->Texture_GUI, &ms_pSharedData->Texture_GUI_Info);

	m_ButtonSizeInTexture = GUI_BUTTON_SIZE;

	m_NormalOffset = D3DXVECTOR2(0, AtlasYOffset);
	m_HoverOffset = D3DXVECTOR2(GUI_BUTTON_SIZE.x, AtlasYOffset);
	m_PressedOffset = D3DXVECTOR2(GUI_BUTTON_SIZE.x * 2, AtlasYOffset);

	SetSize(m_Size);
}

void JWImageButton::UpdateControlState(const SMouseData& MouseData)
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

void JWImageButton::Draw()
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_pBackground->SetXRGB(m_Color_Normal);
		m_pButtonImage->SetAtlasUV(m_NormalOffset, m_ButtonSizeInTexture);
		break;
	case JWENGINE::Hover:
		m_pBackground->SetXRGB(m_Color_Hover);
		m_pButtonImage->SetAtlasUV(m_HoverOffset, m_ButtonSizeInTexture);
		break;
	case JWENGINE::Pressed:
		m_pBackground->SetXRGB(m_Color_Pressed);
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

	// Draw text
	m_pFont->Draw();

	JWControl::EndDrawing();
}

void JWImageButton::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	if (m_pBackground)
	{
		m_pBackground->SetPosition(Position);
	}
	
	if (m_pButtonImage)
	{
		m_pButtonImage->SetPosition(Position + m_ButtonImagePositionOffset);
	}
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

	if (m_pBackground)
	{
		m_pBackground->SetSize(Size);
	}

	if (m_pButtonImage)
	{
		m_pButtonImage->SetPosition(m_PositionClient + m_ButtonImagePositionOffset);
	}
}