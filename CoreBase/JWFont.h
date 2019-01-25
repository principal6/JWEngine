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

		auto SetSize(D3DXVECTOR2 Size)->EError;
		auto SetPosition(D3DXVECTOR2 Offset)->EError;
		auto SetFontAlpha(BYTE Alpha)->EError;
		auto SetFontXRGB(DWORD Color)->EError;
		auto SetBackgroundAlpha(BYTE Alpha)->EError;
		auto SetBackgroundXRGB(DWORD Color)->EError;
		void SetHorizontalAlignment(EHorizontalAlignment Alignment);
		void SetVerticalAlignment(EVerticalAlignment Alignment);
		auto SetText(WSTRING MultilineText)->EError;
		
		void Draw() const;

	private:
		static auto GetImageStringLineLength(WSTRING LineText)->size_t;

		void ClearString();
		void ClearVertexAndIndexData();

		auto CreateVertexBuffer()->EError;
		auto CreateIndexBuffer()->EError;
		auto UpdateVertexBuffer()->EError;
		auto UpdateIndexBuffer()->EError;

		auto CreateTexture(WSTRING FileName)->EError;

		void AddChar(wchar_t CharID, wchar_t CharIDPrev, wchar_t Character, int XOffsetBase = 0, int YOffsetBase = 0);

	private:
		static const DWORD DEFAULT_COLOR_FONT = D3DCOLOR_XRGB(255, 255, 255);
		static const DWORD DEFAULT_BG_COLOR = D3DCOLOR_XRGB(180, 180, 180);

		JWWindow* m_pJWWindow;
		JWImage* m_pBox;
		WSTRING m_BaseDir;

		LPDIRECT3DDEVICE9 m_pDevice;
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
		LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
		LPDIRECT3DTEXTURE9 m_pTexture;

		VECTOR<SVertexImage> m_Vertices;
		VECTOR<SIndex3> m_Indices;

		D3DXVECTOR2 m_PositionOffset;

		EHorizontalAlignment m_HorizontalAlignment;
		EVerticalAlignment m_VerticalAlignment;

		VECTOR<WSTRING> m_StringLines;
		size_t m_ImageStringLength;
		size_t m_ImageStringXAdvance;
		size_t m_ImageStringYAdvance;
		bool m_bIsStringLineFirstChar;
	};
};