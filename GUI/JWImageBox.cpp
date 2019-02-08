#include "JWImageBox.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"

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

auto JWImageBox::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;
	
	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	if (m_pImage = new JWImage)
	{
		if (JW_FAILED(m_pImage->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Set default alignment
	SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

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

void JWImageBox::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_PositionClient);
	m_pImage->SetPosition(m_PositionClient);
}

void JWImageBox::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);

	m_pBackground->SetSize(m_Size);
	//m_pImage->SetSize(m_Size);
}

void JWImageBox::SetTextureAtlas(LPDIRECT3DTEXTURE9 pTextureAtlas, D3DXIMAGE_INFO* pTextureAtlasInfo)
{
	m_pImage->SetTexture(pTextureAtlas, pTextureAtlasInfo);
}

void JWImageBox::SetAtlasUV(D3DXVECTOR2 OffsetInAtlas, D3DXVECTOR2 Size)
{
	m_pImage->SetAtlasUV(OffsetInAtlas, Size);
}

void JWImageBox::SetBackgroundColor(DWORD Color)
{
	JWControl::SetBackgroundColor(Color);

	m_pBackground->SetColor(Color);
}