#include "JWScrollBar.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWWindow.h"
#include "../GUI/JWTextButton.h"
#include "../GUI/JWImageButton.h"

using namespace JWENGINE;

JWScrollBar::JWScrollBar()
{
	m_pBackground = nullptr;
	m_pButtonA = nullptr;
	m_pButtonB = nullptr;
	m_pScroller = nullptr;

	m_ButtonABPressInterval = 0;
	m_ButtonABPressIntervalTick = 20;

	m_ScrollerSize = D3DXVECTOR2(20, 20);
	m_ScrollerPosition = D3DXVECTOR2(0, 0);

	m_bScrollerCaptured = false;

	m_ScrollMax = 0;
	m_ScrollPosition = 0;
	m_ScrollableSize = 0;
}

auto JWScrollBar::Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(pWindow, BaseDir, Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pWindow, ms_BaseDir)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetXRGB(DEFAULT_COLOR_NORMAL);
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
		m_pScroller->SetButtonColor(EControlState::Normal, DEFAULT_COLOR_LESS_WHITE);
		m_pScroller->SetButtonColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_WHITE);
		m_pScroller->SetButtonColor(EControlState::Pressed, DEFAULT_COLOR_WHITE);
	}
	else
	{
		return EError::IMAGEBUTTON_NOT_CREATED;
	}

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

	UpdateButtonSize();
	UpdateButtonPosition();
}

void JWScrollBar::UpdateControlState(const SMouseData& MouseData)
{
	if (m_ButtonABPressInterval < BUTTON_INTERVAL_UPPER_LIMIT)
	{
		m_ButtonABPressInterval++;
	}

	JWControl::UpdateControlState(MouseData);

	m_pButtonA->UpdateControlState(MouseData);
	m_pButtonB->UpdateControlState(MouseData);
	
	EControlState ButtonAState = m_pButtonA->GetState();
	EControlState ButtonBState = m_pButtonB->GetState();

	if ((ButtonAState == EControlState::Clicked) || (ButtonAState == EControlState::Pressed))
	{
		if (m_ButtonABPressInterval == BUTTON_INTERVAL_UPPER_LIMIT)
		{
			if (m_ScrollPosition)
			{
				SetScrollPosition(m_ScrollPosition - 1);
			}

			m_ButtonABPressInterval = 0;
			m_ButtonABPressIntervalTick = 30;
		}
		else if (m_ButtonABPressInterval >= m_ButtonABPressIntervalTick)
		{
			if (m_ScrollPosition)
			{
				SetScrollPosition(m_ScrollPosition - 1);
			}

			m_ButtonABPressInterval = 0;
			m_ButtonABPressIntervalTick = 5;
		}
	}
	else if ((ButtonBState == EControlState::Clicked) || (ButtonBState == EControlState::Pressed))
	{
		if (m_ButtonABPressInterval == BUTTON_INTERVAL_UPPER_LIMIT)
		{
			SetScrollPosition(m_ScrollPosition + 1);

			m_ButtonABPressInterval = 0;
			m_ButtonABPressIntervalTick = 30;
		}
		else if (m_ButtonABPressInterval >= m_ButtonABPressIntervalTick)
		{
			SetScrollPosition(m_ScrollPosition + 1);

			m_ButtonABPressInterval = 0;
			m_ButtonABPressIntervalTick = 5;
		}
	}
	else if (m_pScroller->GetState() == EControlState::Pressed)
	{
		if (!m_bScrollerCaptured)
		{
			m_CapturedScrollerPosition = m_ScrollerPosition - GUI_BUTTON_SIZE;
			m_bScrollerCaptured = true;
		}

		if (m_bScrollerCaptured)
		{
			if (ms_pWindow->GetWindowInputState()->MouseLeftPressed)
			{
				// When the scroller is being dragged.
				POINT ClientMouseDownPosition = m_pScroller->GetClientMouseDownPosition();

				D3DXVECTOR2 new_position = D3DXVECTOR2(0, 0);
				new_position.x = static_cast<float>(MouseData.MousePosition.x - MouseData.MouseDownPosition.x);
				new_position.y = static_cast<float>(MouseData.MousePosition.y - MouseData.MouseDownPosition.y);

				MoveScrollerTo(new_position);
			}
			else
			{
				// When the scroller is released.
				m_pScroller->UpdateControlState(MouseData);

				m_CapturedScrollerPosition.x = 0;
				m_CapturedScrollerPosition.y = 0;

				// Calculate digital position
				float position_float = 0;
				int position_int = 0;
				switch (m_ScrollBarDirection)
				{
				case JWENGINE::EScrollBarDirection::Horizontal:
					position_float = (m_ScrollerPosition.x - GUI_BUTTON_SIZE.x) / (m_ScrollableRest - GUI_BUTTON_SIZE.x);
					position_float *= m_ScrollMax;
					position_int = static_cast<int>(position_float);
					SetScrollPosition(position_int);
					break;
				case JWENGINE::EScrollBarDirection::Vertical:
					position_float = (m_ScrollerPosition.y - GUI_BUTTON_SIZE.y) / (m_ScrollableRest - GUI_BUTTON_SIZE.y);
					position_float *= m_ScrollMax;
					position_int = static_cast<int>(position_float);
					SetScrollPosition(position_int);
					break;
				default:
					break;
				}
			}
		}
	}
	else if ((m_ControlState == EControlState::Pressed) && (m_pScroller->GetState() == EControlState::Normal)
		&& (ButtonAState == EControlState::Normal) && (ButtonBState == EControlState::Normal))
	{
		// Calculate digital position
		size_t ObjectDigitalPosition = CalculateScrollerDigitalPosition(MouseData.MouseDownPosition);

		// Press on the body of the scrollbar => Page scroll
		long long ScrollStride = 0;

		ScrollStride = ObjectDigitalPosition - m_ScrollPosition;

		if (ScrollStride > 0)
		{
			if (ScrollStride > DEFAULT_PAGE_STRIDE)
			{
				ScrollStride = DEFAULT_PAGE_STRIDE;
			}
		}
		else if (ScrollStride < 0)
		{
			if (ScrollStride < -DEFAULT_PAGE_STRIDE)
			{
				ScrollStride = -DEFAULT_PAGE_STRIDE;
			}
		}

		SetScrollPosition(m_ScrollPosition + ScrollStride);

		return;
	}
	else
	{
		// When no button is pressed,
		// release the scroller
		m_bScrollerCaptured = false;
		m_pScroller->UpdateControlState(MouseData);

		m_ButtonABPressInterval = BUTTON_INTERVAL_UPPER_LIMIT;
	}
}

void JWScrollBar::Draw()
{
	switch (m_ControlState)
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

	// Draw text
	m_pFont->Draw();
}

void JWScrollBar::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);
	m_pBackground->SetPosition(Position);

	UpdateButtonPosition();
}

void JWScrollBar::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);
	m_pBackground->SetSize(Size);

	UpdateButtonPosition();
	UpdateButtonSize();
}

void JWScrollBar::SetState(EControlState State)
{
	JWControl::SetState(State);

	if (State == EControlState::Normal)
	{
		// When mouse is off scrollbar, Buttons must be set to normal state as well
		m_pButtonA->SetState(State);
		m_pButtonB->SetState(State);

		if (!ms_pWindow->GetWindowInputState()->MouseLeftPressed)
		{
			m_pScroller->SetState(State);
		}
	}
}

void JWScrollBar::SetScrollRange(size_t Max)
{
	m_ScrollMax = Max;

	UpdateButtonSize();
	UpdateButtonPosition();
}

void JWScrollBar::SetScrollPosition(size_t Position)
{
	if (Position >= m_ScrollMax)
		Position = m_ScrollMax;

	m_ScrollPosition = Position;

	float new_position = 0;

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		new_position = ((m_ScrollableRest + GUI_BUTTON_SIZE.x) / static_cast<float>(m_ScrollMax + 1)) * m_ScrollPosition;
		MoveScrollerTo(D3DXVECTOR2(new_position, 0));
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		new_position = ((m_ScrollableRest + GUI_BUTTON_SIZE.y) / static_cast<float>(m_ScrollMax + 1)) * m_ScrollPosition;
		MoveScrollerTo(D3DXVECTOR2(0, new_position));
		break;
	default:
		break;
	}
}

PRIVATE void JWScrollBar::UpdateButtonSize()
{
	D3DXVECTOR2 NewSize = GUI_BUTTON_SIZE;
	D3DXVECTOR2 APosition = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 BPosition = D3DXVECTOR2(0, 0);

	int new_size_int = 0;

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		NewSize.y = m_Size.y;

		m_ScrollableSize = m_Size.x - GUI_BUTTON_SIZE.x * 2;
		
		new_size_int = static_cast<int>((m_ScrollableSize / static_cast<float>(m_ScrollMax + 1) * 3));
		m_ScrollerSize.x = static_cast<float>(new_size_int);

		m_ScrollerSize.x = max(m_ScrollerSize.x, GUI_BUTTON_SIZE.x);

		m_ScrollerSize.y = m_Size.y - static_cast<float>(DEFAULT_SCROLLER_PADDING * 2);

		m_ScrollableRest = m_ScrollableSize - m_ScrollerSize.x;
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		NewSize.x = m_Size.x;

		m_ScrollableSize = m_Size.y - GUI_BUTTON_SIZE.y * 2;

		new_size_int = static_cast<int>((m_ScrollableSize / static_cast<float>(m_ScrollMax + 1) * 3));
		m_ScrollerSize.y = static_cast<float>(new_size_int);

		m_ScrollerSize.y = max(m_ScrollerSize.y, GUI_BUTTON_SIZE.y);

		m_ScrollerSize.x = m_Size.x - static_cast<float>(DEFAULT_SCROLLER_PADDING * 2);

		m_ScrollableRest = m_ScrollableSize - m_ScrollerSize.y;
		break;
	default:
		break;
	}

	m_pButtonA->SetSize(NewSize);
	m_pButtonB->SetSize(NewSize);
	m_pScroller->SetSize(m_ScrollerSize);
}

PRIVATE void JWScrollBar::UpdateButtonPosition()
{
	D3DXVECTOR2 APosition = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 BPosition = D3DXVECTOR2(0, 0);

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		BPosition = m_PositionClient;
		BPosition.x += m_Size.x - GUI_BUTTON_SIZE.x;
		m_ScrollerPosition.y = static_cast<float>(DEFAULT_SCROLLER_PADDING);
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		BPosition = m_PositionClient;
		BPosition.y += m_Size.y - GUI_BUTTON_SIZE.y;
		m_ScrollerPosition.x = static_cast<float>(DEFAULT_SCROLLER_PADDING);
		break;
	default:
		break;
	}

	m_pButtonA->SetPosition(m_PositionClient);
	m_pButtonB->SetPosition(BPosition);
	m_pScroller->SetPosition(m_PositionClient + m_ScrollerPosition);
}

PRIVATE void JWScrollBar::MoveScrollerTo(D3DXVECTOR2 Position)
{
	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		m_ScrollerPosition.x = m_CapturedScrollerPosition.x + GUI_BUTTON_SIZE.x + Position.x;

		m_ScrollerPosition.x = max(m_ScrollerPosition.x, GUI_BUTTON_SIZE.x);
		m_ScrollerPosition.x = min(m_ScrollerPosition.x, m_ScrollableRest + GUI_BUTTON_SIZE.x);
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		m_ScrollerPosition.y = m_CapturedScrollerPosition.y + GUI_BUTTON_SIZE.y + Position.y;

		m_ScrollerPosition.y = max(m_ScrollerPosition.y, GUI_BUTTON_SIZE.y);
		m_ScrollerPosition.y = min(m_ScrollerPosition.y, m_ScrollableRest + GUI_BUTTON_SIZE.x);
		break;
	default:
		break;
	}

	m_pScroller->SetPosition(m_PositionClient + m_ScrollerPosition);
}

PRIVATE auto JWScrollBar::CalculateScrollerDigitalPosition(POINT MousesPosition) const->size_t
{
	size_t Result = 0;

	float ratio = 0;
	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		ratio = static_cast<float>(MousesPosition.x - m_PositionClient.x - GUI_BUTTON_SIZE.x) / m_ScrollableSize;
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		ratio = static_cast<float>(MousesPosition.y - m_PositionClient.y - GUI_BUTTON_SIZE.y) / m_ScrollableSize;
		break;
	default:
		break;
	}

	Result = static_cast<size_t>(ratio * (m_ScrollMax + 1));

	return Result;
}