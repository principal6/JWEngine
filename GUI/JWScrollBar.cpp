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
		//m_pBackground->SetXRGB(DEFAULT_COLOR_NORMAL);
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
		if (JW_FAILED(m_pScroller->Create(pWindow, BaseDir, Position, m_ScrollerSize)))
			return EError::IMAGEBUTTON_NOT_CREATED;

		m_pScroller->ShouldDrawBorder(false);
	}
	else
	{
		return EError::IMAGEBUTTON_NOT_CREATED;
	}

	SetSize(Size);
	SetPosition(m_PositionClient);

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

	SetButtonSizeAndPosition();
}

void JWScrollBar::UpdateState(const SMouseData& MouseData)
{
	JWControl::UpdateState(MouseData);

	m_pButtonA->UpdateState(MouseData);
	m_pButtonB->UpdateState(MouseData);

	if (m_pScroller->GetState() == EControlState::Pressed)
	{
		if (m_bScrollerCaptured == false)
		{
			m_CapturedScrollerPosition = m_ScrollerPosition;
			m_bScrollerCaptured = true;

			std::cout << "CAPTURE!" << std::endl;
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

PRIVATE void JWScrollBar::SetButtonSizeAndPosition()
{
	D3DXVECTOR2 NewSize = GUI_BUTTON_SIZE;
	D3DXVECTOR2 APosition = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 BPosition = D3DXVECTOR2(0, 0);

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		NewSize.y = m_Size.y;

		BPosition = m_PositionClient;
		BPosition.x += m_Size.x - GUI_BUTTON_SIZE.x;

		m_ScrollerSize.y = m_Size.y - static_cast<float>(DEFAULT_SCROLLER_PADDING * 2);
		m_ScrollerPosition.y = static_cast<float>(DEFAULT_SCROLLER_PADDING);
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		NewSize.x = m_Size.x;

		BPosition = m_PositionClient;
		BPosition.y += m_Size.y - GUI_BUTTON_SIZE.y;

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