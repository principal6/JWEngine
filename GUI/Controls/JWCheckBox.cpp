#include "JWCheckBox.h"
#include "../../CoreBase/JWImage.h"
#include "../JWGUIWindow.h"

using namespace JWENGINE;

JWCheckBox::JWCheckBox()
{
	// A checkbox has always its border.
	m_bShouldDrawBorder = true;
}

auto JWCheckBox::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData)->JWControl*
{
	JWControl::Create(Position, Size, SharedData);

	m_pBackground = new JWImage;
	m_pBackground->Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_pBackground->SetColor(DEFAULT_COLOR_ALMOST_WHITE);
	m_pBackground->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);

	m_pCheckImage = new JWImage;
	m_pCheckImage->Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_pCheckImage->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	m_pCheckImage->SetTexture(m_pSharedData->Texture_GUI, &m_pSharedData->Texture_GUI_Info);
	m_pCheckImage->SetAtlasUV(D3DXVECTOR2(0, GUI_BUTTON_SIZE.y * 2), GUI_BUTTON_SIZE);

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_ControlType = EControlType::CheckBox;

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);

	return this;
}

void JWCheckBox::Destroy() noexcept
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
	JW_DESTROY(m_pCheckImage);
}

void JWCheckBox::Draw() noexcept
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
		m_bChecked = !m_bChecked;
		break;
	default:
		break;
	}

	// Draw background image.
	m_pBackground->Draw();

	// Draw check image if it's checked.
	if (m_bChecked)
	{
		m_pCheckImage->Draw();
	}

	JWControl::EndDrawing();
}

auto JWCheckBox::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	JWControl::SetPosition(Position);
	
	m_pBackground->SetPosition(m_Position);
	m_pCheckImage->SetPosition(m_Position + m_ButtonImageOffset);

	return this;
}

auto JWCheckBox::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl*
{
	D3DXVECTOR2 adjusted_size = Size;

	adjusted_size.x = max(adjusted_size.x, GUI_BUTTON_SIZE.x);
	adjusted_size.y = max(adjusted_size.y, GUI_BUTTON_SIZE.y);

	m_ButtonImageOffset.x = (adjusted_size.x - GUI_BUTTON_SIZE.x) / 2.0f;
	m_ButtonImageOffset.y = (adjusted_size.y - GUI_BUTTON_SIZE.y) / 2.0f;

	JWControl::SetSize(adjusted_size);

	m_pBackground->SetSize(m_Size);
	m_pCheckImage->SetPosition(m_Position + m_ButtonImageOffset);

	return this;
}

auto JWCheckBox::SetCheckState(bool Value) noexcept->JWControl*
{
	m_bChecked = Value;

	return this;
}

auto JWCheckBox::GetCheckState() const noexcept->bool
{
	return m_bChecked;
}