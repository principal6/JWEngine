#pragma once

#include "JWBMFontParser.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	struct SVertexImage;
	struct SIndex3;
	// ***

	class JWFont final : public JWBMFontParser
	{
	public:
		JWFont();
		~JWFont() {};

		auto Create(JWWindow* pJWWindow, WSTRING BaseDir)->EError;
		void Destroy();

		auto MakeFont(WSTRING FileName_FNT)->EError;

		auto SetText(WSTRING Text)->EError;
		auto SetPosition(D3DXVECTOR2 Offset)->EError;
		auto SetAlpha(BYTE Alpha)->EError;
		auto SetXRGB(DWORD Color)->EError;

		void Draw() const;

	private:
		void ClearString();
		void ClearVertexAndIndexData();

		auto CreateVertexBuffer()->EError;
		auto CreateIndexBuffer()->EError;
		auto CreateTexture(WSTRING FileName)->EError;

		auto UpdateVertexBuffer()->EError;
		auto UpdateIndexBuffer()->EError;

		void AddChar(wchar_t CharID, wchar_t Character);

	private:
		JWWindow* m_pJWWindow;
		WSTRING m_BaseDir;

		LPDIRECT3DDEVICE9 m_pDevice;
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
		LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
		LPDIRECT3DTEXTURE9 m_pTexture;

		std::vector<SVertexImage> m_Vertices;
		std::vector<SIndex3> m_Indices;

		D3DXVECTOR2 m_PositionOffset;

		wchar_t m_String[MAX_LINE_LEN];
		size_t m_StringLen;

		wchar_t m_StringCharID[MAX_LINE_LEN];
		size_t m_StringCharLen;
		size_t m_StringCharAdvance;
	};
};