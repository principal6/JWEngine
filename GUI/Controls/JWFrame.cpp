#include "JWFrame.h"
#include "../../CoreBase/JWImage.h"
#include "../../CoreBase/JWWindow.h"

using namespace JWENGINE;

void JWFrame::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type.
	m_ControlType = EControlType::Frame;

	// Create a JWImageBox for background.
	m_Background.Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_Background.SetPosition(m_Position);
	m_Background.SetSize(m_Size);

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);
}

auto JWFrame::AddChildControl(JWControl& ChildControl) noexcept->JWControl*
{
	if (m_pChildControls.size())
	{
		for (auto iterator : m_pChildControls)
		{
			if (iterator == &ChildControl)
			{
				// This control is already adopted!!
				return this;
			}
		}
	}

	if (!ChildControl.HasParentControl())
	{
		// If the control already has parent, we must not adopt it!

		ChildControl.ShouldBeOffsetByMenuBar(false);
		ChildControl.ShouldBeOffsetByParent(true);
		ChildControl.SetParentControl(this);

		ChildControl.SetPosition(ChildControl.GetAbsolutePosition());

		// This is required to clip child control's viewport IAW/ the parent(JWFrame) control's viewport.
		ChildControl.UpdateViewport();

		m_pChildControls.push_back(&ChildControl);
	}

	return this;
}

PROTECTED auto JWFrame::SetFrameConnector(JWControl& Connector, bool IsBasisFrame) noexcept->JWControl*
{
	m_pFrameConnector = &Connector;

	m_IsBasisFrame = IsBasisFrame;

	return this;
}

PROTECTED void JWFrame::UpdateViewport() noexcept
{
	JWControl::UpdateViewport();

	if (m_pChildControls.size())
	{
		for (size_t iterator_index = m_pChildControls.size(); iterator_index > 0; iterator_index--)
		{
			m_pChildControls[iterator_index - 1]->UpdateViewport();
		}
	}
}

PROTECTED void JWFrame::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept
{
	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	bool b_mouse_in_rect = Static_IsMouseInViewPort(p_input_state->MousePosition, m_ControlViewport);

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

void JWFrame::Draw() noexcept
{
	JWControl::BeginDrawing();

	switch (m_ControlState)
	{
	case JWENGINE::Normal:
		m_Background.SetColor(m_Color_Normal);
		break;
	case JWENGINE::Hover:
		// [DEBUG]
		//m_Background.SetColor(m_Color_Hover);
		break;
	case JWENGINE::Pressed:
		// [DEBUG]
		//m_Background.SetColor(m_Color_Pressed);
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	m_Background.Draw();

	if (m_pChildControls.size())
	{
		for (auto iterator : m_pChildControls)
		{
			iterator->Draw();
		}
	}

	JWControl::EndDrawing();
}

auto JWFrame::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	if (m_pFrameConnector)
	{
		if (m_IsBasisFrame)
		{
			SetPositionFromInside(Position);

			m_pFrameConnector->UpdateFrameConectorPositionAndSize();
		}
	}
	else
	{
		SetPositionFromInside(Position);
	}

	return this;
}

PROTECTED void JWFrame::SetPositionFromInside(const D3DXVECTOR2& Position) noexcept
{
	JWControl::SetPosition(Position);

	if (m_pChildControls.size())
	{
		for (auto iterator : m_pChildControls)
		{
			iterator->SetPosition(iterator->GetAbsolutePosition());
		}
	}

	m_Background.SetPosition(m_Position);
}

auto JWFrame::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl*
{
	if (m_pFrameConnector)
	{
		if (m_IsBasisFrame)
		{
			SetSizeFromInside(Size);
		}
		else
		{
			D3DXVECTOR2 new_size = m_pFrameConnector->GetSize();

			switch (m_pFrameConnector->GetFrameConnectorType())
			{
			case EFrameConnectorType::Horizontal:
				new_size.y = Size.y;
				break;
			case EFrameConnectorType::Vertical:
				new_size.x = Size.x;
				break;
			default:
				break;
			}

			SetSizeFromInside(new_size);
		}

		m_pFrameConnector->UpdateFrameConectorPositionAndSize();
	}
	else
	{
		SetSizeFromInside(Size);
	}

	return this;
}

PROTECTED void JWFrame::SetSizeFromInside(const D3DXVECTOR2& Size) noexcept
{
	JWControl::SetSize(Size);

	if (m_pChildControls.size())
	{
		for (auto iterator : m_pChildControls)
		{
			iterator->UpdateViewport();
		}
	}

	m_Background.SetSize(m_Size);
}