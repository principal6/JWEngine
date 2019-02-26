#include "JWImageBox.h"
#include "../../CoreBase/JWImage.h"

using namespace JWENGINE;

JWImageBox::JWImageBox()
{
	// An imagebox normally doesn't have border.
	m_bShouldDrawBorder = false;

	m_pImage = nullptr;

	m_OffsetInAtlas = D3DXVECTOR2(0, 0);

	// Set default color for every control state.
	m_Color_Normal = DEFAULT_COLOR_NORMAL;
	m_Color_Hover = DEFAULT_COLOR_NORMAL;
	m_Color_Pressed = DEFAULT_COLOR_NORMAL;
}

auto JWImageBox::Create(const D3DXVECTOR2 Position, const D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError
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
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	if (m_pImage = new JWImage)
	{
		if (JW_FAILED(m_pImage->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
		{
			return EError::IMAGE_NOT_CREATED;
		}
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_ControlType = EControlType::ImageBox;

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

	return EError::OK;
}

void JWImageBox::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
	JW_DESTROY(m_pImage);
}

void JWImageBox::Draw()
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

auto JWImageBox::SetPosition(const D3DXVECTOR2 Position)->JWControl*
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_Position);
	m_pImage->SetPosition(m_Position);

	return this;
}

auto JWImageBox::SetSize(const D3DXVECTOR2 Size)->JWControl*
{
	JWControl::SetSize(Size);

	m_pBackground->SetSize(m_Size);

	//TODO: size decision ways need to be added (Straight / Stretched / Ratio-width / Ratio-height)
	//m_pImage->SetSize(m_Size);

	return this;
}

auto JWImageBox::SetTextureAtlas(const LPDIRECT3DTEXTURE9 pTextureAtlas, const D3DXIMAGE_INFO* pTextureAtlasInfo)->JWControl*
{
	m_pImage->SetTexture(pTextureAtlas, pTextureAtlasInfo);

	return this;
}

auto JWImageBox::SetAtlasUV(const D3DXVECTOR2 OffsetInAtlas, const D3DXVECTOR2 Size)->JWControl*
{
	m_pImage->SetAtlasUV(OffsetInAtlas, Size);

	return this;
}

auto JWImageBox::SetBackgroundColor(DWORD Color)->JWControl*
{
	JWControl::SetBackgroundColor(Color);

	m_pBackground->SetColor(Color);

	return this;
}