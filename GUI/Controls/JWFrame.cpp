#include "JWFrame.h"
#include "JWImageBox.h"
#include "../../CoreBase/JWWindow.h"

using namespace JWENGINE;

JWFrame::JWFrame()
{
	m_pBackground = nullptr;
}

auto JWFrame::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
	{
		return EError::CONTROL_NOT_CREATED;
	}

	// Create a JWImageBox for background.
	if (m_pBackground = new JWImageBox)
	{
		m_pBackground->ShouldBeOffsetByMenuBar(false);

		if (JW_FAILED(m_pBackground->Create(Position, Size, pSharedData)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetPosition(Position);
		m_pBackground->SetSize(Size);
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Set control type.
	m_ControlType = EControlType::Frame;

	// Set control's size and position.
	SetPosition(Position);
	SetSize(Size);

	return EError::OK;
}

void JWFrame::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
}

auto JWFrame::AddChildControl(JWControl* pChildControl)->JWControl*
{
	if (m_pChildControls.size())
	{
		for (auto iterator : m_pChildControls)
		{
			if (iterator == pChildControl)
			{
				// This control is already adopted!!
				return this;
			}
		}
	}

	pChildControl->ShouldBeOffsetByMenuBar(false);
	pChildControl->ShouldUseViewport(false);
	pChildControl->SetParentControl(this);

	m_pChildControls.push_back(pChildControl);

	return this;
}

void JWFrame::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus)
{
	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	bool b_mouse_in_rect = Static_IsMouseInRECT(p_input_state->MousePosition, m_ControlRect);

	if (b_mouse_in_rect)
	{
		if (m_pChildControls.size())
		{
			for (size_t iterator_index = m_pChildControls.size(); iterator_index > 0; iterator_index--)
			{
				m_pChildControls[iterator_index - 1]->UpdateControlState(ppControlWithMouse, ppControlWithFocus);
			}
		}
	}

	JWControl::UpdateControlState(ppControlWithMouse, ppControlWithFocus);
}

void JWFrame::Draw()
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_pBackground->SetBackgroundColor(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		// [DEBUG]
		//m_pBackground->SetBackgroundColor(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		// [DEBUG]
		//m_pBackground->SetBackgroundColor(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	m_pBackground->Draw();

	if (m_pChildControls.size())
	{
		for (auto iterator : m_pChildControls)
		{
			JWControl::BeginDrawing();
			iterator->Draw();
		}
	}

	JWControl::EndDrawing();
}

auto JWFrame::SetPosition(const D3DXVECTOR2& Position)->JWControl*
{
	JWControl::SetPosition(Position);

	if (m_pChildControls.size())
	{
		for (auto iterator : m_pChildControls)
		{
			D3DXVECTOR2 child_offset_position = iterator->GetAbsolutePosition();

			child_offset_position.x += m_Position.x;
			child_offset_position.y += m_Position.y;

			iterator->SetPosition(child_offset_position);
		}
	}

	m_pBackground->SetPosition(m_Position);

	return this;
}

auto JWFrame::SetSize(const D3DXVECTOR2& Size)->JWControl*
{
	JWControl::SetSize(Size);

	m_pBackground->SetSize(m_Size);

	return this;
}