#include "JWScrollBar.h"
#include "../../CoreBase/JWImage.h"
#include "../../CoreBase/JWWindow.h"

using namespace JWENGINE;

// Static const
const D3DXVECTOR2& JWScrollBar::HORIZONTAL_MINIMUM_SIZE{ GUI_BUTTON_SIZE.x * 2, GUI_BUTTON_SIZE.y };
const D3DXVECTOR2& JWScrollBar::VERTICAL_MINIMUM_SIZE{ GUI_BUTTON_SIZE.x, GUI_BUTTON_SIZE.y * 2 };

void JWScrollBar::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type
	m_ControlType = EControlType::ScrollBar;

	// Set default alignment
	SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);

	m_pBackground = MAKE_UNIQUE(JWImage)();
	m_pBackground->Create(*m_pSharedData->pWindow, m_pSharedData->BaseDir);
	m_pBackground->SetColor(DEFAULT_COLOR_NORMAL);

	m_pButtonA = MAKE_UNIQUE(JWImageButton)();
	m_pButtonA->Create(m_Position, GUI_BUTTON_SIZE, SharedData);
	m_pButtonA->ShouldBeOffsetByMenuBar(false);
	m_pButtonA->ShouldDrawBorder(false);
	m_pButtonA->SetParentControl(this);

	m_pButtonB = MAKE_UNIQUE(JWImageButton)();
	m_pButtonB->Create(m_Position, GUI_BUTTON_SIZE, SharedData);
	m_pButtonB->ShouldBeOffsetByMenuBar(false);
	m_pButtonB->ShouldDrawBorder(false);
	m_pButtonB->SetParentControl(this);

	m_pScroller = MAKE_UNIQUE(JWImageButton)();
	m_pScroller->Create(m_Position, GUI_BUTTON_SIZE, SharedData);
	m_pScroller->ShouldBeOffsetByMenuBar(false);
	m_pScroller->ShouldDrawBorder(false);
	m_pScroller->SetParentControl(this);
	m_pScroller->SetControlStateColor(EControlState::Normal, DEFAULT_COLOR_LESS_WHITE);
	m_pScroller->SetControlStateColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_WHITE);
	m_pScroller->SetControlStateColor(EControlState::Pressed, DEFAULT_COLOR_WHITE);

	// Set control's position.
	// SetSize() must be called in MakeScrollBar()
	// in order to adjust the size according to the ScrollBar's direction.
	// (Because the direction is decided when MakeScrollBar() is called.)
	SetPosition(Position);
}

auto JWScrollBar::MakeScrollBar(EScrollBarDirection Direction) noexcept->JWControl*
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

	// @warning: SetPosition with m_PositionAbsolute in order not to do a duplicate offset.
	SetPosition(m_AbsolutePosition);
	SetSize(m_Size);
	
	UpdateButtonPosition();
	UpdateButtonSize();

	return this;
}

PROTECTED void JWScrollBar::UpdateViewport() noexcept
{
	JWControl::UpdateViewport();

	m_pButtonA->UpdateViewport();
	m_pButtonB->UpdateViewport();
	m_pScroller->UpdateViewport();
}

PROTECTED void JWScrollBar::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept
{
	JWControl::UpdateControlState(ppControlWithMouse, ppControlWithFocus);

	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	if (m_ButtonABPressInterval < BUTTON_INTERVAL_UPPER_LIMIT)
	{
		m_ButtonABPressInterval++;
	}

	m_pButtonA->UpdateControlState(nullptr, nullptr);
	m_pButtonB->UpdateControlState(nullptr, nullptr);
	
	EControlState button_a_state = m_pButtonA->GetControlState();
	EControlState button_b_state = m_pButtonB->GetControlState();

	if ((button_a_state == EControlState::Clicked) || (button_a_state == EControlState::Pressed))
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
	else if ((button_b_state == EControlState::Clicked) || (button_b_state == EControlState::Pressed))
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
	else if (m_pScroller->GetControlState() == EControlState::Pressed)
	{
		SetControlState(EControlState::Pressed);

		if (!m_bScrollerCaptured)
		{
			m_CapturedScrollPosition = m_ScrollPosition;

			m_bScrollerCaptured = true;
		}

		if (m_bScrollerCaptured)
		{
			if (m_pSharedData->pWindow->GetWindowInputStatePtr()->MouseLeftPressed)
			{
				// When the scroller is being dragged.
				long long scroller_stride_x = static_cast<long long>(p_input_state->MouseMovedPosition.x);
				long long scroller_stride_y = static_cast<long long>(p_input_state->MouseMovedPosition.y);
				long long stride_unit = static_cast<long long>(m_ScrollableRest / static_cast<float>(m_ScrollMax));
				long long new_scroll_position = m_CapturedScrollPosition;

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
				new_scroll_position = min(new_scroll_position, static_cast<long long>(m_ScrollMax));

				SetScrollPosition(new_scroll_position);
			}
			else
			{
				// When the scroller is released.
				m_pScroller->UpdateControlState(nullptr, nullptr);

				m_CapturedScrollPosition = 0;
			}
		}
	}
	else if ((m_ControlState == EControlState::Pressed) && (m_pScroller->GetControlState() == EControlState::Normal)
		&& (button_a_state == EControlState::Normal) && (button_b_state == EControlState::Normal))
	{
		// Press on the body of the scrollbar => Page scroll

		// Calculate digital position
		size_t digital_position = CalculateScrollerDigitalPosition(p_input_state->MouseDownPosition);

		long long scroll_stride = digital_position - m_ScrollPosition;

		if (scroll_stride > 0)
		{
			if (scroll_stride > DEFAULT_PAGE_STRIDE)
			{
				scroll_stride = DEFAULT_PAGE_STRIDE;
			}
		}
		else if (scroll_stride < 0)
		{
			if (scroll_stride < -DEFAULT_PAGE_STRIDE)
			{
				scroll_stride = -DEFAULT_PAGE_STRIDE;
			}
		}

		long long new_scroll_position = m_ScrollPosition + scroll_stride;
		new_scroll_position = max(new_scroll_position, 0);

		SetScrollPosition(new_scroll_position);

		return;
	}
	else
	{
		// When no button is pressed,
		// release the scroller
		m_bScrollerCaptured = false;
		m_pScroller->UpdateControlState(nullptr, nullptr);

		m_ButtonABPressInterval = BUTTON_INTERVAL_UPPER_LIMIT;
	}
}

void JWScrollBar::Draw() noexcept
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

auto JWScrollBar::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_Position);

	UpdateButtonPosition();

	return this;
}

auto JWScrollBar::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl*
{
	D3DXVECTOR2 adjusted_size = Size;

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		adjusted_size.x = max(adjusted_size.x, HORIZONTAL_MINIMUM_SIZE.x);
		adjusted_size.y = max(adjusted_size.y, HORIZONTAL_MINIMUM_SIZE.y);
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		adjusted_size.x = max(adjusted_size.x, VERTICAL_MINIMUM_SIZE.x);
		adjusted_size.y = max(adjusted_size.y, VERTICAL_MINIMUM_SIZE.y);
		break;
	default:
		break;
	}

	JWControl::SetSize(adjusted_size);

	m_pBackground->SetSize(m_Size);

	UpdateButtonPosition();
	UpdateButtonSize();

	return this;
}

PROTECTED void JWScrollBar::SetControlState(EControlState State) noexcept
{
	JWControl::SetControlState(State);

	if (State == EControlState::Normal)
	{
		// When mouse is off scrollbar, Buttons must be set to normal state as well
		m_pButtonA->SetControlState(State);
		m_pButtonB->SetControlState(State);

		if (!m_pSharedData->pWindow->GetWindowInputStatePtr()->MouseLeftPressed)
		{
			m_pScroller->SetControlState(State);
		}
	}
}

auto JWScrollBar::SetScrollRange(size_t VisibleUnitCount, size_t TotalUnitCount) noexcept->JWControl*
{
	m_VisibleUnitCount = VisibleUnitCount;
	m_TotalUnitCount = TotalUnitCount;

	m_ScrollMax = m_TotalUnitCount - m_VisibleUnitCount;

	UpdateButtonSize();
	UpdateButtonPosition();

	return this;
}

auto JWScrollBar::SetScrollPosition(size_t Position) noexcept->JWControl*
{
	Position = min(Position, m_ScrollMax);
	
	m_ScrollPosition = Position;

	float scroller_position = 0;

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		scroller_position = (m_ScrollableSize / static_cast<float>(m_TotalUnitCount)) * m_ScrollPosition;
		MoveScrollerTo(D3DXVECTOR2(scroller_position, 0));
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		scroller_position = (m_ScrollableSize / static_cast<float>(m_TotalUnitCount)) * m_ScrollPosition;
		MoveScrollerTo(D3DXVECTOR2(0, scroller_position));
		break;
	default:
		break;
	}

	return this;
}

auto JWScrollBar::GetScrollRange() const noexcept->size_t
{
	return m_ScrollMax;
}

auto JWScrollBar::GetScrollPosition() const noexcept->size_t
{
	return m_ScrollPosition;
}

PRIVATE void JWScrollBar::UpdateButtonSize() noexcept
{
	D3DXVECTOR2 ABSize = GUI_BUTTON_SIZE;
	D3DXVECTOR2 APosition = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 BPosition = D3DXVECTOR2(0, 0);

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		ABSize.y = m_Size.y;

		// Update scrollable size
		m_ScrollableSize = m_Size.x - GUI_BUTTON_SIZE.x * 2;

		m_ScrollerSize.x = (m_ScrollableSize / static_cast<float>(m_TotalUnitCount)) * static_cast<float>(m_VisibleUnitCount);

		m_ScrollerSize.x = min(m_ScrollerSize.x, m_ScrollableSize);
		m_ScrollerSize.x = max(m_ScrollerSize.x, GUI_BUTTON_SIZE.x);

		m_ScrollerSize.y = m_Size.y - static_cast<float>(DEFAULT_SCROLLER_PADDING * 2);

		m_ScrollableRest = m_ScrollableSize - m_ScrollerSize.x;
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		ABSize.x = m_Size.x;

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

	m_pButtonA->SetSize(ABSize);
	m_pButtonB->SetSize(ABSize);
	m_pScroller->SetSize(m_ScrollerSize);
}

PRIVATE void JWScrollBar::UpdateButtonPosition() noexcept
{
	D3DXVECTOR2 APosition = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 BPosition = D3DXVECTOR2(0, 0);

	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		BPosition = m_Position;
		BPosition.x += m_Size.x - GUI_BUTTON_SIZE.x;
		m_ScrollerPosition.y = static_cast<float>(DEFAULT_SCROLLER_PADDING);
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		BPosition = m_Position;
		BPosition.y += m_Size.y - GUI_BUTTON_SIZE.y;
		m_ScrollerPosition.x = static_cast<float>(DEFAULT_SCROLLER_PADDING);
		break;
	default:
		break;
	}

	m_pButtonA->SetPosition(m_Position);
	m_pButtonB->SetPosition(BPosition);
	m_pScroller->SetPosition(m_Position + m_ScrollerPosition);
}

PRIVATE void JWScrollBar::MoveScrollerTo(const D3DXVECTOR2& Position) noexcept
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

	m_pScroller->SetPosition(m_Position + m_ScrollerPosition);
}

PRIVATE auto JWScrollBar::CalculateScrollerDigitalPosition(const POINT& MousesPosition) const noexcept->size_t
{
	size_t Result = 0;

	float ratio = 0;
	switch (m_ScrollBarDirection)
	{
	case JWENGINE::EScrollBarDirection::Horizontal:
		ratio = static_cast<float>(MousesPosition.x - m_Position.x - GUI_BUTTON_SIZE.x) / m_ScrollableSize;
		break;
	case JWENGINE::EScrollBarDirection::Vertical:
		ratio = static_cast<float>(MousesPosition.y - m_Position.y - GUI_BUTTON_SIZE.y) / m_ScrollableSize;
		break;
	default:
		break;
	}

	float temp = ratio * static_cast<float>(m_ScrollMax + 1);
	Result = static_cast<size_t>(temp);

	return Result;
}