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
	struct SWindowData;
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

	struct STextInfo
	{
		wchar_t Character;
		float Left;
		float Right;
		float Top;
		float Bottom;
		float LineTop;
		float U1;
		float U2;
		float V1;
		float V2;
		size_t LineIndex;
		size_t CharIndexInLine;
		size_t AdjustedCharIndex;

		STextInfo() : Character(0), Left(0), Right(0), Top(0), Bottom(0), LineTop(0), U1(0), U2(0), V1(0), V2(0),
			LineIndex(0), CharIndexInLine(0), AdjustedCharIndex(0) {};
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

		// Font color
		void SetFontAlpha(BYTE Alpha);
		void SetFontXRGB(DWORD XRGB);

		// Box color
		void SetBoxAlpha(BYTE Alpha);
		void SetBoxXRGB(DWORD XRGB);

		// Text
		void ClearText();
		auto SetText(WSTRING MultilineText, D3DXVECTOR2 Position, D3DXVECTOR2 BoxSize)->EError;
		auto GetTextLength() const->size_t;
		void SetUseMultiline(bool Value);
		auto GetUseMultiline() const->bool;
		auto GetCharIndexByMousePosition(POINT Position) const->size_t;
		auto GetCharXPosition(size_t CharIndex) const->float;
		
		// GetCharXPositionInBox() limits the character's x position as below
		// [box.position.x <= value <= box.position.x + box.size.x]
		auto GetCharXPositionInBox(size_t CharIndex) const->float;

		auto GetCharYPosition(size_t CharIndex) const->float;

		// GetCharYPositionInBox() limits the character's y position as below
		// [box.position.y <= value <= box.position.y + box.size.y]
		auto GetCharYPositionInBox(size_t CharIndex) const->float;

		auto GetCharacter(size_t CharIndex) const->wchar_t;
		auto GetLineIndexByCharIndex(size_t CharIndex) const->size_t;
		auto GetLineCount() const->size_t;
		auto GetLineSelPositionByCharIndex(size_t CharIndex) const->size_t;
		auto GetLineYPosition(size_t LineIndex) const->float;
		auto GetLineYPositionByCharIndex(size_t CharIndex) const->float;
		auto GetLineLength(size_t LineIndex) const->size_t;
		auto GetLineLengthByCharIndex(size_t CharIndex) const->size_t;
		auto GetLineWidth(size_t LineIndex) const->float;
		auto GetLineWidthByCharIndex(size_t CharIndex) const->float;
		auto GetLineHeight() const->float;
		auto GetLineGlobalSelStart(size_t LineIndex) const->size_t;
		auto GetLineGlobalSelEnd(size_t LineIndex) const->size_t;
		auto GetMaximumLineCount() const->const UINT;
		void UpdateCaretPosition(size_t CaretSelPosition, D3DXVECTOR2* InOutPtrCaretPosition);

		// @warning:
		// This function converts sel position in plain text to sel position in splitted text (with '\0' line end)
		// which is needed because the line splitted text has different length than the original one!
		auto GetAdjustedSelPosition(size_t SelPosition) const->size_t;
		
		// Draw
		void Draw() const;

	private:
		auto CreateMaxVertexIndexBuffer()->EError;
		// TODO:
		//auto CreateDynamicVertexIndexBuffer()->EError;

		auto CreateVertexBuffer()->EError;
		auto CreateIndexBuffer()->EError;
		auto UpdateVertexBuffer()->EError;
		auto UpdateIndexBuffer()->EError;

		auto CreateTexture(WSTRING FileName)->EError;

		auto CalculateCharPositionTop(size_t Chars_ID, size_t LineIndex) const->float;
		auto CalculateCharPositionLeftInLine(size_t CharIndex, const WSTRING& LineText) const->float;
		auto CalculateCharPositionRightInLine(size_t CharIndex, const WSTRING& LineText) const->float;
		auto CalculateLineWidth(const WSTRING& LineText)->float;

		auto IsTextEmpty() const->bool;
		
		void UpdateLetterBoxes();
		void UpdateSinglelineXOffset(float x_difference);
		void UpdateMultilineYOffset(float y_difference);

		void AddChar(wchar_t Character, size_t CharIndexInLine, WSTRING& LineText, size_t LineIndex,
			size_t Chars_ID, size_t Chars_ID_prev, float HorizontalAlignmentOffset, float VerticalAlignmentOffset);

	private:
		static const DWORD DEFAULT_COLOR_FONT = D3DCOLOR_XRGB(255, 255, 255);
		static const DWORD DEFAULT_COLOR_BOX = D3DCOLOR_ARGB(0, 180, 180, 180);
		static const float DEFAULT_BOUNDARY_STRIDE;

		static LPDIRECT3DTEXTURE9 ms_pFontTexture;

		JWWindow* m_pJWWindow;
		JWImage* m_pBackgroundBox;
		WSTRING m_BaseDir;
		const SWindowData* m_pWindowData;

		LPDIRECT3DDEVICE9 m_pDevice;
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
		LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;

		SVertexImage* m_Vertices;
		SIndex3* m_Indices;
		UINT m_VertexSize;
		UINT m_IndexSize;
		UINT m_MaximumCharacterCountInLine;
		UINT m_MaximumLineCount;
		UINT m_MaximumLetterBoxCount;

		EHorizontalAlignment m_HorizontalAlignment;
		EVerticalAlignment m_VerticalAlignment;

		DWORD m_FontColor;
		DWORD m_BoxColor;

		D3DXVECTOR2 m_BoxPosition;
		D3DXVECTOR2 m_BoxSize;

		bool m_bUseMultiline;

		WSTRING m_ImageStringOriginalText;
		size_t m_ImageStringLength;
		size_t m_ImageStringAdjustedLength;
		size_t m_UsedLetterBoxCount;

		VECTOR<STextInfo> m_ImageStringInfo;
		VECTOR<size_t> m_LineLength;
		D3DXVECTOR2 m_CaretPosition;
		float m_SinglelineXOffset;
		float m_MultilineYOffset;
	};
};