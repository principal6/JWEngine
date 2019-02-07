#pragma once

#include "../CoreBase/JWCommon.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWFont;
	class JWLine;

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
	public:
		JWControl();
		virtual ~JWControl() {};

		// Static
		virtual void SetSharedData(const SGUISharedData* SharedData);

		// Create, destroy
		virtual auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError;
		virtual void Destroy();

		// Sub-class maker
		virtual void MakeImageButton(WSTRING TextureAtlasFileName, D3DXVECTOR2 ButtonSizeInTexture, D3DXVECTOR2 NormalOffset,
			D3DXVECTOR2 HoverOffset, D3DXVECTOR2 PressedOffset) {}; // ImageButton
		virtual void MakeSystemArrowButton(ESystemArrowDirection Direction) {}; // ImageButton
		virtual void MakeScrollBar(EScrollBarDirection Direction) {}; // ScrollBar

		// Mouse
		virtual auto IsMouseOver(const SMouseData& MouseData)->bool;
		virtual auto IsMousePressed(const SMouseData& MouseData)->bool;

		// Event
		virtual void OnKeyDown(WPARAM VirtualKeyCode) {};
		virtual void OnCharKey(WPARAM Char) {};
		virtual void OnKeyUp(WPARAM VirtualKeyCode) {};
		virtual void OnMouseMove(LPARAM MousePosition);
		virtual void OnMouseDown(LPARAM MousePosition);
		virtual void OnMouseUp(LPARAM MousePosition);
		virtual void CheckIMEInput() {};

		// Update
		virtual void UpdateControlState(const SMouseData& MouseData);

		/** Draw functions
		*/
		// BeginDrawing() sets the viewport for the control.
		virtual void BeginDrawing();

		// Draw() is defined in sub-classes.
		virtual void Draw() {};

		// EndDrawing() draws control's borderline if m_bShouldDrawBorder is set true.
		// and it resets the original viewport.
		virtual void EndDrawing();


		/** Font-related functions
		*/
		virtual void SetAlignment(EHorizontalAlignment HorizontalAlignment, EVerticalAlignment VerticalAlignment);
		virtual void SetHorizontalAlignment(EHorizontalAlignment Alignment);
		virtual void SetVerticalAlignment(EVerticalAlignment Alignment);
		virtual void SetFontColor(DWORD Color);

		// Focus
		virtual void Focus();
		virtual void KillFocus();

		// Setter
		virtual void SetState(EControlState State);
		virtual void SetStateColor(EControlState State, DWORD Color);
		virtual void SetPosition(D3DXVECTOR2 Position);
		virtual void SetSize(D3DXVECTOR2 Size);
		virtual void SetText(WSTRING Text);
		virtual void SetBorderColor(DWORD Color);
		virtual void SetBorderColor(DWORD ColorA, DWORD ColorB);
		virtual void SetBackgroundColor(DWORD Color) {};
		
		// Getter
		virtual auto GetState() const->EControlState;
		virtual auto GetPosition()->D3DXVECTOR2;
		virtual auto GetSize()->D3DXVECTOR2;
		virtual auto GetText()->WSTRING;
		virtual auto GetControlType() const->EControlType;
		virtual auto GetClientMouseDownPosition() const->POINT;

		// Property setter/getter
		virtual void ShouldDrawBorder(bool Value);
		virtual void ShouldUseViewport(bool Value);
		virtual void SetCheckState(bool Value) {}; // CheckBox / RadioBox
		virtual auto GetCheckState() const->bool { return true; }; // CheckBox / RadioBox
		virtual void SetScrollRange(size_t VisibleUnitCount, size_t TotalUnitCount) {}; // ScrollBar
		virtual void SetScrollPosition(size_t Position) {}; // ScrollBar
		virtual auto GetScrollRange() const->size_t { return 0; } // ScrollBar
		virtual auto GetScrollPosition() const->size_t { return 0; } // ScrollBar

		// SetUseMultiline() is available only for JWEdit or JWLabel control
		virtual void SetUseMultiline(bool Value) {}; // Edit, Label

		virtual void AddTextItem(WSTRING Text) {}; // ListBox
		virtual auto AddMenuBarItem(WSTRING Text)->THandleItem { return THandle_Null; }; // MenuBar

	protected:
		virtual void CalculateRECT();
		
		virtual void UpdateBorderPositionAndSize();
		virtual void UpdateText();
		virtual void UpdateViewport();

	protected:
		static const SGUISharedData* ms_pSharedData;

		D3DVIEWPORT9 m_OriginalViewport;
		D3DVIEWPORT9 m_ControlViewport;

		JWFont* m_pFont;
		JWLine* m_pBorderLine;

		DWORD m_Color_Normal;
		DWORD m_Color_Hover;
		DWORD m_Color_Pressed;

		D3DXVECTOR2 m_PositionClient;
		D3DXVECTOR2 m_Size;
		RECT m_Rect;

		EControlType m_Type;
		EControlState m_ControlState;
		WSTRING m_Text;

		bool m_bShouldDrawBorder;
		bool m_bShouldUseViewport;
		bool m_bHasFocus;

		// Mouse
		POINT m_MousePosition;
		SMouseData m_UpdatedMousedata;
	};
};