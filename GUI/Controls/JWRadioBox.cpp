#include "JWRadioBox.h"
#include "../../CoreBase/JWImage.h"

using namespace JWENGINE;

void JWRadioBox::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type
	m_ControlType = EControlType::RadioBox;

	// A RadioBox never has a border.
	m_bShouldDrawBorder = false;

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	m_pBackground = MAKE_UNIQUE(JWImage)();
	m_pBackground->Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_pBackground->SetTexture(m_pSharedData->Texture_GUI, &m_pSharedData->Texture_GUI_Info);

	// Set control's position and size.
	// @warning: 'D3DXVECTOR2 Size' is not used, but 'GUI_BUTTON_SIZE'.
	SetPosition(Position);
	SetSize(GUI_BUTTON_SIZE);
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