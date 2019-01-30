#include "JWScrollBar.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"
#include "../GUI/JWTextButton.h"
#include "../GUI/JWImageButton.h"

using namespace JWENGINE;

JWScrollBar::JWScrollBar()
{
	m_pBackground = nullptr;
	m_pButtonA = nullptr;
	m_pButtonB = nullptr;
	m_pScroller = nullptr;

	m_ScrollerSize = D3DXVECTOR2(20, 20);
	m_ScrollerPosition = D3DXVECTOR2(0, 0);

	m_bScrollerCaptured = false;

	m_ScrollMax = 0;
	m_ScrollPosition = 0;
}

auto JWScrollBar::Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(pWindow, BaseDir, Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pWindow, ms_BaseDir)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetXRGB(DEFAULT_COLOR_ALMOST_WHITE);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	if (m_pButtonA = new JWImageButton)
	{
		if (JW_FAILED(m_pButtonA->Create(pWindow, BaseDir, Position, GUI_BUTTON_SIZE)))
			return EError::IMAGEBUTTON_NOT_CREATED;

		m_pButtonA->ShouldDrawBorder(false);
	}
	else
	{
		return EError::IMAGEBUTTON_NOT_CREATED;
	}

	if (m_pButtonB = new JWImageButton)
	{
		if (JW_FAILED(m_pButtonB->Create(pWindow, BaseDir, Position, GUI_BUTTON_SIZE)))
			return EError::IMAGEBUTTON_NOT_CREATED;

		m_pButtonB->ShouldDrawBorder(false);
	}
	else
	{
		return EError::IMAGEBUTTON_NOT_CREATED;
	}

	if (m_pScroller = new JWTextButton)
	{
		if (JW_FAILED(m_pScroller->Create(pWindow, BaseDir, Position, GUI_BUTTON_SIZE)))
			return EError::IMAGEBUTTON_NOT_CREATED;

		m_pScroller->ShouldDrawBorder(false);
	}
	else
	{
		return EError::IMAGEBUTTON_NOT_CREATED;
	}

	//SetSize(Size);
	//SetPosition(m_PositionClient);

	// Set default font alignment
	m_pFont->SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_Type = EControlType::ScrollBar;

	return EError::OK;
}

void JWScrollBar::Destroy()
{
	JWControl::Destroy();

	JW_DESTROY(m_pBackground);
}

void JWScrollBar::MakeScrollBar(EScrollBarDirection Direction)
{
	m_ScrollBarDirection = Direction;

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		m_pButtonA->MakeSystemArrowButton(ESystemArrowDirection::Left);
		m_pButtonB->MakeSystemArrowButton(ESystemArrowDirection::Right);
		m_ScrollerPosition.x = GUI_BUTTON_SIZE.x;
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		m_pButtonA->MakeSystemArrowButton(ESystemArrowDirection::Up);
		m_pButtonB->MakeSystemArrowButton(ESystemArrowDirection::Down);
		m_ScrollerPosition.y = GUI_BUTTON_SIZE.y;
		break;
	default:
		break;
	}

	SetSize(m_Size);
	SetPosition(m_PositionClient);
	SetButtonSizeAndPosition();
}

void JWScrollBar::UpdateState(const SMouseData& MouseData)
{
	JWControl::UpdateState(MouseData);

	m_pButtonA->UpdateState(MouseData);
	m_pButtonB->UpdateState(MouseData);

	if (m_pButtonA->GetState() == EControlState::Clicked)
	{
		if (m_ScrollPosition)
			SetScrollPosition(m_ScrollPosition - 1);
	}

	if (m_pButtonB->GetState() == EControlState::Clicked)
	{
		SetScrollPosition(m_ScrollPosition + 1);
	}

	if (m_pScroller->GetState() == EControlState::Pressed)
	{
		if (m_bScrollerCaptured == false)
		{
			m_CapturedScrollerPosition = m_ScrollerPosition;
			m_bScrollerCaptured = true;
		}

		POINT ClientMouseDownPosition = m_pScroller->GetClientMouseDownPosition();

		D3DXVECTOR2 move_stride = D3DXVECTOR2(0, 0);
		move_stride.x = static_cast<float>(MouseData.MousePosition.x - MouseData.MouseDownPosition.x);
		move_stride.y = static_cast<float>(MouseData.MousePosition.y - MouseData.MouseDownPosition.y);

		MoveScroller(move_stride);

		if (MouseData.bMouseLeftButtonPressed)
		{
			m_pScroller->SetState(EControlState::Pressed);
		}
		else
		{
			m_pScroller->UpdateState(MouseData);

			m_CapturedScrollerPosition.x = 0;
			m_CapturedScrollerPosition.y = 0;

			// Calculate Position in Cell

			float position_float = 0;
			int position_int = 0;
			switch (m_ScrollBarDirection)
			{
			case JWENGINE::EScrollBarDirection::Horizontal:
				position_float = (m_ScrollerPosition.x - GUI_BUTTON_SIZE.x) / (m_Size.x - GUI_BUTTON_SIZE.x * 2 - m_ScrollerSize.x);
				position_float *= m_ScrollMax;
				position_int = static_cast<int>(position_float + 0.5f);
				SetScrollPosition(position_int);
				break;
			case JWENGINE::EScrollBarDirection::Vertical:
				position_float = (m_ScrollerPosition.y - GUI_BUTTON_SIZE.x) / (m_Size.y - GUI_BUTTON_SIZE.y * 2 - m_ScrollerSize.y);
				position_float *= m_ScrollMax;
				position_int = static_cast<int>(position_float + 0.5f);
				SetScrollPosition(position_int);
				break;
			default:
				break;
			}
			
		}
	}
	else
	{
		m_bScrollerCaptured = false;
		m_pScroller->UpdateState(MouseData);
	}
}

void JWScrollBar::Draw()
{
	switch (m_State)
	{
	case JWENGINE::Normal:
		break;
	case JWENGINE::Hover:
		break;
	case JWENGINE::Pressed:
		break;
	case JWENGINE::Clicked:
		break;
	default:
		break;
	}

	JWControl::Draw();

	m_pBackground->Draw();

	m_pScroller->Draw();

	m_pButtonA->Draw();
	m_pButtonB->Draw();

	m_pFont->Draw();
}

void JWScrollBar::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);
	m_pBackground->SetPosition(Position);

	SetButtonSizeAndPosition();
}

void JWScrollBar::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);
	m_pBackground->SetSize(Size);

	SetButtonSizeAndPosition();
}

void JWScrollBar::SetState(EControlState State)
{
	JWControl::SetState(State);

	if (State == EControlState::Normal)
	{
		// When mouse is off scrollbar, Buttons must be set to normal state as well
		m_pButtonA->SetState(State);
		m_pButtonB->SetState(State);

		if (m_UpdatedMousedata.bMouseLeftButtonPressed == false)
		{
			m_pScroller->SetState(State);
		}
	}
}

void JWScrollBar::SetScrollRange(size_t Max)
{
	m_ScrollMax = Max;

	SetButtonSizeAndPosition();
}

void JWScrollBar::SetScrollPosition(size_t Position)
{
	if (Position >= m_ScrollMax)
		Position = m_ScrollMax;

	m_ScrollPosition = Position;

	float PosRatio = static_cast<float>(m_ScrollPosition) / static_cast<float>(m_ScrollMax);

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		MoveScroller(D3DXVECTOR2((m_Size.x - GUI_BUTTON_SIZE.x * 2 - (m_ScrollerSize.x / 2)) * PosRatio, 0));
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		MoveScroller(D3DXVECTOR2(0, (m_Size.y - GUI_BUTTON_SIZE.y * 2 - (m_ScrollerSize.y / 2)) * PosRatio));
		break;
	default:
		break;
	}
	
}

PRIVATE void JWScrollBar::SetButtonSizeAndPosition()
{
	D3DXVECTOR2 NewSize = GUI_BUTTON_SIZE;
	D3DXVECTOR2 APosition = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 BPosition = D3DXVECTOR2(0, 0);

	D3DXVECTOR2 NewSizeDebug = m_ScrollerSize;

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		NewSize.y = m_Size.y;

		BPosition = m_PositionClient;
		BPosition.x += m_Size.x - GUI_BUTTON_SIZE.x;

		m_ScrollerSize.x = (m_Size.x - GUI_BUTTON_SIZE.x * 2) / static_cast<float>(m_ScrollMax + 1);
		
		m_ScrollerSize.y = m_Size.y - static_cast<float>(DEFAULT_SCROLLER_PADDING * 2);
		m_ScrollerPosition.y = static_cast<float>(DEFAULT_SCROLLER_PADDING);
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		NewSize.x = m_Size.x;

		BPosition = m_PositionClient;
		BPosition.y += m_Size.y - GUI_BUTTON_SIZE.y;

		m_ScrollerSize.y = (m_Size.y - GUI_BUTTON_SIZE.y * 2) / static_cast<float>(m_ScrollMax + 1);

		m_ScrollerSize.x = m_Size.x - static_cast<float>(DEFAULT_SCROLLER_PADDING * 2);
		m_ScrollerPosition.x = static_cast<float>(DEFAULT_SCROLLER_PADDING);
		break;
	default:
		break;
	}

	m_pButtonA->SetPosition(m_PositionClient);
	m_pButtonA->SetSize(NewSize);

	m_pButtonB->SetPosition(BPosition);
	m_pButtonB->SetSize(NewSize);

	m_pScroller->SetPosition(m_PositionClient + m_ScrollerPosition);
	m_pScroller->SetSize(m_ScrollerSize);
}

PRIVATE void JWScrollBar::MoveScroller(D3DXVECTOR2 Move)
{
	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		m_ScrollerPosition.x = m_CapturedScrollerPosition.x + Move.x;

		m_ScrollerPosition.x = max(m_ScrollerPosition.x, GUI_BUTTON_SIZE.x);
		m_ScrollerPosition.x = min(m_ScrollerPosition.x, m_Size.x - GUI_BUTTON_SIZE.x - m_ScrollerSize.x);
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		m_ScrollerPosition.y = m_CapturedScrollerPosition.y + Move.y;

		m_ScrollerPosition.y = max(m_ScrollerPosition.y, GUI_BUTTON_SIZE.y);
		m_ScrollerPosition.y = min(m_ScrollerPosition.y, m_Size.y - GUI_BUTTON_SIZE.y - m_ScrollerSize.y);
		break;
	default:
		break;
	}

	m_pScroller->SetPosition(m_PositionClient + m_ScrollerPosition);
}