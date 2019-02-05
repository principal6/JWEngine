#include "JWLabel.h"
#include "../CoreBase/JWFont.h"

using namespace JWENGINE;

auto JWLabel::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	// Set control type
	m_Type = EControlType::Label;

	// Set default background color
	m_pFont->SetBoxAlpha(DEFUALT_ALPHA_BACKGROUND);
	m_pFont->SetBoxXRGB(DEFAULT_COLOR_BACKGROUND);

	return EError::OK;
}

void JWLabel::Draw()
{
	JWControl::Draw();
	
	// Draw text
	m_pFont->Draw();
}

void JWLabel::SetBackgroundAlpha(BYTE Alpha)
{
	m_pFont->SetBoxAlpha(Alpha);
}

void JWLabel::SetBackgroundXRGB(DWORD XRGB)
{
	m_pFont->SetBoxXRGB(XRGB);
}