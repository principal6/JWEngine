#pragma once

#include "../CoreBase/JWCommon.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWFont;
	// ***

	enum EControlType
	{
		Button,
	};

	enum EControlState
	{
		Normal,
		Hover,
		Pressed,
		Clicked,
	};

	static inline auto IsMouseInRECT(const POINT& Position, const RECT& Rect)->bool
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

		virtual void Update(const SMouseData& MouseData);
		virtual void Draw() {};

		virtual void SetPosition(D3DXVECTOR2 Position);
		virtual void SetSize(D3DXVECTOR2 Size);
		virtual void SetText(WSTRING Text, DWORD FontColor);

		virtual auto GetPosition()->D3DXVECTOR2;
		virtual auto GetSize()->D3DXVECTOR2;
		virtual auto GetText()->WSTRING;

		virtual void ShouldDrawBorder(bool Value);

		virtual auto GetState()->EControlState const;

	protected:
		virtual void CalculateRECT();

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

		EControlState m_State;
		WSTRING m_Text;
		DWORD m_FontColor;

		bool m_bShouldDrawBorder;
	};
};