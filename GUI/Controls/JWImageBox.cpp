#include "JWImageBox.h"
#include "../../CoreBase/JWImage.h"

using namespace JWENGINE;

void JWImageBox::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type
	m_ControlType = EControlType::ImageBox;

	// Set default color for every control state.
	m_Color_Normal = DEFAULT_COLOR_NORMAL;
	m_Color_Hover = DEFAULT_COLOR_NORMAL;
	m_Color_Pressed = DEFAULT_COLOR_NORMAL;

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	m_Background.Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_Image.Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);
}

void JWImageBox::Draw() noexcept
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

	// Draw background
	m_Background.Draw();

	// Draw image
	m_Image.Draw();

	JWControl::EndDrawing();
}

auto JWImageBox::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl&
{
	JWControl::SetPosition(Position);

	m_Background.SetPosition(m_Position);
	m_Image.SetPosition(m_Position);

	return *this;
}

auto JWImageBox::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl&
{
	JWControl::SetSize(Size);

	m_Background.SetSize(m_Size);

	//TODO: size decision ways need to be added (Straight / Stretched / Ratio-width / Ratio-height)
	//m_Image.SetSize(m_Size);

	return *this;
}

auto JWImageBox::SetTextureAtlas(const LPDIRECT3DTEXTURE9 pTextureAtlas, const D3DXIMAGE_INFO* pTextureAtlasInfo) noexcept->JWControl&
{
	m_Image.SetTexture(pTextureAtlas, pTextureAtlasInfo);

	return *this;
}

auto JWImageBox::SetAtlasUV(const D3DXVECTOR2& OffsetInAtlas, const D3DXVECTOR2& Size) noexcept->JWControl&
{
	m_Image.SetAtlasUV(OffsetInAtlas, Size);

	return *this;
}

auto JWImageBox::SetBackgroundColor(DWORD Color) noexcept->JWControl&
{
	JWControl::SetBackgroundColor(Color);

	m_Background.SetColor(Color);

	return *this;
}