#include "../CoreBase/JWCommon.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	// ***

	enum class EFontID
	{
		Title,
		Subtitle,
		Text,
		Debug,
	};

	struct SFontInstance
	{
		EFontID ID;
		LPD3DXFONT pFont;

		SFontInstance() : pFont(nullptr) {};
		SFontInstance(EFontID _ID, LPD3DXFONT _pFont) : ID(_ID), pFont(_pFont) {};
	};

	class JWFont final
	{
	public:
		JWFont();
		~JWFont() {};

		// Set parent JWWindow
		auto JWFont::Create(JWWindow* pJWWindow)->EError;

		// Release all the instances
		void JWFont::Destroy();

		// Make a font
		void JWFont::MakeFont(EFontID ID, WSTRING FontName, int FontSize, bool IsBold);

		// Select a font
		auto JWFont::SelectFontByID(EFontID ID)->JWFont*;

		// Set font color
		auto JWFont::SetFontColor(DWORD Color)->JWFont*;

		// Draw the text
		auto JWFont::Draw(int X, int Y, WSTRING String)->JWFont*;

	private:
		
		// Pointer to the parent JWWindow
		JWWindow* m_pJWWindow;

		// Font instances
		VECTOR<SFontInstance> m_Fonts;

		// Current font instance ID
		int m_CurrFontInstanceID;

		// Current font color
		DWORD m_FontColor;
	};
};