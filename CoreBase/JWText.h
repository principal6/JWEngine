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

	struct SVertexData
	{
		LPDIRECT3DVERTEXBUFFER9 pBuffer;
		SVertexImage* Vertices;
		UINT VertexSize;

		SVertexData() : pBuffer(nullptr), Vertices(nullptr), VertexSize(0) {};
	};

	struct SIndexData
	{
		LPDIRECT3DINDEXBUFFER9 pBuffer;
		SIndex3* Indices;
		UINT IndexSize;

		SIndexData() : pBuffer(nullptr), Indices(nullptr), IndexSize(0) {};
	};

	struct SGlyphInfo
	{
		size_t chars_id;
		float x;
		float y;
		float y_for_texture;
		float width;
		float height;
		size_t line_index;

		SGlyphInfo() : chars_id(0), x(0), y(0), y_for_texture(0), width(0), height(0), line_index(0) {};
	};

	class JWText final : public JWBMFontParser
	{
	public:
		JWText();
		~JWText() {};

		// Create instant-text JWText.
		// A 'JWGUIWindow' must have one instant-text JWText for its controls.
		auto CreateInstantText(const JWWindow* pJWWindow, const WSTRING* pBaseDir)->EError;

		// Create non-instant-text JWText.
		// A 'JWEdit' control must call this function when it's being created.
		// To fill in the third paramater(pFontTexture), call GetFontTexturePtr() of the JWGUIWindow-shared JWText object.
		auto CreateNonInstantText(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const LPDIRECT3DTEXTURE9 pFontTexture)->EError;

		// Destroy JWText object, no matter it's instant or non-instant.
		void Destroy();

		void UpdateNonInstantText(WSTRING Text, const D3DXVECTOR2 Position, const D3DXVECTOR2 AreaSize);

		// TODO: SetWatermark(), SetWatermarkColor()
		void DrawNonInstantText();

		// Draw insant text to the window.
		// Since it's instant text, there's no update function.
		// @warning: instant text must be a single-line text.
		// If the text is multi-line, it will be clipped.
		void DrawInstantText(WSTRING SingleLineText, const D3DXVECTOR2 Position,
			const EHorizontalAlignment HorizontalAlignment = EHorizontalAlignment::Left);

		// Get pointer to the font texture.
		// This function needs to be used when you call CreateNonInstantText().
		auto GetFontTexturePtr() const-> const LPDIRECT3DTEXTURE9;

		// Every line's height is equal to the font's size (ms_FontData.Info.Size).
		auto GetLineHeight() const->float;

	private:
		// @warning: the font texture must be created only once per JWGUIWindow (i.e. per D3D device).
		auto CreateFontTexture(const WSTRING FileName_FNT)->EError;

		// Create vertex and index buffers for instant text with limited length (MAX_INSTANT_TEXT_LENGTH).
		// @warning: this function is called in CreateFontTexture().
		auto CreateInstantTextBuffers()->EError;

		// Create vertex and index buffers for non-instant text with limited length
		// (the length is computed to be the maximum, in accordance with screen's size and font's size.)
		auto CreateNonInstantTextBuffers()->EError;

		auto CreateVertexBuffer(SVertexData* pVertexData)->EError;
		auto CreateIndexBuffer(SIndexData* pIndexData)->EError;
		auto UpdateVertexBuffer(SVertexData* pVertexData)->EError;
		auto UpdateIndexBuffer(SIndexData* pIndexData)->EError;

		void SetInstantTextGlyph(size_t Character_index, SGlyphInfo* pCurrInfo, const SGlyphInfo* pPrevInfo);

		auto GetLineWidth(const WSTRING* pLineText) const->float;

	private:
		static const DWORD DEFAULT_COLOR_FONT = D3DCOLOR_XRGB(255, 255, 255);
		static const DWORD DEFAULT_COLOR_BOX = D3DCOLOR_ARGB(0, 180, 180, 180);
		static const float DEFAULT_SINGLE_LINE_STRIDE;
		static constexpr unsigned __int32 MAX_INSTANT_TEXT_LENGTH = 256;
		static constexpr unsigned __int32 MAX_INSTANT_TEXT_VERTEX_SIZE = MAX_INSTANT_TEXT_LENGTH * 4;
		static constexpr unsigned __int32 MAX_INSTANT_TEXT_INDEX_SIZE = MAX_INSTANT_TEXT_LENGTH * 2;

		bool m_bIsInstantText;
		bool m_bUseMultiline;
		bool m_bUseAutomaticLineBreak;

		const JWWindow* m_pJWWindow;
		const WSTRING* m_pBaseDir;

		// There must be one device per each window (Texture must be created on each device).
		LPDIRECT3DDEVICE9 m_pDevice;

		// Font texture, which must be shared in all JWText objects to save memory space.
		LPDIRECT3DTEXTURE9 m_pFontTexture;

		/*
		** Member variables for instant-text.
		*/
		SVertexData m_InstantVertexData;
		SIndexData m_InstantIndexData;

		/*
		** Member variables for non-instant text.
		*/
		SVertexData m_NonInstantVertexData;
		SIndexData m_NonInstantIndexData;
		
		VECTOR<SGlyphInfo> m_NonInstantTextInfo;

		bool m_bShowCaret;
		size_t m_CaretSelPosition;
		D3DXVECTOR2 m_CaretPosition;

		D3DXVECTOR2 m_NonInstantTextOffset;
	};
};