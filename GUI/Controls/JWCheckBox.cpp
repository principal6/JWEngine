#include "JWCheckBox.h"
#include "../../CoreBase/JWImage.h"
#include "../JWGUIWindow.h"

using namespace JWENGINE;

void JWCheckBox::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type
	m_ControlType = EControlType::CheckBox;

	// A checkbox has always its border.
	m_bShouldDrawBorder = true;

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	m_Background.Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_Background.SetColor(DEFAULT_COLOR_ALMOST_WHITE);
	m_Background.SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);

	m_CheckImage.Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_CheckImage.SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	m_CheckImage.SetTexture(m_pSharedData->Texture_GUI, &m_pSharedData->Texture_GUI_Info);
	m_CheckImage.SetAtlasUV(D3DXVECTOR2(0, GUI_BUTTON_SIZE.y * 2), GUI_BUTTON_SIZE);

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);
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
	m_Background.Draw();

	// Draw check image if it's checked.
	if (m_bChecked)
	{
		m_CheckImage.Draw();
	}

	JWControl::EndDrawing();
}

auto JWCheckBox::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	JWControl::SetPosition(Position);
	
	m_Background.SetPosition(m_Position);
	m_CheckImage.SetPosition(m_Position + m_ButtonImageOffset);

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

	m_Background.SetSize(m_Size);
	m_CheckImage.SetPosition(m_Position + m_ButtonImageOffset);

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