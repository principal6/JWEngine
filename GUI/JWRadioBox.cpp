#include "JWRadioBox.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"

using namespace JWENGINE;

JWRadioBox::JWRadioBox()
{
	// RadioBox never has border
	m_bShouldDrawBorder = false;

	m_pBackground = nullptr;

	m_bChecked = false;
}

auto JWRadioBox::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		
		//m_pBackground->SetXRGB(D3DCOLOR_XRGB(255, 255, 255));
		m_pBackground->SetTexture(ms_pSharedData->Texture_GUI, &ms_pSharedData->Texture_GUI_Info);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	// Set default font alignment
	m_pFont->SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_ControlType = EControlType::RadioBox;

	// Set control's size and position.
	// @ WARNING:
	// 'D3DXVECTOR2 Size' is not used but 'GUI_BUTTON_SIZE'
	SetSize(GUI_BUTTON_SIZE);
	SetPosition(Position);

	return EError::OK;
}

void JWRadioBox::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
}

void JWRadioBox::Draw()
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
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

	if (m_bChecked)
	{
		m_pBackground->SetAtlasUV(D3DXVECTOR2(GUI_BUTTON_SIZE.x, GUI_BUTTON_SIZE.y * 2), GUI_BUTTON_SIZE);
	}
	else
	{
		m_pBackground->SetAtlasUV(D3DXVECTOR2(GUI_BUTTON_SIZE.x * 2, GUI_BUTTON_SIZE.y * 2), GUI_BUTTON_SIZE);
	}

	m_pBackground->Draw();

	// Draw text
	m_pFont->Draw();

	JWControl::EndDrawing();
}

void JWRadioBox::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_PositionClient);
}

void JWRadioBox::SetCheckState(bool Value)
{
	m_bChecked = Value;
}

auto JWRadioBox::GetCheckState() const->bool
{
	return m_bChecked;
}