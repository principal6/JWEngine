#include "JWRadioBox.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"

using namespace JWENGINE;

JWRadioBox::JWRadioBox()
{
	m_pBackground = nullptr;

	m_bChecked = false;
}

auto JWRadioBox::Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(pWindow, BaseDir, Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pWindow, ms_BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		
		//m_pBackground->SetXRGB(D3DCOLOR_XRGB(255, 255, 255));
		m_pBackground->SetTexture(GUI_TEXTURE_FILENAME);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	// @Warning:
	// 'D3DXVECTOR2 Size' is not used but GUI_BUTTON_SIZE
	SetSize(GUI_BUTTON_SIZE);
	SetPosition(m_PositionClient);

	// Set default font alignment
	m_pFont->SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_Type = EControlType::RadioBox;

	return EError::OK;
}

void JWRadioBox::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
}

void JWRadioBox::Draw()
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
		//m_bChecked = !m_bChecked;
		break;
	default:
		break;
	}

	JWControl::Draw();

	if (m_bChecked)
	{
		m_pBackground->SetAtlasUV(D3DXVECTOR2(GUI_BUTTON_SIZE.x, GUI_BUTTON_SIZE.y * 2), GUI_BUTTON_SIZE);
	}
	else
	{
		m_pBackground->SetAtlasUV(D3DXVECTOR2(GUI_BUTTON_SIZE.x * 2, GUI_BUTTON_SIZE.y * 2), GUI_BUTTON_SIZE);
	}

	m_pBackground->Draw();

	// RadioBox never has border
	/*
	m_pBackground->DrawBoundingBox();
	*/

	m_pFont->Draw();
}

void JWRadioBox::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);
	m_pBackground->SetPosition(Position);
}

void JWRadioBox::SetCheckState(bool Value)
{
	m_bChecked = Value;
}

auto JWRadioBox::GetCheckState() const->bool
{
	return m_bChecked;
}