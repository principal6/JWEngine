#include "JWLabel.h"
#include "../../CoreBase/JWImage.h"

using namespace JWENGINE;

JWLabel::JWLabel()
{
	// Set default color for every control state.
	m_Color_Normal = DEFAULT_COLOR_BACKGROUND_LABEL;
	m_Color_Hover = DEFAULT_COLOR_BACKGROUND_LABEL;
	m_Color_Pressed = DEFAULT_COLOR_BACKGROUND_LABEL;
}

auto JWLabel::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
		return EError::CONTROL_NOT_CREATED;

	// Create image for background
	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetPosition(Position);
		m_pBackground->SetSize(Size);
		m_pBackground->SetAlpha(DEFUALT_ALPHA_BACKGROUND_LABEL);
		m_pBackground->SetXRGB(DEFAULT_COLOR_BACKGROUND_LABEL);
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Set control type
	m_ControlType = EControlType::Label;

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);

	return EError::OK;
}

void JWLabel::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
}

void JWLabel::Draw()
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_pBackground->SetColor(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		m_pBackground->SetColor(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		m_pBackground->SetColor(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	m_pBackground->Draw();

	JWControl::EndDrawing();
}

auto JWLabel::SetPosition(const D3DXVECTOR2& Position)->JWControl*
{
	JWControl::SetPosition(Position);

	if (m_pBackground)
	{
		m_pBackground->SetPosition(m_Position);
	}

	return this;
}

auto JWLabel::SetSize(const D3DXVECTOR2& Size)->JWControl*
{
	JWControl::SetSize(Size);

	if (m_pBackground)
	{
		m_pBackground->SetSize(m_Size);
	}

	return this;
}