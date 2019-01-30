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
		void OnCharKey(WPARAM Char) override;
		void OnMouseDown(LPARAM MousePosition) override;
		void OnMouseMove(LPARAM MousePosition) override;
		void CheckIME() override;

	private:
		void SelectionToLeft(size_t Stride = 1);
		void SelectionToRight(size_t Stride = 1);

		void GetSelStartAndEndData();
		void UpdateCaret();
		void UpdateSelection();

		auto IsTextSelected() const->bool;
		void EraseAfter();
		void EraseBefore();
		void EraseSelectedText();
		void InsertNewLine();
		void InsertChar(wchar_t Char);

		void UpdateTextCaretSelection();

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND = D3DCOLOR_XRGB(200, 200, 200);
		static const ULONGLONG DEFAULT_CARET_TICK = 30;

		JWLine* m_pCaret;
		JWRectangle* m_pSelection;

		size_t m_SelStart;
		size_t m_SelEnd;
		size_t m_CapturedSelPosition;
		size_t* m_pCaretSelPosition;

		SLineData m_SelStartLineData;
		SLineData m_SelEndLineData;

		WSTRING m_IMETempText;
		size_t m_IMETempSel;

		ULONGLONG m_CaretTickCount;
		WSTRING m_ClipText;
	};
};