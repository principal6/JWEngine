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
	public:
		JWScrollBar();
		~JWScrollBar() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUISharedData* pSharedData)->EError override;
		void Destroy() override;

		void MakeScrollBar(EScrollBarDirection Direction) override;

		void UpdateControlState(const SMouseData& MouseData) override; // Must override

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;
		void SetState(EControlState State) override; // Must override
		void SetScrollRange(size_t VisibleUnitCount, size_t TotalUnitCount) override;
		void SetScrollPosition(size_t Position) override;

		auto GetScrollRange() const->size_t override;
		auto GetScrollPosition() const->size_t override;

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