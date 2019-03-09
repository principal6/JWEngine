#include "JWFrameConnector.h"
#include "../../CoreBase/JWImage.h"
#include "../../CoreBase/JWWindow.h"

using namespace JWENGINE;

void JWFrameConnector::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type.
	m_ControlType = EControlType::FrameConnector;

	m_Color_Normal = DEFAULT_COLOR_LESS_BLACK;
	m_Color_Hover = m_Color_Normal;
	m_Color_Pressed = DEFAULT_COLOR_ALMOST_BLACK;

	// Create a JWImageBox for background.
	m_Background.Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_Background.SetPosition(m_Position);
	m_Background.SetSize(m_Size);

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);
}

auto JWFrameConnector::MakeVerticalConnector(JWControl& LeftFrame, JWControl& RightFrame)->JWControl*
{
	if (&LeftFrame == &RightFrame)
	{
		// You must connect two different frames!
		LOG_DEBUG(L"[FAILED] JWFrameConnector->MakeVerticalConnector() - You must connect two different frames.");

		return this;
	}

	m_pFrameA = &LeftFrame;
	m_pFrameB = &RightFrame;

	m_pFrameA->SetFrameConnector(*this, true);
	m_pFrameB->SetFrameConnector(*this, false);

	m_FrameConnectorType = EFrameConnectorType::Vertical;

	UpdateFrameConectorPositionAndSize();

	return this;
}

auto JWFrameConnector::MakeHorizontalConnector(JWControl& UpFrame, JWControl& DownFrame)->JWControl*
{
	m_pFrameA = &UpFrame;
	m_pFrameB = &DownFrame;

	m_pFrameA->SetFrameConnector(*this, true);
	m_pFrameB->SetFrameConnector(*this, false);

	m_FrameConnectorType = EFrameConnectorType::Horizontal;

	UpdateFrameConectorPositionAndSize();

	return this;
}

PROTECTED void JWFrameConnector::SetSizableCursor() noexcept
{
	switch (m_FrameConnectorType)
	{
	case JWENGINE::EFrameConnectorType::Invalid:
		break;
	case JWENGINE::EFrameConnectorType::Horizontal:
		SetCursor(LoadCursor(nullptr, IDC_SIZENS));
		break;
	case JWENGINE::EFrameConnectorType::Vertical:
		SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
		break;
	default:
		break;
	}
}

PROTECTED void JWFrameConnector::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept
{
	JWControl::UpdateControlState(ppControlWithMouse, ppControlWithFocus);

	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	bool b_mouse_in_rect = Static_IsMouseInViewPort(p_input_state->MousePosition, m_ControlViewport);
	bool b_mouse_down_in_rect = Static_IsMouseInViewPort(p_input_state->MouseDownPosition, m_ControlViewport);

	if (b_mouse_in_rect)
	{
		SetSizableCursor();

		if (p_input_state->MouseLeftPressed)
		{
			if (b_mouse_down_in_rect)
			{
				if (!m_IsBeingCaptured)
				{
					m_IsBeingCaptured = true;

					m_CapturedMouseDownPosition = p_input_state->MouseDownPosition;
					m_CapturedPosition = m_Position;
				}
			}
		}
	}

	if (p_input_state->MouseLeftReleased)
	{
		if (m_IsBeingCaptured)
		{
			m_IsBeingCaptured = false;
		}
	}

	if (m_IsBeingCaptured)
	{
		JWControl::SetControlState(EControlState::Pressed);

		if (ppControlWithMouse)
		{
			*ppControlWithMouse = this;
		}

		SetSizableCursor();

		POINT mouse_diff{};
		mouse_diff.x = p_input_state->MousePosition.x - m_CapturedMouseDownPosition.x;
		mouse_diff.y = p_input_state->MousePosition.y - m_CapturedMouseDownPosition.y;

		D3DXVECTOR2 new_position = m_CapturedPosition;

		D3DXVECTOR2 A_size{};
		D3DXVECTOR2 A_position{};

		switch (m_FrameConnectorType)
		{
		case JWENGINE::EFrameConnectorType::Invalid:
			break;
		case JWENGINE::EFrameConnectorType::Horizontal:
			new_position.y += static_cast<float>(mouse_diff.y);

			JWControl::SetPosition(new_position);

			A_size = m_pFrameA->GetSize();
			A_position = m_pFrameA->GetPosition();

			A_size.y = new_position.y - A_position.y;

			m_pFrameA->SetSize(A_size);

			break;
		case JWENGINE::EFrameConnectorType::Vertical:
			new_position.x += static_cast<float>(mouse_diff.x);

			JWControl::SetPosition(new_position);

			A_size = m_pFrameA->GetSize();
			A_position = m_pFrameA->GetPosition();

			A_size.x = new_position.x - A_position.x;

			m_pFrameA->SetSize(A_size);

			break;
		default:
			break;
		}

		//LOG_DEBUG(L"Position: " + ConvertFloatToWSTRING(m_Position.x) + L" / " + ConvertFloatToWSTRING(m_Position.y));

		UpdateFrameConectorPositionAndSize();
	}
}

void JWFrameConnector::Draw() noexcept
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

PROTECTED void JWFrameConnector::UpdateFrameConectorPositionAndSize() noexcept
{
	D3DXVECTOR2 A_position{};
	D3DXVECTOR2 A_size{};
	D3DXVECTOR2 B_position{};
	D3DXVECTOR2 B_size{};
	D3DXVECTOR2 connector_position{};
	D3DXVECTOR2 connector_size{};

	switch (m_FrameConnectorType)
	{
	case JWENGINE::EFrameConnectorType::Invalid:
		break;
	case JWENGINE::EFrameConnectorType::Horizontal:
		// Get frame A's position and size.
		A_position = m_pFrameA->GetAbsolutePosition();
		A_size = m_pFrameA->GetSize();

		// Set connector's position and size.
		connector_position = A_position;
		connector_position.y += A_size.y;
		JWControl::SetPosition(connector_position);

		connector_size = m_Size;
		connector_size.x = A_size.x;
		JWControl::SetSize(connector_size);

		// Set frame B's position and size.
		B_position = connector_position;
		B_position.y += m_Size.y;
		m_pFrameB->SetFramePositionFromInside(B_position);

		B_size = m_pFrameB->GetSize();
		B_size.x = A_size.x;
		m_pFrameB->SetFrameSizeFromInside(B_size);

		break;
	case JWENGINE::EFrameConnectorType::Vertical:
		// Get frame A's position and size.
		A_position = m_pFrameA->GetAbsolutePosition();
		A_size = m_pFrameA->GetSize();

		// Set connector's position and size.
		connector_position = A_position;
		connector_position.x += A_size.x;
		JWControl::SetPosition(connector_position);

		connector_size = m_Size;
		connector_size.y = A_size.y;
		JWControl::SetSize(connector_size);

		// Set frame B's position and size.
		B_position = connector_position;
		B_position.x += m_Size.x;
		m_pFrameB->SetFramePositionFromInside(B_position);

		B_size = m_pFrameB->GetSize();
		B_size.y = A_size.y;
		m_pFrameB->SetFrameSizeFromInside(B_size);

		break;
	default:
		break;
	}

	m_Background.SetPosition(m_Position);
	m_Background.SetSize(m_Size);
}

auto JWFrameConnector::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	UpdateFrameConectorPositionAndSize();

	return this;
}

auto JWFrameConnector::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl*
{
	D3DXVECTOR2 new_size = Size;

	switch (m_FrameConnectorType)
	{
	case JWENGINE::EFrameConnectorType::Invalid:
		break;
	case JWENGINE::EFrameConnectorType::Horizontal:
		new_size.x = m_pFrameA->GetSize().x;
		break;
	case JWENGINE::EFrameConnectorType::Vertical:
		new_size.y = m_pFrameA->GetSize().y;
		break;
	default:
		break;
	}

	m_Size = new_size;

	UpdateFrameConectorPositionAndSize();

	return this;
}

auto JWFrameConnector::GetFrameConnectorType() const noexcept->EFrameConnectorType
{
	return m_FrameConnectorType;
}