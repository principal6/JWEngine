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
	friend class JWGUIWindow;

	public:
		JWControl();
		virtual ~JWControl() {};

		// Create JWControl.
		virtual auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError;

		// Destroy JWControl.
		virtual void Destroy();

		/*
		** Maker functions for sub-classes
		*/
		// [JWImageButton] Make a normal JWImageButton.
		virtual void MakeImageButton(WSTRING TextureAtlasFileName, D3DXVECTOR2 ButtonSizeInTexture, D3DXVECTOR2 NormalOffset,
			D3DXVECTOR2 HoverOffset, D3DXVECTOR2 PressedOffset) {};

		// [JWImageButton] Make a system arrow JWImageButton.
		virtual void MakeSystemArrowButton(ESystemArrowDirection Direction) {};

		// [JWScrollBar] Decide the direction (horizontal/vertical) of the JWScrollBar.
		virtual void MakeScrollBar(EScrollBarDirection Direction) {};

		/*
		** Recall event
		*/
		// [JWListBox] Return THandleItem value of the selected sub-item in the JWListBox.
		// @warning: calling this function initilaizes the clicked subitem index.
		virtual auto OnSubItemClick()->THandleItem { return THandle_Null; };

		// Return true if JWControl's state is Hover.
		virtual auto OnMouseHover() const->bool;

		// Return true if JWControl's state is Pressed.
		virtual auto OnMousePressed() const->bool;

		// Return true if JWControl's state is Clicked.
		virtual auto OnMouseCliked() const->bool;

		// Update control's state.
		virtual void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus);

		// Attach JWScrollBar to this control.
		virtual void AttachScrollBar(JWControl* pScrollBar);

		// Detach the formerly attached JWScrollBar.
		virtual void DetachScrollBar();

		/*
		** Draw functions
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
		virtual void SetText(WSTRING Text);
		virtual void GetText(WSTRING* OutPtrText);
		virtual void SetTextAlignment(EHorizontalAlignment HorizontalAlignment, EVerticalAlignment VerticalAlignment);
		virtual void SetTextHorizontalAlignment(EHorizontalAlignment Alignment);
		virtual void SetTextVerticalAlignment(EVerticalAlignment Alignment);
		virtual void SetFontColor(DWORD Color);

		// Focus
		virtual void Focus();
		virtual void KillFocus();

		// Setter
		virtual void SetState(EControlState State);
		virtual void SetStateColor(EControlState State, DWORD Color);
		virtual void SetPosition(D3DXVECTOR2 Position);
		virtual void SetSize(D3DXVECTOR2 Size);
		virtual void SetBorderColor(DWORD Color);
		virtual void SetBorderColor(DWORD ColorA, DWORD ColorB);
		virtual void SetBackgroundColor(DWORD Color);

		// Getter
		virtual auto GetState() const->EControlState;
		virtual auto GetPosition()->D3DXVECTOR2;
		virtual auto GetSize()->D3DXVECTOR2;
		virtual auto GetControlType() const->EControlType;

		/*
		** Property setter/getter
		** Property setter functions return 'this' pointer.
		*/
		virtual auto ShouldDrawBorder(bool Value)->JWControl*;
		virtual auto ShouldUseViewport(bool Value)->JWControl*;

		// [JWImageBox]
		virtual auto SetTextureAtlas(LPDIRECT3DTEXTURE9 pTextureAtlas, D3DXIMAGE_INFO* pTextureAtlasInfo)->JWControl* { return this; };

		// [JWImageBox]
		virtual auto SetAtlasUV(D3DXVECTOR2 OffsetInAtlas, D3DXVECTOR2 Size)->JWControl* { return this; };

		// [JWCheckBox] | [JWRadioBox]
		virtual void SetCheckState(bool Value) {};

		// [JWCheckBox] | [JWRadioBox]
		virtual auto GetCheckState() const->bool { return true; };
		
		// [JWScrollBar]
		virtual void SetScrollRange(size_t VisibleUnitCount, size_t TotalUnitCount) {};

		// [JWScrollBar]
		virtual void SetScrollPosition(size_t Position) {};

		// [JWScrollBar]
		virtual auto GetScrollRange() const->size_t { return 0; };

		// [JWScrollBar]
		virtual auto GetScrollPosition() const->size_t { return 0; };

		// [JWEdit]
		virtual void SetUseMultiline(bool Value) {};

		// [JWListBox]
		virtual void AddListBoxItem(WSTRING Text, D3DXVECTOR2 OffsetInAtlas = D3DXVECTOR2(0, 0), D3DXVECTOR2 SizeInAtlas = D3DXVECTOR2(0, 0)) {};
		
		// [JWListBox]
		virtual void SetMinimumItemHeight(float Value) {};

		// [JWListBox]
		virtual auto GetListBoxItemCount() const->const size_t { return 0; };

		// [JWListBox]
		virtual auto GetListBoxItemHeight() const->const float { return 0; };

		// [JWListBox]
		virtual auto GetSelectedItemIndex() const->const TIndex { return TIndex_NotSpecified; };

		// [JWListBox]
		virtual void ShouldUseAutomaticScrollBar(bool Value) {}; 

		// [JWListBox] / [JWTextButton]
		virtual void ShouldUseToggleSelection(bool Value) {};

		// [JWListBox]
		virtual void ShouldUseImageItem(LPDIRECT3DTEXTURE9 pTexture, D3DXIMAGE_INFO* pInfo) {};

		// [JWMenuBar]
		// @warning: this functions doesn't return 'this' pointer,
		// but the handle(THandleItem) of the added item.
		virtual auto AddMenuBarItem(WSTRING Text)->THandleItem { return THandle_Null; };

		// [JWMenuBar]
		// @warning: this functons doesn't return 'this' pointer,
		// but the handle(THandleItem) of the added subitem.
		virtual auto AddMenuBarSubItem(THandleItem hItem, WSTRING Text)->THandleItem { return THandle_Null; };

	protected:
		virtual void CalculateControlRect();
		
		virtual void UpdateBorderPositionAndSize();
		virtual void UpdateViewport();

		// Event functions called in JWGUIWindow (friend class).
		virtual void WindowMouseDown() {};
		virtual void WindowMouseMove() {};
		virtual void WindowKeyDown(WPARAM VirtualKeyCode) {};
		virtual void WindowCharKey(WPARAM Char) {};
		virtual void WindowKeyUp(WPARAM VirtualKeyCode) {};
		virtual void WindowIMEInput(SGUIIMEInputInfo& IMEInfo) {};

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

		WSTRING m_Text;
		EHorizontalAlignment m_HorizontalAlignment;
		EVerticalAlignment m_VerticalAlignment;
		D3DXVECTOR2 m_CalculatedTextPosition;

		bool m_bShouldDrawBorder;
		bool m_bShouldUseViewport;
		bool m_bHasFocus;
	};
};