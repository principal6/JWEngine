#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWText;
	class JWImageBox;
	class JWRectangle;
	// ***

	class JWEdit final : public JWControl
	{
	public:
		JWEdit();
		~JWEdit() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void Draw() override;

		void SetText(WSTRING Text) override;
		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;

		void Focus() override;

	protected:
		// Events called in JWGUIWindow (friend class).
		void WindowKeyDown(WPARAM VirtualKeyCode) override;
		void WindowCharKeyInput(WPARAM Char) override;
		void WindowIMEInput(SGUIIMEInputInfo& IMEInfo) override;

	private:
		void InsertCharacter(wchar_t Char);
		void EraseCharacter(size_t SelPosition);

	private:
		static const DWORD DEFAULT_COLOR_BACKGROUND_EDIT = DEFAULT_COLOR_ALMOST_BLACK;
		static const unsigned int DEFAULT_EDIT_PADDING = 2;
		static const size_t DEFAULT_CARET_INTERVAL = 30;

		JWImageBox* m_pBackground;

		JWText* m_pEditText;

		D3DXVECTOR2 m_PaddedPosition;
		D3DXVECTOR2 m_PaddedSize;

		size_t m_CaretShowInterval;

		bool m_bIMEInput;
		bool m_bIMECaretCaptured;
		size_t m_IMECapturedCaret;
	};
};