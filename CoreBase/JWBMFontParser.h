#pragma once

#include "JWCommon.h"
#include "../Dependency/tinyxml2.h"

namespace JWENGINE
{
	class JWBMFontParser
	{
	public:
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

				struct BMPadding
				{
					// The order of the paddings is the following
					UINT Up;
					UINT Right;
					UINT Down;
					UINT Left;
				};

				struct BMSpacing
				{
					// The order of the spacings is the following
					UINT Horz;
					UINT Vert;
				};

				WSTRING Face; // Name of the true type font
				UINT Size; // Size of the true type font
				BOOL IsBold;
				BOOL IsItalic;
				WSTRING Charset; // Name of OEM (when not unicode)
				BOOL IsUnicode;
				UINT StretchH; // Font height stretch in percentage (100 = no stretch)
				BOOL IsSmooth;
				UINT AA; // Supersampling level (1 = no supersampling)
				BMPadding Padding; // Padding for each character
				BMSpacing Spacing; // Spacing for each character
				UINT bOutline; // Outline thickness for the characters
			};

			struct BMCommon
			{
				UINT LineHeight; // Distance in pixels between each line of text
				UINT Base; // Number of pixels from the absolute top of the line to the base of the characters
				UINT ScaleW; // Width of the texture
				UINT ScaleH; // Height of the texture
				UINT Pages; // Number of texture pages
				BOOL IsPacked; // True if monochrome characters have been packed into each of the texture channels
				// Channel value
				// 0 - if the channel holds the glyph data
				// 1 - if it holds the outline
				// 2 - if it holds the glyph and the outline
				// 3 - if its set to zero, and
				// 4 - if its set to one.
				UINT AlphaChnl;
				UINT RedChnl;
				UINT GreenChnl;
				UINT BlueChnl;
			};

			struct BMPage
			{
				UINT ID; // Page ID
				WSTRING File; // Texture file name
			};

			struct BMChar
			{
				UINT ID; // Character ID
				UINT X; // Left position of the character image in the texture
				UINT Y; // Top position of the character image in the texture
				UINT Width; // Width of the character image in the texture
				UINT Height; // Height of the character image in the texture
				INT XOffset; // X offset for when copying the image from the texture to the screen
				INT YOffset; // Y offset for when copying the image from the texture to the screen
				UINT XAdvance; // Position advanced after drawing the character
				UINT Page; // Texture page for the current character image
				UINT Chnl; // Texture channel (1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels)
			};

			struct BMKerning
			{
				UINT First; // First character ID
				UINT Second; // Second character ID
				INT Amount; // X position adjusted when drawing the second character after the first one
			};

			BMInfo Info;
			BMCommon Common;
			VECTOR<BMPage> Pages;
			VECTOR<BMChar> Chars;
			VECTOR<BMKerning> Kernings;
		};

	protected:
		static BMFont m_FontData;

	protected:
		virtual auto ParseComma(STRING Data, UINT ID)->UINT const;

	public:
		JWBMFontParser() {};
		virtual ~JWBMFontParser() {};

		virtual auto Parse(WSTRING FileName)->bool;
		virtual auto GetFontData()->const BMFont* const;
	};
};