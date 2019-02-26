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

		auto Create(const D3DXVECTOR2 Position, const D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void Draw() override;

		auto SetPosition(const D3DXVECTOR2 Position)->JWControl* override;
		auto SetSize(const D3DXVECTOR2 Size)->JWControl* override;
		auto SetText(const WSTRING Text)->JWControl* override;
		auto SetFontColor(const DWORD Color)->JWControl* override;
		auto SetWatermark(const WSTRING Text)->JWControl* override;
		auto SetWatermarkColor(const DWORD Color)->JWControl* override;

		void Focus() override;

		auto ShouldUseMultiline(const bool Value) noexcept->JWControl* override;
		auto ShouldUseAutomaticLineBreak(const bool Value) noexcept->JWControl* override;
		auto ShouldUseNumberInputsOnly(const bool Value) noexcept->JWControl* override;
		
	protected:
		// Events called in JWGUIWindow (friend class).
		void WindowMouseDown() override;
		void WindowMouseMove() override;
		void WindowKeyDown(const WPARAM VirtualKeyCode) override;
		void WindowCharKeyInput(const WPARAM Char) override;
		void WindowIMEInput(const SGUIIMEInputInfo& IMEInfo) override;

	private:
		void InsertCharacter(wchar_t Char);
		void InsertString(WSTRING String);
		void EraseCharacter(size_t SelPosition);
		void EraseSelection();
		void CopySelection();
		void PasteFromClipboard();

		void UpdatePaddedViewport();

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