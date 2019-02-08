#include "JWScrollBar.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWWindow.h"
#include "../GUI/JWTextButton.h"
#include "../GUI/JWImageButton.h"

using namespace JWENGINE;

// Static const
const D3DXVECTOR2 JWScrollBar::HORIZONTAL_MINIMUM_SIZE = D3DXVECTOR2(GUI_BUTTON_SIZE.x * 2, GUI_BUTTON_SIZE.y);
const D3DXVECTOR2 JWScrollBar::VERTICAL_MINIMUM_SIZE = D3DXVECTOR2(GUI_BUTTON_SIZE.x, GUI_BUTTON_SIZE.y * 2);

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
	m_VisibleUnitCount = 0;
	m_TotalUnitCount = 0;
	m_ScrollPosition = 0;
	m_CapturedScrollPosition = 0;
	m_ScrollableSize = 0;
}

auto JWScrollBar::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetColor(DEFAULT_COLOR_NORMAL);
	}
	else
	{
		return EError::IMAGE_NOT_CREATED;
	}

	if (m_pButtonA = new JWImageButton)
	{
		if (JW_FAILED(m_pButtonA->Create(Position, GUI_BUTTON_SIZE)))
			return EError::IMAGEBUTTON_NOT_CREATED;

		m_pButtonA->ShouldDrawBorder(false);
	}
	else
	{
		return EError::IMAGEBUTTON_NOT_CREATED;
	}

	if (m_pButtonB = new JWImageButton)
	{
		if (JW_FAILED(m_pButtonB->Create(Position, GUI_BUTTON_SIZE)))
			return EError::IMAGEBUTTON_NOT_CREATED;

		m_pButtonB->ShouldDrawBorder(false);
	}
	else
	{
		return EError::IMAGEBUTTON_NOT_CREATED;
	}

	if (m_pScroller = new JWTextButton)
	{
		if (JW_FAILED(m_pScroller->Create(Position, GUI_BUTTON_SIZE)))
			return EError::IMAGEBUTTON_NOT_CREATED;

		m_pScroller->ShouldDrawBorder(false);
		m_pScroller->SetStateColor(EControlState::Normal, DEFAULT_COLOR_LESS_WHITE);
		m_pScroller->SetStateColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_WHITE);
		m_pScroller->SetStateColor(EControlState::Pressed, DEFAULT_COLOR_WHITE);
	}
	else
	{
		return EError::IMAGEBUTTON_NOT_CREATED;
	}

	// Set default alignment
	SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	// Set control type
	m_ControlType = EControlType::ScrollBar;

	// Set control's position.
	// SetSize() must be called in MakeScrollBar()
	// in order to adjust the size according to the ScrollBar's direction.
	// (Because the direction is decided when MakeScrollBar() is called.)
	SetPosition(Position);

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
			m_ButtonABPressIntervalTick = 4;
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
		SetState(EControlState::Pressed);

		if (!m_bScrollerCaptured)
		{
			m_CapturedScrollPosition = m_ScrollPosition;

			m_bScrollerCaptured = true;
		}

		if (m_bScrollerCaptured)
		{
			if (ms_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
			{
				// When the scroller is being dragged.
				int scroller_stride_x = MouseData.MousePosition.x - MouseData.MouseDownPosition.x;
				int scroller_stride_y = MouseData.MousePosition.y - MouseData.MouseDownPosition.y;
				int stride_unit = static_cast<int>(m_ScrollableRest / static_cast<float>(m_ScrollMax));
				int new_scroll_position = static_cast<int>(m_CapturedScrollPosition);

				switch (m_ScrollBarDirection)
				{
				case JWENGINE::EScrollBarDirection::Horizontal:
					if (stride_unit)
					{
						// To avoid division by 0.
						scroller_stride_x = scroller_stride_x / stride_unit;
					}
					
					new_scroll_position += scroller_stride_x;
					break;
				case JWENGINE::EScrollBarDirection::Vertical:
					if (stride_unit)
					{
						// To avoid division by 0.
						scroller_stride_y = scroller_stride_y / stride_unit;
					}

					new_scroll_position += scroller_stride_y;
					break;
				default:
					break;
				}

				new_scroll_position = max(new_scroll_position, 0);
				new_scroll_position = min(new_scroll_position, static_cast<int>(m_ScrollMax));

				SetScrollPosition(static_cast<size_t>(new_scroll_position));
			}
			else
			{
				// When the scroller is released.
				m_pScroller->UpdateControlState(MouseData);

				m_CapturedScrollPosition = 0;
			}
		}
	}
	else if ((m_ControlState == EControlState::Pressed) && (m_pScroller->GetState() == EControlState::Normal)
		&& (ButtonAState == EControlState::Normal) && (ButtonBState == EControlState::Normal))
	{
		// Press on the body of the scrollbar => Page scroll

		// Calculate digital position
		size_t ObjectDigitalPosition = CalculateScrollerDigitalPosition(MouseData.MouseDownPosition);

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

		long long new_scroll_position = m_ScrollPosition + ScrollStride;
		new_scroll_position = max(new_scroll_position, 0);

		SetScrollPosition(new_scroll_position);

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
	JWControl::BeginDrawing();

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

	m_pBackground->Draw();

	m_pScroller->Draw();

	m_pButtonA->Draw();
	m_pButtonB->Draw();

	JWControl::EndDrawing();
}

void JWScrollBar::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_PositionClient);

	UpdateButtonPosition();
}

void JWScrollBar::SetSize(D3DXVECTOR2 Size)
{
	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		Size.x = max(Size.x, HORIZONTAL_MINIMUM_SIZE.x);
		Size.y = max(Size.y, HORIZONTAL_MINIMUM_SIZE.y);
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		Size.x = max(Size.x, VERTICAL_MINIMUM_SIZE.x);
		Size.y = max(Size.y, VERTICAL_MINIMUM_SIZE.y);
		break;
	default:
		break;
	}

	JWControl::SetSize(Size);

	m_pBackground->SetSize(m_Size);

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

		if (!ms_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
		{
			m_pScroller->SetState(State);
		}
	}
}

void JWScrollBar::SetScrollRange(size_t VisibleUnitCount, size_t TotalUnitCount)
{
	m_VisibleUnitCount = VisibleUnitCount;
	m_TotalUnitCount = TotalUnitCount;

	m_ScrollMax = m_TotalUnitCount - m_VisibleUnitCount;

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
		new_position = (m_ScrollableSize / static_cast<float>(m_TotalUnitCount)) * m_ScrollPosition;
		MoveScrollerTo(D3DXVECTOR2(new_position, 0));
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		new_position = (m_ScrollableSize / static_cast<float>(m_TotalUnitCount)) * m_ScrollPosition;
		MoveScrollerTo(D3DXVECTOR2(0, new_position));
		break;
	default:
		break;
	}
}

auto JWScrollBar::GetScrollRange() const->size_t
{
	return m_ScrollMax;
}

auto JWScrollBar::GetScrollPosition() const->size_t
{
	return m_ScrollPosition;
}

PRIVATE void JWScrollBar::UpdateButtonSize()
{
	D3DXVECTOR2 NewSize = GUI_BUTTON_SIZE;
	D3DXVECTOR2 APosition = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 BPosition = D3DXVECTOR2(0, 0);

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		NewSize.y = m_Size.y;

		// Update scrollable size
		m_ScrollableSize = m_Size.x - GUI_BUTTON_SIZE.x * 2;

		m_ScrollerSize.x = (m_ScrollableSize / static_cast<float>(m_TotalUnitCount)) * static_cast<float>(m_VisibleUnitCount);

		m_ScrollerSize.x = min(m_ScrollerSize.x, m_ScrollableSize);
		m_ScrollerSize.x = max(m_ScrollerSize.x, GUI_BUTTON_SIZE.x);

		m_ScrollerSize.y = m_Size.y - static_cast<float>(DEFAULT_SCROLLER_PADDING * 2);

		m_ScrollableRest = m_ScrollableSize - m_ScrollerSize.x;
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		NewSize.x = m_Size.x;

		m_ScrollableSize = m_Size.y - GUI_BUTTON_SIZE.y * 2;

		m_ScrollerSize.y = (m_ScrollableSize / static_cast<float>(m_TotalUnitCount)) * static_cast<float>(m_VisibleUnitCount);

		m_ScrollerSize.y = min(m_ScrollerSize.y, m_ScrollableSize);
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
		m_ScrollerPosition.x = GUI_BUTTON_SIZE.x + Position.x;

		m_ScrollerPosition.x = max(m_ScrollerPosition.x, GUI_BUTTON_SIZE.x);
		m_ScrollerPosition.x = min(m_ScrollerPosition.x, m_ScrollableRest + GUI_BUTTON_SIZE.x);
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		m_ScrollerPosition.y = GUI_BUTTON_SIZE.y + Position.y;

		m_ScrollerPosition.y = max(m_ScrollerPosition.y, GUI_BUTTON_SIZE.y);
		m_ScrollerPosition.y = min(m_ScrollerPosition.y, m_ScrollableRest + GUI_BUTTON_SIZE.y);
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