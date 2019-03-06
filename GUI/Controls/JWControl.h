#pragma once

#include "../../CoreBase/JWCommon.h"
#include "../../CoreBase/JWImage.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWLine;
	class JWScrollBar;

	enum class EHorizontalAlignment;
	enum class EVerticalAlignment;
	// ***

	enum EControlType
	{
		Invalid,

		TextButton,
		ImageButton,
		Label,
		Edit,
		CheckBox,
		RadioBox,
		ScrollBar,
		ListBox,
		MenuBar,
		ImageBox,
		Frame,
	};

	enum EControlState
	{
		Normal,
		Hover,
		Pressed,
		Clicked,
	};

	enum class ESystemArrowDirection
	{
		Left,
		Right,
		Up,
		Down,
	};

	enum class EScrollBarDirection
	{
		Horizontal,
		Vertical,
		Invalid,
	};

	static inline auto Static_IsMouseInRECT(const POINT& Position, const RECT& Rect)->bool
	{
		if ((Position.x >= Rect.left) && (Position.x <= Rect.right))
		{
			if ((Position.y >= Rect.top) && (Position.y <= Rect.bottom))
			{
				return true;
			}
		}
		return false;
	}

	static inline auto Static_IsMouseInViewPort(const POINT& Position, const D3DVIEWPORT9& ViewPort)->bool
	{
		if ((Position.x >= static_cast<long>(ViewPort.X)) && (Position.x <= static_cast<long>(ViewPort.Width + ViewPort.X)))
		{
			if ((Position.y >= static_cast<long>(ViewPort.Y)) && (Position.y <= static_cast<long>(ViewPort.Height + ViewPort.Y)))
			{
				return true;
			}
		}
		return false;
	}

	class JWControl
	{
	friend class JWFrame;
	friend class JWTextButton;
	friend class JWGUIWindow;

	public:
		JWControl() {};
		virtual ~JWControl() {};

		virtual void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData);

		/*
		** Maker functions for sub-classes
		*/
		// [JWImageButton] Make a normal JWImageButton.
		virtual auto MakeImageButton(const WSTRING& TextureAtlasFileName, const D3DXVECTOR2& ButtonSizeInTexture, const D3DXVECTOR2& NormalOffset,
			const D3DXVECTOR2& HoverOffset, const D3DXVECTOR2& PressedOffset)->JWControl* { return this; };

		// [JWImageButton] Make a system arrow JWImageButton.
		virtual auto MakeSystemArrowButton(ESystemArrowDirection Direction)->JWControl* { return this; };

		// [JWScrollBar] Decide the direction (horizontal/vertical) of the JWScrollBar.
		virtual auto MakeScrollBar(EScrollBarDirection Direction)->JWControl* { return this; };


		/*
		** Drawing functions
		*/
		// Set the viewport for this control.
		virtual void BeginDrawing() noexcept;

		// @warning: This function is defined in sub-classes.
		virtual void Draw() {};

		// Draw control's borderline if 'ShouldDrawBorder' property is set true.
		// and also reset the original viewport.
		virtual void EndDrawing();


		/*
		** Text(font)-related functions
		*/
		virtual auto SetText(const WSTRING& Text) noexcept->JWControl*;
		virtual auto GetText(WSTRING& OutText) noexcept->JWControl*;
		virtual auto SetTextAlignment(EHorizontalAlignment HorizontalAlignment, EVerticalAlignment VerticalAlignment) noexcept->JWControl*;
		virtual auto SetTextHorizontalAlignment(EHorizontalAlignment Alignment) noexcept->JWControl*;
		virtual auto SetTextVerticalAlignment(EVerticalAlignment Alignment) noexcept->JWControl*;
		virtual auto SetFontColor(DWORD Color) noexcept->JWControl*;


		/*
		** Setter functions.
		*/
		virtual auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*;
		virtual auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl*;
		virtual auto SetBorderColor(DWORD Color) noexcept->JWControl*;
		virtual auto SetBorderColor(DWORD ColorA, DWORD ColorB) noexcept->JWControl*;
		virtual auto SetBackgroundColor(DWORD Color) noexcept->JWControl*;


		/*
		** Getter functions.
		*/
		virtual auto GetPosition() const noexcept->const D3DXVECTOR2;
		virtual auto GetAbsolutePosition() const noexcept->const D3DXVECTOR2;
		virtual auto GetSize() const noexcept->const D3DXVECTOR2;
		virtual auto GetControlType() const noexcept->EControlType;
		virtual auto GetControlState() const noexcept->EControlState;


		/*
		** Parent Control
		*/
		virtual auto SetParentControl(JWControl* pParentControl) noexcept->JWControl*;
		virtual auto HasParentControl() noexcept->bool;


		/*
		** Recall event
		*/
		// Return true if JWControl's state is Hover.
		virtual auto OnMouseHover() const noexcept->bool;

		// Return true if JWControl's state is Pressed.
		virtual auto OnMousePressed() const noexcept->bool;

		// Return true if JWControl's state is Clicked.
		virtual auto OnMouseCliked() const noexcept->bool;

		
		/*
		** ScrollBar attachment
		*/
		// Attach a JWScrollBar to this control.
		// @warning: if pScrollBar is not a pointer to a JWScrollBar, this function won't work.
		virtual auto AttachScrollBar(const JWControl& ScrollBar)->JWControl*;

		// Detach the formerly attached JWScrollBar.
		virtual auto DetachScrollBar() noexcept->JWControl*;


		/*
		** Property setter/getter
		** Property setter functions return 'this' pointer.
		*/
		virtual auto ShouldDrawBorder(bool Value) noexcept->JWControl*;

		// @warning: this function is used only for debugging purpose.
		// do not use this to implement a new functionality of the control.
		virtual auto ShouldUseViewport(bool Value) noexcept->JWControl*;
		
		virtual auto ShouldBeOffsetByMenuBar(bool Value) noexcept->JWControl*;

		virtual auto ShouldBeOffsetByParent(bool Value) noexcept->JWControl*;


		/*
		** Not defined methods (they will be defined in each sub-classes.
		*/
		// [JWListBox]
		// Return THandleItem value of the selected sub-item in the JWListBox.
		// @warning: calling this function initilaizes the clicked subitem index.
		virtual auto OnSubItemClick()->THandleItem { return THandleItem_Null; };

		// [JWImageBox]
		virtual auto SetTextureAtlas(const LPDIRECT3DTEXTURE9 pTextureAtlas, const D3DXIMAGE_INFO* pTextureAtlasInfo)->JWControl* { return this; };

		// [JWImageBox]
		virtual auto SetAtlasUV(const D3DXVECTOR2& OffsetInAtlas, const D3DXVECTOR2& Size)->JWControl* { return this; };

		// [JWCheckBox] | [JWRadioBox]
		virtual auto SetCheckState(bool Value)->JWControl* { return this; };

		// [JWCheckBox] | [JWRadioBox]
		virtual auto GetCheckState() const->bool { return true; };
		
		// [JWScrollBar]
		virtual auto SetScrollRange(size_t VisibleUnitCount, size_t TotalUnitCount)->JWControl* { return this; };

		// [JWScrollBar]
		virtual auto SetScrollPosition(size_t Position)->JWControl* { return this; };

		// [JWScrollBar]
		virtual auto GetScrollRange() const->size_t { return 0; };

		// [JWScrollBar]
		virtual auto GetScrollPosition() const->size_t { return 0; };

		// [JWEdit]
		virtual auto ShouldUseMultiline(bool Value)->JWControl* { return this; };

		// [JWEdit]
		virtual auto ShouldUseAutomaticLineBreak(bool Value)->JWControl* { return this; };

		// [JWEdit]
		virtual auto ShouldUseNumberInputsOnly(bool Value)->JWControl* { return this; };

		// [JWEdit]
		virtual auto SetWatermark(const WSTRING& Text)->JWControl* { return this; };

		// [JWEdit]
		virtual auto SetWatermarkColor(DWORD Color)->JWControl* { return this; };

		// [JWListBox]
		virtual auto ShouldUseAutomaticScrollBar(bool Value)->JWControl* { return this; };

		// [JWListBox] / [JWTextButton]
		virtual auto ShouldUseToggleSelection(bool Value)->JWControl* { return this; };

		// [JWListBox]
		virtual auto ShouldUseImageItem(bool Value)->JWControl* { return this; };

		// [JWListBox]
		virtual auto SetImageItemTextureAtlas(const LPDIRECT3DTEXTURE9 pTexture, const D3DXIMAGE_INFO* pInfo)->JWControl* { return this; };

		// [JWListBox]
		virtual auto AddListBoxItem(const WSTRING& Text, const D3DXVECTOR2& OffsetInAtlas = D3DXVECTOR2(0, 0),
			const D3DXVECTOR2& SizeInAtlas = D3DXVECTOR2(0, 0))->JWControl* { return this; };

		// [JWListBox]
		virtual auto SetMinimumItemHeight(float Value)->JWControl* { return this; };

		// [JWListBox]
		virtual auto GetListBoxItemCount() const->size_t { return 0; };

		// [JWListBox]
		virtual auto GetListBoxItemHeight() const->float { return 0; };

		// [JWListBox]
		virtual auto GetSelectedItemIndex() const->TIndex { return TIndex_Invalid; };

		// [JWMenuBar]
		// @warning: this functions doesn't return 'this' pointer,
		// but the handle(THandleItem) of the added item.
		virtual auto AddMenuBarItem(const WSTRING& Text)->THandleItem { return THandleItem_Null; };

		// [JWMenuBar]
		// @warning: this functons doesn't return 'this' pointer,
		// but the handle(THandleItem) of the added subitem.
		virtual auto AddMenuBarSubItem(THandleItem hItem, const WSTRING& Text)->THandleItem { return THandleItem_Null; };

		// [JWFrame]
		virtual auto AddChildControl(JWControl& ChildControl)->JWControl* { return this; };

	protected:
		// Calculate RECT of this control.
		virtual void CalculateControlRect() noexcept;
		

		/*
		** Updater function
		*/
		// Update control's state (called in JWGUIWindow - friend class).
		virtual void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept;
		virtual void UpdateBorderPositionAndSize() noexcept;

		// UpdateViewport() must be overriden when the control has child controls.
		// JWMenuBar is an exception to this rule.
		virtual void UpdateViewport() noexcept;
		virtual void UpdateChildViewport(D3DVIEWPORT9& Viewport);

		/*
		** Setter functions.
		*/
		virtual void SetControlState(EControlState State) noexcept;
		virtual void SetControlStateColor(EControlState State, DWORD Color) noexcept;


		/*
		** Focus-related functions
		*/
		// Set focus on this control.
		virtual void Focus() noexcept;

		// Kill the focus of this control.
		virtual void KillFocus() noexcept;

		// Event functions called in JWGUIWindow (friend class).
		virtual void WindowMouseDown() {};
		virtual void WindowMouseMove() {};
		virtual void WindowKeyDown(WPARAM VirtualKeyCode) {};
		virtual void WindowKeyUp(WPARAM VirtualKeyCode) {};
		virtual void WindowCharKeyInput(WPARAM Char) {};
		virtual void WindowIMEInput(const SGUIIMEInputInfo& IMEInfo) {};

	protected:
		const SGUIWindowSharedData* m_pSharedData;

		// Owning pointer
		UNIQUE_PTR<JWLine> m_pBorderLine{};

		// Observing pointer
		JWScrollBar* m_pAttachedScrollBar{ nullptr };
		JWControl* m_pParentControl{ nullptr };

		EControlType m_ControlType{ EControlType::Invalid };
		EControlState m_ControlState{ EControlState::Normal };
		RECT m_ControlRect{ 0, 0, 0, 0 };
		D3DXVECTOR2 m_Position{ 0, 0 };
		D3DXVECTOR2 m_AbsolutePosition{ 0, 0 };
		D3DXVECTOR2 m_Size{ 0, 0 };

		D3DVIEWPORT9 m_OriginalViewport{};
		D3DVIEWPORT9 m_ControlViewport{};

		DWORD m_Color_Normal{ DEFAULT_COLOR_NORMAL };
		DWORD m_Color_Hover{ DEFAULT_COLOR_HOVER };
		DWORD m_Color_Pressed{ DEFAULT_COLOR_PRESSED };

		WSTRING m_Text{};
		DWORD m_FontColor{ DEFAULT_COLOR_FONT };
		EHorizontalAlignment m_HorizontalAlignment{ EHorizontalAlignment::Left };
		EVerticalAlignment m_VerticalAlignment{ EVerticalAlignment::Top };
		D3DXVECTOR2 m_CalculatedTextPosition{ 0, 0 };

		bool m_bShouldDrawBorder{ false };
		bool m_bShouldUseViewport{ true };
		bool m_bShouldBeOffsetByMenuBar{ true };
		bool m_bShouldBeOffsetByParent{ false };
		bool m_bHasFocus{ false };
		bool m_bBorderColorSet{ false };
	};
};