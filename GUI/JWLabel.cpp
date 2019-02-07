#include "JWLabel.h"
#include "../CoreBase/JWFont.h"

using namespace JWENGINE;

JWLabel::JWLabel()
{
	// Set default color for every control state.
	m_Color_Normal = DEFAULT_COLOR_BACKGROUND_LABEL;
	m_Color_Hover = DEFAULT_COLOR_BACKGROUND_LABEL;
	m_Color_Pressed = DEFAULT_COLOR_BACKGROUND_LABEL;
}

auto JWLabel::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	// Set control type
	m_Type = EControlType::Label;

	// Set default background color
	m_pFont->SetBoxColor(GetMixedColor(DEFUALT_ALPHA_BACKGROUND_LABEL, DEFAULT_COLOR_BACKGROUND_LABEL));

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

	return EError::OK;
}

void JWLabel::Draw()
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_pFont->SetBoxColor(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		m_pFont->SetBoxColor(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		m_pFont->SetBoxColor(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	// Draw text
	m_pFont->Draw();

	JWControl::EndDrawing();
}

void JWLabel::SetBackgroundColor(DWORD Color)
{
	m_pFont->SetBoxColor(Color);
}

void JWLabel::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	m_pFont->SetText(m_Text, m_PositionClient, m_Size);
}

void JWLabel::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);

	m_pFont->SetText(m_Text, m_PositionClient, m_Size);
}

void JWLabel::SetUseMultiline(bool Value)
{
	m_pFont->SetUseMultiline(Value);
	UpdateText();
}