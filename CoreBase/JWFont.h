#pragma once

#include "JWCommon.h"
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

		auto JWFont::Create(JWWindow* pJWWindow, WSTRING BaseDir)->EError;
		void JWFont::Destroy();

		auto JWFont::MakeFont(WSTRING FileName_FNT)->EError;

		auto JWFont::SetText(WSTRING Text)->EError;
		auto JWFont::SetPosition(D3DXVECTOR2 Offset)->EError;
		auto JWFont::SetAlpha(BYTE Alpha)->EError;
		auto JWFont::SetXRGB(DWORD Color)->EError;

		void JWFont::Draw() const;

	private:
		void JWFont::ClearString();
		void JWFont::ClearVertexAndIndexData();

		auto JWFont::CreateVertexBuffer()->EError;
		auto JWFont::CreateIndexBuffer()->EError;
		auto JWFont::CreateTexture(WSTRING FileName)->EError;

		auto JWFont::UpdateVertexBuffer()->EError;
		auto JWFont::UpdateIndexBuffer()->EError;

		void JWFont::AddChar(wchar_t CharID, wchar_t Character);

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