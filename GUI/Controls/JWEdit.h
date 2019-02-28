#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWText;
	class JWImageBox;
	// ***

	class JWEdit final : public JWControl
	{
	public:
		JWEdit();
		~JWEdit() {};

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->JWControl* override;
		void Destroy() noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;
		auto SetText(const WSTRING& Text) noexcept->JWControl* override;
		auto SetFontColor(const DWORD Color) noexcept->JWControl* override;
		auto SetWatermark(const WSTRING& Text) noexcept->JWControl* override;
		auto SetWatermarkColor(const DWORD Color) noexcept->JWControl* override;

		void Focus() noexcept override;

		auto ShouldUseMultiline(const bool Value) noexcept->JWControl* override;
		auto ShouldUseAutomaticLineBreak(const bool Value) noexcept->JWControl* override;
		auto ShouldUseNumberInputsOnly(const bool Value) noexcept->JWControl* override;
		
	protected:
		// Events called in JWGUIWindow (friend class).
		void WindowMouseDown() noexcept override;
		void WindowMouseMove() noexcept override;
		void WindowKeyDown(const WPARAM VirtualKeyCode) noexcept override;
		void WindowCharKeyInput(const WPARAM Char) noexcept override;
		void WindowIMEInput(const SGUIIMEInputInfo& IMEInfo) noexcept override;

	private:
		void InsertCharacter(wchar_t Char) noexcept;
		void InsertString(WSTRING String) noexcept;
		void EraseCharacter(size_t SelPosition) noexcept;
		void EraseSelection() noexcept;
		void CopySelection() noexcept;
		void PasteFromClipboard() noexcept;

		void UpdatePaddedViewport() noexcept;

	private:
		static const DWORD DEFAULT_COLOR_BACKGROUND_EDIT = DEFAULT_COLOR_ALMOST_BLACK;
		static const unsigned int DEFAULT_EDIT_PADDING = 2;
		static const size_t DEFAULT_CARET_INTERVAL = 30;

		JWImageBox* m_pBackground;
		JWText* m_pEditText;
		DWORD m_FontColor;

		WSTRING m_Watermark;
		DWORD m_WatermarkColor;

		D3DVIEWPORT9 m_PaddedViewport;
		D3DXVECTOR2 m_PaddedPosition;
		D3DXVECTOR2 m_PaddedSize;

		size_t m_CaretShowInterval;

		bool m_bIMEInput;
		bool m_bIMECaretCaptured;
		size_t m_IMECapturedCaret;

		bool m_bUseMultiline;
		bool m_bShouldGetOnlyNumbers;
	};
};