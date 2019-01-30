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

		auto Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void MakeScrollBar(EScrollBarDirection Direction) override;

		void UpdateState(const SMouseData& MouseData) override; // Must override

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;
		void SetState(EControlState State) override; // Must override
		void SetScrollRange(size_t Max) override;
		void SetScrollPosition(size_t Position) override;

	private:
		void SetButtonSizeAndPosition();
		void MoveScroller(D3DXVECTOR2 Move);

	private:
		static const int DEFAULT_SCROLLER_PADDING = 2;

		JWImage* m_pBackground;
		JWImageButton* m_pButtonA;
		JWImageButton* m_pButtonB;
		JWTextButton* m_pScroller;

		EScrollBarDirection m_ScrollBarDirection;
		D3DXVECTOR2 m_ScrollerSize;
		D3DXVECTOR2 m_ScrollerPosition;
		D3DXVECTOR2 m_CapturedScrollerPosition;
		bool m_bScrollerCaptured;

		size_t m_ScrollMax;
		size_t m_ScrollPosition;
	};
};