#pragma once

#include "JWBMFontParser.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWImage;
	struct SVertexImage;
	struct SIndex3;
	// ***

	enum class EHorizontalAlignment
	{
		Left,
		Center,
		Right,
	};

	enum class EVerticalAlignment
	{
		Top,
		Middle,
		Bottom,
	};

	class JWFont final : public JWBMFontParser
	{
	public:
		JWFont();
		~JWFont() {};

		auto Create(JWWindow* pJWWindow, WSTRING BaseDir)->EError;
		void Destroy();

		auto MakeFont(WSTRING FileName_FNT)->EError;

		// Alignment
		void SetAlignment(EHorizontalAlignment HorizontalAlignment, EVerticalAlignment VerticalAlignment);
		void SetHorizontalAlignment(EHorizontalAlignment Alignment);
		void SetVerticalAlignment(EVerticalAlignment Alignment);

		// Color
		void SetFontAlpha(BYTE Alpha);
		void SetFontXRGB(DWORD XRGB);
		void SetBoxAlpha(BYTE Alpha);
		void SetBoxXRGB(DWORD XRGB);

		// Text
		void ClearText();
		auto AddText(WSTRING MultilineText, D3DXVECTOR2 Position, D3DXVECTOR2 BoxSize)->EError;
		
		// Draw
		void Draw() const;

	private:
		static auto GetImageStringLineLength(WSTRING LineText)->size_t;

		void ClearString();
		void ClearVertexAndIndexData();

		auto CreateMaxVertexIndexForFont()->EError;
		auto CreateVertexBuffer()->EError;
		auto CreateIndexBuffer()->EError;
		auto UpdateVertexBuffer()->EError;
		auto UpdateIndexBuffer()->EError;

		auto CreateTexture(WSTRING FileName)->EError;

		void AddChar(wchar_t CharID, wchar_t CharIDPrev, wchar_t Character, int XOffsetBase, int YOffsetBase, DWORD Color);

	private:
		static const DWORD DEFAULT_COLOR_FONT = D3DCOLOR_XRGB(255, 255, 255);
		static const DWORD DEFAULT_COLOR_BOX = D3DCOLOR_ARGB(0, 180, 180, 180);

		static LPDIRECT3DTEXTURE9 ms_pTexture;

		JWWindow* m_pJWWindow;
		VECTOR<JWImage*> m_pBox;
		WSTRING m_BaseDir;

		LPDIRECT3DDEVICE9 m_pDevice;
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
		LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;

		VECTOR<SVertexImage> m_Vertices;
		VECTOR<SIndex3> m_Indices;

		D3DXVECTOR2 m_PositionOffset;

		EHorizontalAlignment m_HorizontalAlignment;
		EVerticalAlignment m_VerticalAlignment;

		WSTRING m_EntireString;
		VECTOR<WSTRING> m_StringLines;
		size_t m_ImageStringLength;
		size_t m_ImageStringXAdvance;
		size_t m_ImageStringYAdvance;
		bool m_bIsStringLineFirstChar;

		DWORD m_FontColor;
		DWORD m_BoxColor;
	};
};