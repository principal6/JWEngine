#include "JWCheckBox.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"

using namespace JWENGINE;

JWCheckBox::JWCheckBox()
{
	m_pBackground = nullptr;
	m_pCheckImage = nullptr;

	m_ButtonImageOffset = D3DXVECTOR2(0, 0);
	m_OffsetInAtlas = D3DXVECTOR2(0, 0);

	m_bChecked = false;
}

auto JWCheckBox::Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(pWindow, BaseDir, Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pWindow, ms_BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pBackground->SetXRGB(DEFAULT_COLOR_ALMOST_WHITE);
		m_pBackground->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	if (m_pCheckImage = new JWImage)
	{
		if (JW_FAILED(m_pCheckImage->Create(ms_pWindow, ms_BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		m_pCheckImage->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
		m_pCheckImage->SetTexture(GUI_TEXTURE_FILENAME);
		m_pCheckImage->SetAtlasUV(D3DXVECTOR2(0, GUI_BUTTON_SIZE.y * 2), GUI_BUTTON_SIZE);
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
	m_Type = EControlType::CheckBox;

	return EError::OK;
}

void JWCheckBox::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
	JW_DESTROY(m_pCheckImage);
}

void JWCheckBox::Draw()
{
	switch (m_State)
	{
	case JWENGINE::Normal:
		break;
	case JWENGINE::Hover:
		break;
	case JWENGINE::Pressed:
		break;
	case JWENGINE::Clicked:
		m_bChecked = !m_bChecked;
		break;
	default:
		break;
	}

	JWControl::Draw();

	m_pBackground->Draw();

	if (m_bChecked)
	{
		m_pCheckImage->Draw();
	}
	
	// Checkbox has always border
	m_pBackground->DrawBoundingBox();

	m_pFont->Draw();
}

void JWCheckBox::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);
	m_pBackground->SetPosition(Position);
	m_pCheckImage->SetPosition(Position + m_ButtonImageOffset);
}

void JWCheckBox::SetSize(D3DXVECTOR2 Size)
{
	if (Size.x <= GUI_BUTTON_SIZE.x)
		Size.x = GUI_BUTTON_SIZE.x;

	if (Size.y <= GUI_BUTTON_SIZE.y)
		Size.y = GUI_BUTTON_SIZE.y;

	m_ButtonImageOffset.x = (Size.x - GUI_BUTTON_SIZE.x) / 2.0f;
	m_ButtonImageOffset.y = (Size.y - GUI_BUTTON_SIZE.y) / 2.0f;

	JWControl::SetSize(Size);
	m_pBackground->SetSize(Size);
	m_pCheckImage->SetPosition(m_PositionClient + m_ButtonImageOffset);
}