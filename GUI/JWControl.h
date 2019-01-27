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
		Button,
		Label,
		Edit,
	};

	enum EControlState
	{
		Normal,
		Hover,
		Pressed,
		Clicked,
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
		JWControl() {};
		virtual ~JWControl() {};

		virtual auto Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError;
		virtual void Destroy();

		// Mouse
		virtual auto IsMouseOver(const SMouseData& MouseData)->bool;
		virtual auto IsMousePressed(const SMouseData& MouseData)->bool;

		// Update
		virtual void UpdateState(const SMouseData& MouseData);

		// Draw
		virtual void Draw();

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
		virtual auto GetState()->EControlState const;
		virtual auto GetPosition()->D3DXVECTOR2;
		virtual auto GetSize()->D3DXVECTOR2;
		virtual auto GetText()->WSTRING;

		// Property setter
		virtual void ShouldDrawBorder(bool Value);

	protected:
		virtual void CalculateRECT();
		virtual void UpdateText();

	protected:
		static const DWORD DEFAULT_COLOR_NORMAL = D3DCOLOR_XRGB(120, 120, 120);
		static const DWORD DEFAULT_COLOR_HOVER = D3DCOLOR_XRGB(180, 180, 180);
		static const DWORD DEFAULT_COLOR_PRESSED = D3DCOLOR_XRGB(80, 80, 255);
		static const DWORD DEFAULT_COLOR_BORDER = D3DCOLOR_XRGB(20, 20, 60);

		static WSTRING ms_BaseDir;
		static JWWindow* ms_pWindow;

		JWFont* m_pFont;

		D3DXVECTOR2 m_PositionClient;
		D3DXVECTOR2 m_Size;
		RECT m_Rect;

		EControlType m_Type;
		EControlState m_State;
		WSTRING m_Text;
		DWORD m_FontColor;

		bool m_bShouldDrawBorder;
		bool m_bHasFocus;
	};
};