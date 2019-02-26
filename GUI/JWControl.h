#pragma once

#include "../CoreBase/JWCommon.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWGUIWindow;
	class JWWindow;
	class JWLine;
	class JWScrollBar;

	enum class EHorizontalAlignment;
	enum class EVerticalAlignment;
	// ***

	enum EControlType
	{
		NotDefined,
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

	class JWControl
	{
	friend class JWTextButton;
	friend class JWGUIWindow;

	public:
		JWControl();
		virtual ~JWControl() {};

		// Create JWControl.
		virtual auto Create(const D3DXVECTOR2 Position, const D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError;

		// Destroy JWControl.
		virtual void Destroy();

		/*
		** Maker functions for sub-classes
		*/
		// [JWImageButton] Make a normal JWImageButton.
		virtual auto MakeImageButton(const WSTRING TextureAtlasFileName, const D3DXVECTOR2 ButtonSizeInTexture, const D3DXVECTOR2 NormalOffset,
			const D3DXVECTOR2 HoverOffset, const D3DXVECTOR2 PressedOffset)->JWControl* { return this; };

		// [JWImageButton] Make a system arrow JWImageButton.
		virtual auto MakeSystemArrowButton(const ESystemArrowDirection Direction)->JWControl* { return this; };

		// [JWScrollBar] Decide the direction (horizontal/vertical) of the JWScrollBar.
		virtual auto MakeScrollBar(const EScrollBarDirection Direction)->JWControl* { return this; };


		/*
		** Drawing functions
		*/
		// Set the viewport for this control.
		virtual void BeginDrawing();

		// @warning: This function is defined in sub-classes.
		virtual void Draw() {};

		// Draw control's borderline if 'ShouldDrawBorder' property is set true.
		// and also reset the original viewport.
		virtual void EndDrawing();


		/*
		** Text(font)-related functions
		*/
		virtual auto SetText(const WSTRING Text)->JWControl*;
		virtual auto GetText(WSTRING* OutPtrText)->JWControl*;
		virtual auto SetTextAlignment(const EHorizontalAlignment HorizontalAlignment, const EVerticalAlignment VerticalAlignment)->JWControl*;
		virtual auto SetTextHorizontalAlignment(const EHorizontalAlignment Alignment)->JWControl*;
		virtual auto SetTextVerticalAlignment(const EVerticalAlignment Alignment)->JWControl*;
		virtual auto SetFontColor(const DWORD Color)->JWControl*;

		// Setter
		virtual auto SetPosition(const D3DXVECTOR2 Position)->JWControl*;
		virtual auto SetSize(const D3DXVECTOR2 Size)->JWControl*;

		// Getter
		virtual auto GetPosition() const->const D3DXVECTOR2;
		virtual auto GetSize() const->const D3DXVECTOR2;
		virtual auto GetControlType() const->const EControlType;
		virtual auto GetControlState() const->const EControlState;

		/*
		** Recall event
		*/
		// Return true if JWControl's state is Hover.
		virtual auto OnMouseHover() const->bool;

		// Return true if JWControl's state is Pressed.
		virtual auto OnMousePressed() const->bool;

		// Return true if JWControl's state is Clicked.
		virtual auto OnMouseCliked() const->bool;

		// [JWListBox]
		// Return THandleItem value of the selected sub-item in the JWListBox.
		// @warning: calling this function initilaizes the clicked subitem index.
		virtual auto OnSubItemClick()->THandleItem { return THandleItem_Null; };


		// Attach a JWScrollBar to this control.
		// @warning: if pScrollBar is not a pointer to a JWScrollBar, this function won't work.
		virtual auto AttachScrollBar(const JWControl* pScrollBar)->JWControl*;

		// Detach the formerly attached JWScrollBar.
		virtual auto DetachScrollBar()->JWControl*;

		/*
		** Property setter/getter
		** Property setter functions return 'this' pointer.
		*/
		virtual auto ShouldDrawBorder(const bool Value)->JWControl*;
		virtual auto ShouldUseViewport(const bool Value)->JWControl*;

		// [JWImageBox]
		virtual auto SetTextureAtlas(const LPDIRECT3DTEXTURE9 pTextureAtlas, const D3DXIMAGE_INFO* pTextureAtlasInfo)->JWControl* { return this; };

		// [JWImageBox]
		virtual auto SetAtlasUV(const D3DXVECTOR2 OffsetInAtlas, const D3DXVECTOR2 Size)->JWControl* { return this; };

		// [JWCheckBox] | [JWRadioBox]
		virtual auto SetCheckState(const bool Value)->JWControl* { return this; };

		// [JWCheckBox] | [JWRadioBox]
		virtual auto GetCheckState() const->const bool { return true; };
		
		// [JWScrollBar]
		virtual auto SetScrollRange(const size_t VisibleUnitCount, const size_t TotalUnitCount)->JWControl* { return this; };

		// [JWScrollBar]
		virtual auto SetScrollPosition(const size_t Position)->JWControl* { return this; };

		// [JWScrollBar]
		virtual auto GetScrollRange() const->size_t { return 0; };

		// [JWScrollBar]
		virtual auto GetScrollPosition() const->size_t { return 0; };

		// [JWEdit]
		virtual auto ShouldUseMultiline(const bool Value)->JWControl* { return this; };

		// [JWEdit]
		virtual auto ShouldUseAutomaticLineBreak(const bool Value)->JWControl* { return this; };

		// [JWEdit]
		virtual auto SetWatermark(const WSTRING Text)->JWControl* { return this; };

		// [JWEdit]
		virtual auto SetWatermarkColor(const DWORD Color)->JWControl* { return this; };

		// [JWListBox]
		virtual auto ShouldUseAutomaticScrollBar(const bool Value)->JWControl* { return this; };

		// [JWListBox] / [JWTextButton]
		virtual auto ShouldUseToggleSelection(const bool Value)->JWControl* { return this; };

		// [JWListBox]
		virtual auto ShouldUseImageItem(const bool Value)->JWControl* { return this; };

		// [JWListBox]
		virtual auto SetImageItemTextureAtlas(const LPDIRECT3DTEXTURE9 pTexture, const D3DXIMAGE_INFO* pInfo)->JWControl* { return this; };

		// [JWListBox]
		virtual auto AddListBoxItem(const WSTRING Text, const D3DXVECTOR2 OffsetInAtlas = D3DXVECTOR2(0, 0),
			const D3DXVECTOR2 SizeInAtlas = D3DXVECTOR2(0, 0))->JWControl* { return this; };

		// [JWListBox]
		virtual auto SetMinimumItemHeight(const float Value)->JWControl* { return this; };

		// [JWListBox]
		virtual auto GetListBoxItemCount() const->const size_t { return 0; };

		// [JWListBox]
		virtual auto GetListBoxItemHeight() const->const float { return 0; };

		// [JWListBox]
		virtual auto GetSelectedItemIndex() const->const TIndex { return TIndex_NotSpecified; };

		// [JWMenuBar]
		// @warning: this functions doesn't return 'this' pointer,
		// but the handle(THandleItem) of the added item.
		virtual auto AddMenuBarItem(const WSTRING Text)->THandleItem { return THandleItem_Null; };

		// [JWMenuBar]
		// @warning: this functons doesn't return 'this' pointer,
		// but the handle(THandleItem) of the added subitem.
		virtual auto AddMenuBarSubItem(const THandleItem hItem, const WSTRING Text)->THandleItem { return THandleItem_Null; };

	protected:
		// Calculate RECT of this control.
		virtual void CalculateControlRect();
		

		/*
		** Updater function
		*/
		// Update control's state (called in JWGUIWindow - friend class).
		virtual void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus);
		virtual void UpdateBorderPositionAndSize();
		virtual void UpdateViewport();


		/*
		** Setter functions.
		*/
		virtual void SetControlState(const EControlState State);
		virtual void SetControlStateColor(const EControlState State, const DWORD Color);
		virtual void SetBorderColor(const DWORD Color);
		virtual void SetBorderColor(const DWORD ColorA, const DWORD ColorB);
		virtual void SetBackgroundColor(const DWORD Color);


		/*
		** Focus-related functions
		*/
		// Set focus on this control.
		virtual void Focus();

		// Kill the focus of this control.
		virtual void KillFocus();

		// Event functions called in JWGUIWindow (friend class).
		virtual void WindowMouseDown() {};
		virtual void WindowMouseMove() {};
		virtual void WindowKeyDown(const WPARAM VirtualKeyCode) {};
		virtual void WindowKeyUp(const WPARAM VirtualKeyCode) {};
		virtual void WindowCharKeyInput(const WPARAM Char) {};
		virtual void WindowIMEInput(const SGUIIMEInputInfo& IMEInfo) {};

	protected:
		const SGUIWindowSharedData* m_pSharedData;

		D3DVIEWPORT9 m_OriginalViewport;
		D3DVIEWPORT9 m_ControlViewport;

		JWLine* m_pBorderLine;
		JWScrollBar* m_pAttachedScrollBar;

		DWORD m_Color_Normal;
		DWORD m_Color_Hover;
		DWORD m_Color_Pressed;

		EControlType m_ControlType;
		EControlState m_ControlState;
		RECT m_ControlRect;
		D3DXVECTOR2 m_Position;
		D3DXVECTOR2 m_Size;

		DWORD m_FontColor;
		WSTRING m_Text;
		EHorizontalAlignment m_HorizontalAlignment;
		EVerticalAlignment m_VerticalAlignment;
		D3DXVECTOR2 m_CalculatedTextPosition;

		bool m_bShouldDrawBorder;
		bool m_bShouldUseViewport;
		bool m_bHasFocus;
	};
};