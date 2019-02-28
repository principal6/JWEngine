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

		SGlyphInfo() : chars_id(1), left(0), top(0), drawing_top(0), width(0), height(0), line_index(0), glyph_index_in_line(SIZE_T_INVALID) {};
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
		void CreateInstantText(const JWWindow* pJWWindow, const WSTRING* pBaseDir);

		// Create non-instant-text JWText.
		// A 'JWEdit' control must call this function when it's being created.
		// To fill in the third paramater(pFontTexture), call GetFontTexturePtr() of the JWGUIWindow-shared JWText object.
		void CreateNonInstantText(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const LPDIRECT3DTEXTURE9 pFontTexture);

		// Destroy JWText object, no matter it's instant or non-instant.
		void Destroy() noexcept;

		// Call this function when first set the text, or when the JWEdit control is resized or repositioned.
		void SetNonInstantText(WSTRING Text, const D3DXVECTOR2& Position, const D3DXVECTOR2& AreaSize) noexcept;

		// @warning: only used for IME input in order to erase temporary IME character input.
		void SetNonInstantTextString(WSTRING Text) noexcept;

		void SetNonInstantTextColor(const DWORD FontColor) noexcept;

		// Insert a character in non-instant-text at the caret position.
		void InsertInNonInstantText(const WSTRING& String) noexcept;

		void DrawNonInstantText() const noexcept;

		// Draw insant text to the window.
		// Since it's instant text, there's no update function.
		// @warning: instant text must be a single-line text.
		// If the text is multi-line, it will be clipped.
		void DrawInstantText(WSTRING SingleLineText, const D3DXVECTOR2& Position,
			const EHorizontalAlignment HorizontalAlignment = EHorizontalAlignment::Left, const DWORD FontColor = DEFAULT_COLOR_FONT) noexcept;

		void DrawCaret() const noexcept;

		void DrawSelectionBox() const noexcept;

		// Get pointer to the font texture.
		// This function needs to be used when you call CreateNonInstantText().
		auto GetFontTexturePtr() const noexcept->const LPDIRECT3DTEXTURE9;

		// Every line's height is equal to the font's size (ms_FontData.Info.Size).
		auto GetLineHeight() const noexcept->const float;

		void MoveCaretToLeft() noexcept;
		void MoveCaretToRight() noexcept;
		void MoveCaretUp() noexcept;
		void MoveCaretDown() noexcept;
		void MoveCaretHome() noexcept;
		void MoveCaretEnd() noexcept;
		void MoveCaretTo(const size_t SelPosition) noexcept;

		void SelectToLeft() noexcept;
		void SelectToRight() noexcept;
		void SelectUp() noexcept;
		void SelectDown() noexcept;
		void SelectHome() noexcept;
		void SelectEnd() noexcept;
		void SelectTo(const size_t SelPosition) noexcept;
		void SelectAll() noexcept;
		void ReleaseSelection() noexcept;

		auto GetCaretSelPosition() const noexcept->const size_t;
		auto GetMousePressedSelPosition(const POINT MousePosition) const noexcept->const size_t;
		auto GetSelectionStart() const noexcept->const size_t;
		auto GetSelectionEnd() const noexcept->const size_t;

		auto IsTextSelected() const noexcept->const bool;

		void ShouldUseAutomaticLineBreak(const bool Value) noexcept;

	private:
		// @warning: the font texture must be created only once per JWGUIWindow (i.e. per D3D device).
		void CreateFontTexture(const WSTRING& FileName_FNT);

		// Create vertex and index buffers for instant text with limited length (MAX_INSTANT_TEXT_LENGTH).
		// @warning: this function is called in CreateFontTexture().
		void CreateInstantTextBuffers();

		// Create vertex and index buffers for non-instant text with limited length
		// (the length is computed to be the maximum, in accordance with screen's size and font's size.)
		void CreateNonInstantTextBuffers();

		void CreateVertexBuffer(SVertexData* pVertexData);
		void CreateIndexBuffer(SIndexData* pIndexData);
		void UpdateVertexBuffer(SVertexData* pVertexData);
		void UpdateIndexBuffer(SIndexData* pIndexData);

		void UpdateNonInstantTextVisibleVertices() noexcept;
		void UpdateCaret() noexcept;
		void UpdateSelectionBox() noexcept;

		void SetInstantTextGlyph(const size_t Character_index, SGlyphInfo* pCurrInfo, const SGlyphInfo* pPrevInfo) noexcept;
		void SetNonInstantTextGlyph(SGlyphInfo* pCurrInfo, SGlyphInfo* pPrevInfo) noexcept;

		auto GetLineWidth(const WSTRING* pLineText) const noexcept->const float;
		auto GetLineStartGlyphIndex(const size_t LineIndex) const noexcept->const size_t;
		auto GetLineEndGlyphIndex(const size_t LineIndex) const noexcept->const size_t;

	private:
		static const DWORD DEFAULT_COLOR_CARET = DEFAULT_COLOR_FONT;
		static const DWORD DEFAULT_COLOR_BOX = D3DCOLOR_ARGB(0, 180, 180, 180);
		static const DWORD DEFAULT_COLOR_SELECTION = D3DCOLOR_ARGB(100, 255, 0, 255);
		static const float DEFAULT_SIDE_CONSTRAINT_STRIDE;
		static constexpr unsigned int MAX_INSTANT_TEXT_LENGTH = 256;
		static constexpr unsigned int MAX_INSTANT_TEXT_VERTEX_SIZE = MAX_INSTANT_TEXT_LENGTH * 4;
		static constexpr unsigned int MAX_INSTANT_TEXT_INDEX_SIZE = MAX_INSTANT_TEXT_LENGTH * 2;

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