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

auto JWRadioBox::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUISharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;
		
		//m_pBackground->SetColor(D3DCOLOR_XRGB(255, 255, 255));
		m_pBackground->SetTexture(m_pSharedData->Texture_GUI, &m_pSharedData->Texture_GUI_Info);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

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

	JWControl::EndDrawing();
}

void JWRadioBox::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_Position);
}

void JWRadioBox::SetCheckState(bool Value)
{
	m_bChecked = Value;
}

auto JWRadioBox::GetCheckState() const->bool
{
	return m_bChecked;
}