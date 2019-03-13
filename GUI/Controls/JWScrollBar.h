#pragma once

#include "JWControl.h"
#include "JWImageButton.h"

namespace JWENGINE
{
	class JWScrollBar final : public JWControl
	{
	friend class JWControl;
	friend class JWListBox;

	public:
		JWScrollBar() {};
		~JWScrollBar() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		auto MakeScrollBar(EScrollBarDirection Direction) noexcept->JWControl& override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl& override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl& override;
		auto SetScrollRange(size_t VisibleUnitCount, size_t TotalUnitCount) noexcept->JWControl& override;
		auto SetScrollPosition(size_t Position) noexcept->JWControl& override;

		auto GetScrollRange() const noexcept->size_t override;
		auto GetScrollPosition() const noexcept->size_t override;

	protected:
		// Must be overridden.
		void UpdateViewport() noexcept;

		// Must be overridden.
		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept override;
		

		// Must be overridden.
		void SetControlState(EControlState State) noexcept override;

	private:
		void UpdateButtonSize() noexcept;
		void UpdateButtonPosition() noexcept;
		void MoveScrollerTo(const D3DXVECTOR2& Position) noexcept;

		auto CalculateScrollerDigitalPosition(const POINT& MousesPosition) const noexcept->size_t;

	private:
		static constexpr int DEFAULT_SCROLLER_PADDING{ 2 };
		static constexpr int DEFAULT_PAGE_STRIDE{ 1 };
		static constexpr int DEFAULT_AB_PRESS_TICK{ 20 };
		static constexpr int BUTTON_INTERVAL_UPPER_LIMIT{ 100 };
		static const D3DXVECTOR2 HORIZONTAL_MINIMUM_SIZE;
		static const D3DXVECTOR2 VERTICAL_MINIMUM_SIZE;

		JWImage m_Background;
		JWImageButton m_ButtonA;
		JWImageButton m_ButtonB;
		JWImageButton m_Scroller;

		size_t m_ButtonABPressInterval{};
		size_t m_ButtonABPressIntervalTick{ DEFAULT_AB_PRESS_TICK };

		EScrollBarDirection m_ScrollBarDirection{ EScrollBarDirection::Invalid };
		D3DXVECTOR2 m_ScrollerSize{ GUI_BUTTON_SIZE.x, GUI_BUTTON_SIZE.y };

		// m_ScrollerPosition range = [GUI_BUTTON_SIZE, m_ScrollableRest]
		D3DXVECTOR2 m_ScrollerPosition{};
		bool m_bScrollerCaptured{ false };

		// Scroll range = [0, ScrollMax]
		size_t m_ScrollMax{};
		size_t m_VisibleUnitCount{};
		size_t m_TotalUnitCount{};
		size_t m_ScrollPosition{};
		size_t m_CapturedScrollPosition{};
		float m_ScrollableSize{};
		float m_ScrollableRest{};
	};
};