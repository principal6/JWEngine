#include "JWButton.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWFont.h"

using namespace JWENGINE;

auto JWButton::Create(JWWindow* pWindow, WSTRING BaseDir)->EError
{
	JWControl::Create(pWindow, BaseDir);

	m_pImage = new JWImage;
	m_pImage->Create(ms_pWindow, ms_BaseDir);

	m_pFont = new JWFont;
	m_pFont->Create(ms_pWindow, ms_BaseDir);
	m_pFont->MakeFont(DEFAULT_FONT);
}

void JWButton::Destroy()
{
	JW_DESTROY(m_pImage);
	JW_DESTROY(m_pFont);
}
