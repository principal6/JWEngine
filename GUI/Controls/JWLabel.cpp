#include "JWLabel.h"
#include "../../CoreBase/JWImage.h"

using namespace JWENGINE;

void JWLabel::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type
	m_ControlType = EControlType::Label;

	// Set default color for every control state.
	m_Color_Normal = DEFAULT_COLOR_BACKGROUND_LABEL;
	m_Color_Hover = DEFAULT_COLOR_BACKGROUND_LABEL;
	m_Color_Pressed = DEFAULT_COLOR_BACKGROUND_LABEL;

	// Create image for background
	m_Background.Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_Background.SetPosition(m_Position);
	m_Background.SetSize(m_Size);
	m_Background.SetAlpha(DEFUALT_ALPHA_BACKGROUND_LABEL);
	m_Background.SetXRGB(DEFAULT_COLOR_BACKGROUND_LABEL);

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);
}

void JWLabel::Draw() noexcept
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_Background.SetColor(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		m_Background.SetColor(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		m_Background.SetColor(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	m_Background.Draw();

	JWControl::EndDrawing();
}

auto JWLabel::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	JWControl::SetPosition(Position);

	m_Background.SetPosition(m_Position);

	return this;
}

auto JWLabel::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl*
{
	JWControl::SetSize(Size);

	m_Background.SetSize(m_Size);

	return this;
}