#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWLine;
	class JWRectangle;
	// ***

	class JWEdit final : public JWControl
	{
	public:
		JWEdit();
		~JWEdit() {};

		auto Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void Draw() override;

		void Focus() override;

		void OnKeyDown(WPARAM VirtualKeyCode) override;
		void OnMouseDown(LPARAM MousePosition) override;
		void OnMouseMove(LPARAM MousePosition) override;

	private:
		void GetSelStartAndEndData();
		void UpdateCaret();
		void UpdateSelection();

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND = D3DCOLOR_XRGB(200, 200, 200);

		JWLine* m_pCaret;
		JWRectangle* m_pSelection;

		size_t m_SelStart;
		size_t m_SelEnd;

		SLineData m_SelStartLineData;
		SLineData m_SelEndLineData;
	};
};