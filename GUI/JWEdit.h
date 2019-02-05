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

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void Draw() override;

		void Focus() override;

		void SetSize(D3DXVECTOR2 Size) override;
		void SetPosition(D3DXVECTOR2 Position) override;
		void SetUseMultiline(bool Value) override;

		void OnKeyDown(WPARAM VirtualKeyCode) override;
		void OnCharKey(WPARAM Char) override;
		void OnMouseDown(LPARAM MousePosition) override;
		void OnMouseMove(LPARAM MousePosition) override;
		void CheckIMEInput() override;

	private:
		void SelectOrMoveCaretToLeft(size_t Stride = 1); // Select characters to the left or move the caret to the left
		void SelectOrMoveCaretToRight(size_t Stride = 1); // Select characters to the right or move the caret to the right
		void MoveCaretToLeft(size_t Stride = 1); // Move the caret to the left
		void MoveCaretToRight(size_t Stride = 1); // Move the caret to the right

		void UpdateBorderline();
		void UpdateViewport();

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
		static const BYTE DEFUALT_ALPHA_BACKGROUND_EDIT = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND_EDIT = DEFAULT_COLOR_NORMAL;
		static const DWORD DEFAULT_COLOR_FONT_EDIT = DEFAULT_COLOR_BLACK;
		static const DWORD DEFAULT_COLOR_SELECTION = D3DCOLOR_XRGB(50, 100, 255); // 0 100 255
		static const ULONGLONG DEFAULT_CARET_TICK = 30;

		JWLine* m_pBorderLine;

		D3DVIEWPORT9 m_OriginalViewport;
		D3DVIEWPORT9 m_EditViewport;

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
		const TCHAR* m_pIMECharacter;
		bool m_bIMECompleted;

		ULONGLONG m_CaretTickCount;
		WSTRING m_ClipText;

		bool m_bUseMultiline;
		float m_YSizeMultiline;
		float m_YSizeSingleline;
	};
};