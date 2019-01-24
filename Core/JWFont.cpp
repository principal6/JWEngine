#include "../CoreBase/JWWindow.h"
#include "JWFont.h"

using namespace JWENGINE;

JWFont::JWFont()
{
	m_CurrFontInstanceID = 0;
	m_FontColor = 0xFF000000;
}

auto JWFont::Create(JWWindow* pJWWindow)->EError
{
	if (pJWWindow == nullptr)
		return EError::NULLPTR_WINDOW;

	m_pJWWindow = pJWWindow;

	return EError::OK;
}

void JWFont::Destroy()
{
	m_pJWWindow = nullptr;

	for (SFontInstance& iterator : m_Fonts)
	{
		if (iterator.pFont)
		{
			iterator.pFont->Release();
			iterator.pFont = nullptr;
		}
	}
}

void JWFont::MakeFont(EFontID ID, WSTRING FontName, int FontSize, bool IsBold)
{
	UINT Weight = FW_NORMAL;
	if (IsBold)
		Weight = FW_BOLD;

	LPD3DXFONT tpFont;
	D3DXCreateFont(m_pJWWindow->GetDevice(), FontSize, 0, Weight, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_DONTCARE, FontName.c_str(), &tpFont);

	m_Fonts.emplace_back(ID, tpFont);
}

auto JWFont::SelectFontByID(EFontID ID)->JWFont*
{
	int iterator_n = 0;
	for (SFontInstance& iterator : m_Fonts)
	{
		if (iterator.ID == ID)
		{
			m_CurrFontInstanceID = iterator_n;
			return this;
		}
		iterator_n++;
	}

	return nullptr;
}

auto JWFont::SetFontColor(DWORD Color)->JWFont*
{
	m_FontColor = Color;
	return this;
}

auto JWFont::Draw(int X, int Y, WSTRING String)->JWFont*
{
	RECT Rect_Font;
	SetRect(&Rect_Font, X, Y, m_pJWWindow->GetWindowData()->WindowWidth, m_pJWWindow->GetWindowData()->WindowHeight);
	m_Fonts[m_CurrFontInstanceID].pFont->DrawText(nullptr, String.c_str(), -1, &Rect_Font, DT_LEFT|DT_NOCLIP, m_FontColor);
	return this;
}