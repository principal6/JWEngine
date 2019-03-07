#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWText;
	// ***

	class JWEdit final : public JWControl
	{
	public:
		JWEdit() {};
		~JWEdit() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;
		auto SetText(const WSTRING& Text) noexcept->JWControl* override;
		auto SetFontColor(DWORD Color) noexcept->JWControl* override;
		auto SetWatermark(const WSTRING& Text) noexcept->JWControl* override;
		auto SetWatermarkColor(DWORD Color) noexcept->JWControl* override;

		void Focus() noexcept override;

		auto ShouldUseMultiline(bool Value) noexcept->JWControl* override;
		auto ShouldUseAutomaticLineBreak(bool Value) noexcept->JWControl* override;
		auto ShouldUseNumberInputsOnly(bool Value) noexcept->JWControl* override;
		
	protected:
		// Must be overridden.
		void UpdateViewport() noexcept;

		// Events called in JWGUIWindow (friend class).
		void WindowMouseDown() noexcept override;
		void WindowMouseMove() noexcept override;
		void WindowKeyDown(WPARAM VirtualKeyCode) noexcept override;
		void WindowCharKeyInput(WPARAM Char) noexcept override;
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
		static constexpr DWORD DEFAULT_COLOR_BACKGROUND_EDIT{ DEFAULT_COLOR_ALMOST_BLACK };
		static constexpr unsigned int DEFAULT_EDIT_PADDING{ 2 };
		static constexpr size_t DEFAULT_CARET_INTERVAL{ 30 };

		UNIQUE_PTR<JWImage> m_pBackground;
		UNIQUE_PTR<JWText> m_pEditText;

		DWORD m_FontColor{ DEFAULT_COLOR_FONT };

		WSTRING m_Watermark{ DEFAULT_EDIT_WATERMARK };
		DWORD m_WatermarkColor{ DEFAULT_COLOR_WATERMARK };

		D3DVIEWPORT9 m_PaddedViewport{};
		D3DXVECTOR2 m_PaddedPosition{};
		D3DXVECTOR2 m_PaddedSize{};

		size_t m_CaretShowInterval{};

		bool m_bIMEInput{ false };
		bool m_bIMECaretCaptured{ false };
		size_t m_IMECapturedCaret{};

		bool m_bUseMultiline{ false };
		bool m_bShouldGetOnlyNumbers{ false };
	};
};