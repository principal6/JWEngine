#pragma once

#include "JWBMFontParser.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWLine;
	class JWRectangle;

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
		float left;
		float top;
		float drawing_top;
		float width;
		float height;
		size_t line_index;
		size_t glyph_index_in_line;

		SGlyphInfo() : chars_id(0), left(0), top(0), drawing_top(0), width(0), height(0), line_index(0), glyph_index_in_line(0) {};
		SGlyphInfo(float _left, float _top) : chars_id(0), left(_left), top(_top), drawing_top(0), width(0), height(0),
			line_index(0), glyph_index_in_line(0) {};
	};

	struct SLineInfo
	{
		size_t start_glyph_index;
		size_t end_glyph_index;

		SLineInfo() : start_glyph_index(0), end_glyph_index(0) {};
		SLineInfo(size_t start, size_t end) : start_glyph_index(start), end_glyph_index(end) {};
	};

	class JWText final : public JWBMFontParser
	{
	public:
		JWText();
		~JWText() {};

		// TODO: add SetWatermark(), SetWatermarkColor()

		// Create instant-text JWText.
		// A 'JWGUIWindow' must have one instant-text JWText for its controls.
		auto CreateInstantText(const JWWindow* pJWWindow, const WSTRING* pBaseDir)->EError;

		// Create non-instant-text JWText.
		// A 'JWEdit' control must call this function when it's being created.
		// To fill in the third paramater(pFontTexture), call GetFontTexturePtr() of the JWGUIWindow-shared JWText object.
		auto CreateNonInstantText(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const LPDIRECT3DTEXTURE9 pFontTexture)->EError;

		// Destroy JWText object, no matter it's instant or non-instant.
		void Destroy();

		void SetNonInstantTextColor(const DWORD FontColor);

		// Call this function when first set the text, or when the JWEdit control is resized or repositioned.
		void SetNonInstantText(WSTRING Text, const D3DXVECTOR2 Position, const D3DXVECTOR2 AreaSize);

		void InsertCharacterInNonInstantText(size_t SelPosition, const wchar_t Character);
		void DrawNonInstantText();

		// Draw insant text to the window.
		// Since it's instant text, there's no update function.
		// @warning: instant text must be a single-line text.
		// If the text is multi-line, it will be clipped.
		void DrawInstantText(WSTRING SingleLineText, const D3DXVECTOR2 Position,
			const EHorizontalAlignment HorizontalAlignment = EHorizontalAlignment::Left, const DWORD FontColor = DEFAULT_COLOR_FONT);

		void DrawCaret();

		void DrawSelectionBox();

		// Get pointer to the font texture.
		// This function needs to be used when you call CreateNonInstantText().
		auto GetFontTexturePtr() const-> const LPDIRECT3DTEXTURE9;

		// Every line's height is equal to the font's size (ms_FontData.Info.Size).
		auto GetLineHeight() const->float;

		void MoveCaretToLeft();
		void MoveCaretToRight();
		void MoveCaretUp();
		void MoveCaretDown();
		void MoveCaretHome();
		void MoveCaretEnd();
		void MoveCaretTo(const size_t SelPosition);

		void SelectToLeft();
		void SelectToRight();
		void SelectUp();
		void SelectDown();
		void SelectHome();
		void SelectEnd();
		void SelectTo(const size_t SelPosition);
		void SelectAll();
		void ReleaseSelection();

		auto GetCaretSelPosition() const->const size_t;
		auto GetMousePressedSelPosition(POINT MousePosition) const->const size_t;
		auto GetSelectionStart() const->const size_t;
		auto GetSelectionEnd() const->const size_t;
		auto IsTextSelected() const->const bool;

		void ShouldUseAutomaticLineBreak(bool Value);

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

		void UpdateNonInstantTextVisibleVertices();
		void UpdateCaret();
		void UpdateSelectionBox();

		void SetInstantTextGlyph(size_t Character_index, SGlyphInfo* pCurrInfo, const SGlyphInfo* pPrevInfo);
		void SetNonInstantTextGlyph(SGlyphInfo* pCurrInfo, const SGlyphInfo* pPrevInfo);

		auto GetLineWidth(const WSTRING* pLineText) const->float;
		auto GetLineStartGlyphIndex(const size_t LineIndex)->size_t;
		auto GetLineEndGlyphIndex(const size_t LineIndex)->size_t;

	private:
		static const DWORD DEFAULT_COLOR_CARET = DEFAULT_COLOR_FONT;
		static const DWORD DEFAULT_COLOR_BOX = D3DCOLOR_ARGB(0, 180, 180, 180);
		static const DWORD DEFAULT_COLOR_SELECTION = D3DCOLOR_ARGB(100, 255, 0, 255);
		static const float DEFAULT_SIDE_CONSTRAINT_STRIDE;
		static constexpr unsigned __int32 MAX_INSTANT_TEXT_LENGTH = 256;
		static constexpr unsigned __int32 MAX_INSTANT_TEXT_VERTEX_SIZE = MAX_INSTANT_TEXT_LENGTH * 4;
		static constexpr unsigned __int32 MAX_INSTANT_TEXT_INDEX_SIZE = MAX_INSTANT_TEXT_LENGTH * 2;

		bool m_bIsInstantText;
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
		
		WSTRING m_NonInstantText;
		VECTOR<SGlyphInfo> m_NonInstantTextGlyphInfo;
		VECTOR<SLineInfo> m_NonInstantTextLineInfo;
		DWORD m_NonInstantTextColor;

		D3DXVECTOR2 m_ConstraintPosition;
		D3DXVECTOR2 m_ConstraintSize;

		JWLine* m_pCaretLine;
		D3DXVECTOR2 m_CaretPosition;
		D3DXVECTOR2 m_CaretSize;
		size_t m_CaretSelPosition;

		JWRectangle* m_pSelectionBox;
		size_t m_CapturedSelPosition;
		size_t m_SelectionStart;
		size_t m_SelectionEnd;
		bool m_bIsTextSelected;

		D3DXVECTOR2 m_NonInstantTextOffset;
	};
};