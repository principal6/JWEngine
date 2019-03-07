#pragma once

#include "JWCommon.h"
#include "../Dependency/tinyxml2.h"

namespace JWENGINE
{
#ifdef USE_BMFONT_MAX_FULL_PRIVATE
	// This includes <Private use>
	static constexpr int MAX_WCHAR_INDEX = 985343;
#elif USE_BMFONT_MAX_FULL
	// This includes <Musical symbols> & <Tags>, which are quite unnecessary
	static constexpr int MAX_WCHAR_INDEX = 917631;
#elif USE_BMFONT_MAX_CHINESE_EXTENDED
	// Maximum index for CJK fonts (chinese extended)
	static constexpr int MAX_WCHAR_INDEX = 195071;
#else
	// Maximum index for normal CJK fonts (korean included)
	static constexpr int MAX_WCHAR_INDEX = 68191;
#endif

	struct BMFont
	{
		struct BMInfo;
		struct BMCommon;
		struct BMPage;
		struct BMChar;
		struct BMKerning;

		struct BMInfo
		{
			struct BMPadding;
			struct BMSpacing;

			// The order of the paddings is the following
			// Up, Right, Down, Left.
			struct BMPadding
			{
				UINT Up{};
				UINT Right{};
				UINT Down{};
				UINT Left{};
			};

			// The order of the spacings is the following
			// Horz, Vert.
			struct BMSpacing
			{
				UINT Horz{};
				UINT Vert{};
			};

			WSTRING Face; // Name of the true type font
			UINT Size{}; // Size of the true type font
			BOOL IsBold{};
			BOOL IsItalic{};
			WSTRING Charset; // Name of OEM (when not unicode)
			BOOL IsUnicode{};
			UINT StretchH{}; // Font height stretch in percentage (100 = no stretch)
			BOOL IsSmooth{};
			UINT AA{}; // Supersampling level (1 = no supersampling)
			BMPadding Padding; // Padding for each character
			BMSpacing Spacing; // Spacing for each character
			UINT bOutline{}; // Outline thickness for the characters
		};

		// Channel value
		// 0 - if the channel holds the glyph data
		// 1 - if it holds the outline
		// 2 - if it holds the glyph and the outline
		// 3 - if its set to zero, and
		// 4 - if its set to one.
		struct BMCommon
		{
			UINT LineHeight{}; // Distance in pixels between each line of text
			UINT Base{}; // Number of pixels from the absolute top of the line to the base of the characters
			UINT ScaleW{}; // Width of the texture
			UINT ScaleH{}; // Height of the texture
			UINT Pages{}; // Number of texture pages
			BOOL IsPacked{}; // True if monochrome characters have been packed into each of the texture channels

			UINT AlphaChnl{};
			UINT RedChnl{};
			UINT GreenChnl{};
			UINT BlueChnl{};
		};

		struct BMPage
		{
			UINT ID{}; // Page ID
			WSTRING File; // Texture file name
		};

		struct BMChar
		{
			UINT ID{}; // Character ID
			UINT X{}; // Left position of the character image in the texture
			UINT Y{}; // Top position of the character image in the texture
			UINT Width{}; // Width of the character image in the texture
			UINT Height{}; // Height of the character image in the texture
			INT XOffset{}; // X offset for when copying the image from the texture to the screen
			INT YOffset{}; // Y offset for when copying the image from the texture to the screen
			UINT XAdvance{}; // Position advanced after drawing the character
			UINT Page{}; // Texture page for the current character image
			UINT Chnl{}; // Texture channel (1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels)
		};

		struct BMKerning
		{
			UINT First{}; // First character ID
			UINT Second{}; // Second character ID
			INT Amount{}; // X position adjusted when drawing the second character after the first one
		};

		BMInfo Info;
		BMCommon Common;
		VECTOR<BMPage> Pages;
		VECTOR<BMChar> Chars;
		VECTOR<BMKerning> Kernings;

		std::map<wchar_t, size_t> CharMap;
		std::map<std::pair<UINT, UINT>, int> KerningMap;

		size_t MappedCharacters[MAX_WCHAR_INDEX]{};

		bool bFontDataParsed{ false };
	};

	class JWBMFontParser
	{
	public:
		JWBMFontParser() {};
		virtual ~JWBMFontParser() {};

		virtual auto Parse(const WSTRING& FileName) noexcept->bool;
		virtual auto GetFontData() const noexcept->const BMFont*;
		virtual auto GetCharsIDFromCharacter(wchar_t Character) const noexcept->size_t;

	protected:
		virtual auto ParseComma(const STRING& Data, UINT ID) noexcept->UINT;

	protected:
		static BMFont ms_FontData;
	};
};