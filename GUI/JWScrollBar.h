#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	class JWTextButton;
	class JWImageButton;
	// ***

	class JWScrollBar final : public JWControl
	{
	friend class JWControl;
	friend class JWListBox;

	public:
		JWScrollBar();
		~JWScrollBar() {};

		auto Create(const D3DXVECTOR2 Position, const D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		auto MakeScrollBar(const EScrollBarDirection Direction)->JWControl* override;

		void Draw() override;

		auto SetPosition(const D3DXVECTOR2 Position)->JWControl* override;
		auto SetSize(const D3DXVECTOR2 Size)->JWControl* override;
		auto SetScrollRange(const size_t VisibleUnitCount, const size_t TotalUnitCount)->JWControl* override;
		auto SetScrollPosition(const size_t Position)->JWControl* override;

		auto GetScrollRange() const->size_t override;
		auto GetScrollPosition() const->size_t override;

	protected:
		// Must be overridden.
		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) override;

		// Must be overridden.
		void SetControlState(EControlState State) override;

	private:
		void UpdateButtonSize();
		void UpdateButtonPosition();
		void MoveScrollerTo(D3DXVECTOR2 Position);

		auto CalculateScrollerDigitalPosition(POINT MousesPosition) const->size_t;

	private:
		static const int DEFAULT_SCROLLER_PADDING = 2;
		static const int DEFAULT_PAGE_STRIDE = 1;
		static const int BUTTON_INTERVAL_UPPER_LIMIT = 100;
		static const D3DXVECTOR2 HORIZONTAL_MINIMUM_SIZE;
		static const D3DXVECTOR2 VERTICAL_MINIMUM_SIZE;

		JWImage* m_pBackground;
		JWImageButton* m_pButtonA;
		JWImageButton* m_pButtonB;
		JWTextButton* m_pScroller;

		size_t m_ButtonABPressInterval;
		size_t m_ButtonABPressIntervalTick;

		EScrollBarDirection m_ScrollBarDirection;
		D3DXVECTOR2 m_ScrollerSize;

		// m_ScrollerPosition range = [GUI_BUTTON_SIZE, m_ScrollableRest]
		D3DXVECTOR2 m_ScrollerPosition;
		bool m_bScrollerCaptured;

		// Scroll range = [0, ScrollMax]
		size_t m_ScrollMax;
		size_t m_VisibleUnitCount;
		size_t m_TotalUnitCount;
		size_t m_ScrollPosition;
		size_t m_CapturedScrollPosition;
		float m_ScrollableSize;
		float m_ScrollableRest;
	};
};