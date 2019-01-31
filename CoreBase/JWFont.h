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
		auto SetText(WSTRING MultilineText, D3DXVECTOR2 Position, D3DXVECTOR2 BoxSize)->EError;
		auto GetCharIndexInLine(LONG XPosition, const WSTRING& LineText) const->size_t;
		auto GetCharXPositionInLine(size_t CharIndex, const WSTRING& LineText) const->float;
		auto GetCharYPosition(size_t Chars_index, size_t LineIndex) const->float;
		auto GetLineYPosition(size_t LineIndex) const->float;
		auto GetLineLength(const WSTRING& LineText)->float;
		auto GetLineHeight() const->float;
		
		// Draw
		void Draw() const;

	private:
		auto CreateMaxVertexIndexForFont()->EError;
		auto CreateVertexBuffer()->EError;
		auto CreateIndexBuffer()->EError;
		auto UpdateVertexBuffer()->EError;
		auto UpdateIndexBuffer()->EError;

		auto CreateTexture(WSTRING FileName)->EError;

		void AddChar(size_t CharIndexInLine, WSTRING& LineText, size_t LineIndex, size_t Chars_index, size_t Chars_index_prev,
			float HorizontalAlignmentOffset, float VerticalAlignmentOffset, D3DXVECTOR2 Position, D3DXVECTOR2 BoxSize);

	private:
		static const DWORD DEFAULT_COLOR_FONT = D3DCOLOR_XRGB(255, 255, 255);
		static const DWORD DEFAULT_COLOR_BOX = D3DCOLOR_ARGB(0, 180, 180, 180);

		static LPDIRECT3DTEXTURE9 ms_pTexture;

		JWWindow* m_pJWWindow;
		JWImage* m_pBox;
		WSTRING m_BaseDir;

		LPDIRECT3DDEVICE9 m_pDevice;
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
		LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;

		SVertexImage* m_Vertices;
		SIndex3* m_Indices;

		D3DXVECTOR2 m_PositionOffset;

		EHorizontalAlignment m_HorizontalAlignment;
		EVerticalAlignment m_VerticalAlignment;

		WSTRING m_StringText;
		size_t m_ImageStringLength;

		DWORD m_FontColor;
		DWORD m_BoxColor;
	};
};