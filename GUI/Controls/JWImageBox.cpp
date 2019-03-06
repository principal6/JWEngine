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

	m_pBackground = MAKE_UNIQUE(JWImage)();
	m_pBackground->Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);

	m_pImage = MAKE_UNIQUE(JWImage)();
	m_pImage->Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);

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

	// Draw background
	m_pBackground->Draw();

	// Draw image
	m_pImage->Draw();

	JWControl::EndDrawing();
}

auto JWImageBox::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_Position);
	m_pImage->SetPosition(m_Position);

	return this;
}

auto JWImageBox::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl*
{
	JWControl::SetSize(Size);

	m_pBackground->SetSize(m_Size);

	//TODO: size decision ways need to be added (Straight / Stretched / Ratio-width / Ratio-height)
	//m_pImage->SetSize(m_Size);

	return this;
}

auto JWImageBox::SetTextureAtlas(const LPDIRECT3DTEXTURE9 pTextureAtlas, const D3DXIMAGE_INFO* pTextureAtlasInfo) noexcept->JWControl*
{
	m_pImage->SetTexture(pTextureAtlas, pTextureAtlasInfo);

	return this;
}

auto JWImageBox::SetAtlasUV(const D3DXVECTOR2& OffsetInAtlas, const D3DXVECTOR2& Size) noexcept->JWControl*
{
	m_pImage->SetAtlasUV(OffsetInAtlas, Size);

	return this;
}

auto JWImageBox::SetBackgroundColor(DWORD Color) noexcept->JWControl*
{
	JWControl::SetBackgroundColor(Color);

	m_pBackground->SetColor(Color);

	return this;
}