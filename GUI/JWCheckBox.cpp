#include "JWCheckBox.h"
#include "../CoreBase/JWImage.h"

using namespace JWENGINE;

JWCheckBox::JWCheckBox()
{
	// A checkbox has always its border.
	m_bShouldDrawBorder = true;

	m_pBackground = nullptr;
	m_pCheckImage = nullptr;

	m_ButtonImageOffset = D3DXVECTOR2(0, 0);
	m_OffsetInAtlas = D3DXVECTOR2(0, 0);

	m_bChecked = false;
}

auto JWCheckBox::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
	{
		return EError::CONTROL_NOT_CREATED;
	}

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
		{
			return EError::IMAGE_NOT_CREATED;
		}
		m_pBackground->SetColor(DEFAULT_COLOR_ALMOST_WHITE);
		m_pBackground->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	if (m_pCheckImage = new JWImage)
	{
		if (JW_FAILED(m_pCheckImage->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
		{
			return EError::IMAGE_NOT_CREATED;
		}
		m_pCheckImage->SetBoundingBoxXRGB(DEFAULT_COLOR_BORDER);
		m_pCheckImage->SetTexture(m_pSharedData->Texture_GUI, &m_pSharedData->Texture_GUI_Info);
		m_pCheckImage->SetAtlasUV(D3DXVECTOR2(0, GUI_BUTTON_SIZE.y * 2), GUI_BUTTON_SIZE);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_ControlType = EControlType::CheckBox;

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

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

void JWCheckBox::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_Position);
	m_pCheckImage->SetPosition(m_Position + m_ButtonImageOffset);
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

	m_pBackground->SetSize(m_Size);
	m_pCheckImage->SetPosition(m_Position + m_ButtonImageOffset);
}

void JWCheckBox::SetCheckState(bool Value)
{
	m_bChecked = Value;
}

auto JWCheckBox::GetCheckState() const->bool
{
	return m_bChecked;
}