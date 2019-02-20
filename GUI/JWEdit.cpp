#include "JWEdit.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWText.h"
#include "../CoreBase/JWRectangle.h"
#include "JWImageBox.h"

using namespace JWENGINE;


JWEdit::JWEdit()
{
	m_bShouldDrawBorder = true;

	// Set default color for every control state.
	m_Color_Normal = DEFAULT_COLOR_BACKGROUND_EDIT;
	m_Color_Hover = DEFAULT_COLOR_BACKGROUND_EDIT;
	m_Color_Pressed = DEFAULT_COLOR_BACKGROUND_EDIT;
}

auto JWEdit::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
		return EError::CONTROL_NOT_CREATED;

	// Create a JWImageBox for background.
	if (m_pBackground = new JWImageBox)
	{
		if (JW_FAILED(m_pBackground->Create(Position, Size, pSharedData)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetPosition(Position);
		m_pBackground->SetSize(Size);
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Create non-instant JWText for JWEdit control.
	if (m_pEditText = new JWText)
	{
		if (JW_FAILED(m_pEditText->CreateNonInstantText(m_pSharedData->pWindow, &m_pSharedData->BaseDir, m_pSharedData->pText->GetFontTexturePtr())))
			return EError::TEXT_NOT_CREATED;


	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Set control type.
	m_ControlType = EControlType::Label;

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

	return EError::OK;
}

void JWEdit::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pEditText);
	JW_DESTROY(m_pBackground);
}

void JWEdit::Draw()
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_pBackground->SetBackgroundColor(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		m_pBackground->SetBackgroundColor(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		m_pBackground->SetBackgroundColor(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	m_pBackground->Draw();

	JWControl::EndDrawing();
}