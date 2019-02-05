#pragma once

#include "../CoreBase/JWCommon.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWFont;

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

		virtual void SetSharedData(const SGUISharedData* SharedData);
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

		// Draw
		virtual void Draw() {};

		// Font
		virtual void SetAlignment(EHorizontalAlignment HorizontalAlignment, EVerticalAlignment VerticalAlignment);
		virtual void SetHorizontalAlignment(EHorizontalAlignment Alignment);
		virtual void SetVerticalAlignment(EVerticalAlignment Alignment);
		virtual void SetFontAlpha(BYTE Alpha);
		virtual void SetFontXRGB(DWORD XRGB);

		// Focus
		virtual void Focus();
		virtual void KillFocus();

		// Setter
		virtual void SetState(EControlState State);
		virtual void SetPosition(D3DXVECTOR2 Position);
		virtual void SetSize(D3DXVECTOR2 Size);
		virtual void SetText(WSTRING Text);
		
		// Getter
		virtual auto GetState() const->EControlState;
		virtual auto GetPosition()->D3DXVECTOR2;
		virtual auto GetSize()->D3DXVECTOR2;
		virtual auto GetText()->WSTRING;
		virtual auto GetControlType() const->EControlType;
		virtual auto GetClientMouseDownPosition() const->POINT;

		// Property setter/getter
		virtual void ShouldDrawBorder(bool Value);
		virtual void SetButtonColor(EControlState State, DWORD Color) {}; // TextButton
		virtual void SetCheckState(bool Value) {}; // CheckBox / RadioBox
		virtual auto GetCheckState() const->bool { return true; }; // CheckBox / RadioBox
		virtual void SetScrollRange(size_t Max) {}; // ScrollBar
		virtual void SetScrollPosition(size_t Position) {}; // ScrollBar
		virtual void SetUseMultiline(bool Value) {}; // Edit

	protected:
		virtual void CalculateRECT();
		virtual void UpdateText();

	protected:
		static const SGUISharedData* ms_pSharedData;

		JWFont* m_pFont;

		D3DXVECTOR2 m_PositionClient;
		D3DXVECTOR2 m_Size;
		RECT m_Rect;

		EControlType m_Type;
		EControlState m_ControlState;
		WSTRING m_Text;

		bool m_bShouldDrawBorder;
		bool m_bHasFocus;

		// Mouse
		POINT m_MousePosition;
		SMouseData m_UpdatedMousedata;
	};
};