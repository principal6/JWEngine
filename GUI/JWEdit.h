#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWImage;
	class JWRectangle;
	// ***

	class JWEdit final : public JWControl
	{
	public:
		JWEdit();
		~JWEdit() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUISharedData* pSharedData)->EError override;
		void Destroy() override;

		void Draw() override;

		void Focus() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;
		void SetUseMultiline(bool Value) override;

		// We must override SetText() in order to UpdateText().
		void SetText(WSTRING Text) override;

	protected:
		// Events called in JWGUI (friend class).
		void WindowMouseDown(LPARAM MousePosition) override;
		void WindowMouseMove(LPARAM MousePosition) override;
		void WindowKeyDown(WPARAM VirtualKeyCode) override;
		void WindowCharKey(WPARAM Char) override;
		void WindowIMEInput(bool Writing, bool Completed, TCHAR* pWritingTCHAR, TCHAR* pCompletedTCHAR) override;

	private:
		void SelectOrMoveCaretToLeft(size_t Stride = 1); // Select characters to the left or move the caret to the left
		void SelectOrMoveCaretToRight(size_t Stride = 1); // Select characters to the right or move the caret to the right
		void MoveCaretToLeft(size_t Stride = 1); // Move the caret to the left
		void MoveCaretToRight(size_t Stride = 1); // Move the caret to the right

		void UpdateText();
		void UpdateCaretAndSelection();
		
		void UpdateCaretPosition();
		void UpdateSelectionBox();

		auto IsTextSelected() const->bool;
		auto IsTextEmpty() const->bool;
		auto GetTextLength() const->size_t;
		void CopySelectedText();
		void PasteText();
		void InsertChar(wchar_t Char);
		void InsertString(WSTRING String);
		void InsertNewLine();
		void EraseSelectedText(bool bEraseAfter = false);
		void EraseAfter();
		void EraseBefore();
		void SelectAll();

	private:
		static const BYTE DEFAULT_ALPHA_BACKGROUND_EDIT = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND_EDIT = DEFAULT_COLOR_ALMOST_BLACK;
		static const DWORD DEFAULT_COLOR_FONT_EDIT = DEFAULT_COLOR_WHITE;
		static const DWORD DEFAULT_COLOR_CARET = D3DCOLOR_XRGB(180, 180, 255);
		static const DWORD DEFAULT_COLOR_SELECTION = D3DCOLOR_XRGB(50, 100, 255); // 0 100 255
		static const ULONGLONG DEFAULT_CARET_TICK = 30;
		static const float EDIT_PADDING_X;
		static const float EDIT_PADDING_Y;

		JWImage* m_pBackground;
		JWFont* m_pEditFont;

		JWLine* m_pCaret;
		D3DXVECTOR2 m_CaretSize;
		D3DXVECTOR2 m_CaretPosition;
		size_t* m_pCaretSelPosition;
		size_t m_PreviousCaretSelPosition;

		JWRectangle* m_pSelection;
		size_t m_SelStart;
		size_t m_SelEnd;
		size_t* m_pCapturedSelPosition;

		WSTRING m_IMETempText;
		size_t m_IMETempSel;
		const TCHAR* m_pIMEWritingCharacter;
		const TCHAR* m_pIMECompletedCharacter;
		bool m_bIMECompleted;
		bool m_bIMEWriting;

		ULONGLONG m_CaretTickCount;
		WSTRING m_ClipText;

		bool m_bUseMultiline;
		float m_YSizeMultiline;
		float m_YSizeSingleline;
	};
};