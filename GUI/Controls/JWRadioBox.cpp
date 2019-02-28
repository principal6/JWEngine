#include "JWRadioBox.h"
#include "../../CoreBase/JWImage.h"

using namespace JWENGINE;

JWRadioBox::JWRadioBox()
{
	// RadioBox never has border
	m_bShouldDrawBorder = false;

	m_pBackground = nullptr;

	m_bChecked = false;
}

auto JWRadioBox::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->JWControl*
{
	JWControl::Create(Position, Size, pSharedData);

	if (m_pBackground = new JWImage)
	{
		m_pBackground->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir);
		m_pBackground->SetTexture(m_pSharedData->Texture_GUI, &m_pSharedData->Texture_GUI_Info);
	}
	else
	{
		throw EError::IMAGE_NOT_CREATED;
	}

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_ControlType = EControlType::RadioBox;

	// Set control's position and size.
	// @ WARNING:
	// 'D3DXVECTOR2 Size' is not used but 'GUI_BUTTON_SIZE'
	SetPosition(Position);
	SetSize(GUI_BUTTON_SIZE);
	
	return this;
}

void JWRadioBox::Destroy() noexcept
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
}

void JWRadioBox::Draw() noexcept
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

auto JWRadioBox::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_Position);

	return this;
}

auto JWRadioBox::SetCheckState(bool Value) noexcept->JWControl*
{
	m_bChecked = Value;

	return this;
}

auto JWRadioBox::GetCheckState() const noexcept->bool
{
	return m_bChecked;
}