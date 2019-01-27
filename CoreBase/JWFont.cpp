#include "JWFont.h"
#include "JWImage.h"
#include "JWWindow.h"

using namespace JWENGINE;

LPDIRECT3DTEXTURE9 JWFont::ms_pTexture;

JWFont::JWFont()
{
	m_pDevice = nullptr;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;

	// Set default alignment
	m_HorizontalAlignment = EHorizontalAlignment::Left;
	m_VerticalAlignment = EVerticalAlignment::Top;

	// Set default colors
	m_FontColor = DEFAULT_COLOR_FONT;
	m_BoxColor = DEFAULT_COLOR_BOX;

	ClearString();
}

PRIVATE void JWFont::ClearString()
{
	m_EntireString.clear();
	m_StringLines.clear();

	m_ImageStringLength = 0;
	m_bIsStringLineFirstChar = true;
	m_ImageStringXAdvance = 0;
	m_ImageStringYAdvance = 0;
}

PRIVATE void JWFont::ClearVertexAndIndexData()
{
	m_Vertices.clear();
	m_Indices.clear();
}

auto JWFont::Create(JWWindow* pJWWindow, WSTRING BaseDir)->EError
{
	if (pJWWindow == nullptr)
		return EError::NULLPTR_WINDOW;

	m_pJWWindow = pJWWindow;
	m_pDevice = pJWWindow->GetDevice();
	m_BaseDir = BaseDir;

	ClearVertexAndIndexData();

	return EError::OK;
}

void JWFont::Destroy()
{
	m_pDevice = nullptr;

	ClearVertexAndIndexData();
	ClearText();

	JW_RELEASE(ms_pTexture);
	JW_RELEASE(m_pIndexBuffer);
	JW_RELEASE(m_pVertexBuffer);
}

auto JWFont::MakeFont(WSTRING FileName_FNT)->EError
{
	WSTRING NewFileName;
	NewFileName = m_BaseDir;
	NewFileName += ASSET_DIR;
	NewFileName += FileName_FNT;

	if (m_FontData.bFontCreated)
	{
		// Font already exists, no need to parse again
		return CreateMaxVertexIndexForFont();
	}
	else
	{
		// Font not yet created, then create one
		if (Parse(NewFileName))
		{
			if (JW_SUCCEEDED(CreateMaxVertexIndexForFont()))
			{
				// JWFont will always use only one page in the BMFont, whose ID is '0'
				if (JW_FAILED(CreateTexture(m_FontData.Pages[0].File)))
					return EError::TEXTURE_NOT_CREATED;
			}
		}
	}
	
	return EError::FONT_NOT_CREATED;
}

PRIVATE auto JWFont::CreateMaxVertexIndexForFont()->EError
{
	// MakeOutter rectangles with max size (MAX_TEXT_LEN)
	for (UINT i = 0; i < MAX_TEXT_LEN; i++)
	{
		m_Vertices.emplace_back(SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 0));
		m_Vertices.emplace_back(SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 0));
		m_Vertices.emplace_back(SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 1));
		m_Vertices.emplace_back(SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 1));
		m_Indices.emplace_back(SIndex3(i * 4, i * 4 + 1, i * 4 + 3));
		m_Indices.emplace_back(SIndex3(i * 4, i * 4 + 3, i * 4 + 2));
	}

	CreateVertexBuffer();
	CreateIndexBuffer();
	UpdateVertexBuffer();
	UpdateIndexBuffer();

	m_FontData.bFontCreated = true;

	return EError::OK;
}

PRIVATE auto JWFont::CreateVertexBuffer()->EError
{
	if (m_Vertices.size())
	{
		int rVertSize = sizeof(SVertexImage) * static_cast<int>(m_Vertices.size());
		if (FAILED(m_pDevice->CreateVertexBuffer(rVertSize, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVertexBuffer, nullptr)))
		{
			return EError::VERTEX_BUFFER_NOT_CREATED;
		}
		return EError::OK;
	}
	return EError::NULL_VERTEX;
}

PRIVATE auto JWFont::CreateIndexBuffer()->EError
{
	if (m_Indices.size())
	{
		int rIndSize = sizeof(SIndex3) * static_cast<int>(m_Indices.size());
		if (FAILED(m_pDevice->CreateIndexBuffer(rIndSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, nullptr)))
		{
			return EError::INDEX_BUFFER_NOT_CREATED;
		}
		return EError::OK;
	}
	return EError::NULL_INDEX;
}

PRIVATE auto JWFont::UpdateVertexBuffer()->EError
{
	if (m_Vertices.size())
	{
		int rVertSize = sizeof(SVertexImage) * static_cast<int>(m_Vertices.size());
		VOID* pVertices;
		if (FAILED(m_pVertexBuffer->Lock(0, rVertSize, (void**)&pVertices, 0)))
		{
			return EError::VERTEX_BUFFER_NOT_LOCKED;
		}
		memcpy(pVertices, &m_Vertices[0], rVertSize);
		m_pVertexBuffer->Unlock();
		return EError::OK;
	}
	return EError::NULL_VERTEX;
}

PRIVATE auto JWFont::UpdateIndexBuffer()->EError
{
	if (m_Indices.size())
	{
		int rIndSize = sizeof(SIndex3) * static_cast<int>(m_Indices.size());
		VOID* pIndices;
		if (FAILED(m_pIndexBuffer->Lock(0, rIndSize, (void **)&pIndices, 0)))
		{
			return EError::INDEX_BUFFER_NOT_LOCKED;
		}
		memcpy(pIndices, &m_Indices[0], rIndSize);
		m_pIndexBuffer->Unlock();
		return EError::OK;
	}
	return EError::NULL_INDEX;
}

PRIVATE auto JWFont::CreateTexture(WSTRING FileName)->EError
{
	if (ms_pTexture)
	{
		ms_pTexture->Release();
		ms_pTexture = nullptr;
	}

	WSTRING NewFileName;
	NewFileName = m_BaseDir;
	NewFileName += ASSET_DIR;
	NewFileName += FileName;

	// Craete texture without color key
	if (FAILED(D3DXCreateTextureFromFileExW(m_pDevice, NewFileName.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &ms_pTexture)))
		return EError::TEXTURE_NOT_CREATED;

	return EError::OK;
}

void JWFont::SetAlignment(EHorizontalAlignment HorizontalAlignment, EVerticalAlignment VerticalAlignment)
{
	SetHorizontalAlignment(HorizontalAlignment);
	SetVerticalAlignment(VerticalAlignment);
}

void JWFont::SetHorizontalAlignment(EHorizontalAlignment Alignment)
{
	m_HorizontalAlignment = Alignment;
}

void JWFont::SetVerticalAlignment(EVerticalAlignment Alignment)
{
	m_VerticalAlignment = Alignment;
}

void JWFont::SetFontAlpha(BYTE Alpha)
{
	SetColorAlpha(&m_FontColor, Alpha);
}

void JWFont::SetFontXRGB(DWORD XRGB)
{
	SetColorXRGB(&m_FontColor, XRGB);
}

void JWFont::SetBoxAlpha(BYTE Alpha)
{
	SetColorAlpha(&m_BoxColor, Alpha);
}

void JWFont::SetBoxXRGB(DWORD XRGB)
{
	SetColorXRGB(&m_BoxColor, XRGB);
}

void JWFont::ClearText()
{
	// Clear image string
	ClearString();

	// Clear boxes
	if (m_pBox.size())
	{
		for (JWImage* iterator : m_pBox)
		{
			JW_DESTROY(iterator);
		}
		m_pBox.clear();
	}
}

auto JWFont::AddText(WSTRING MultilineText, D3DXVECTOR2 Position, D3DXVECTOR2 BoxSize)->EError
{
	if (!MultilineText.size())
		return EError::NULL_STRING;

	if (m_EntireString.length() + MultilineText.length() > MAX_TEXT_LEN)
		return EError::BUFFER_NOT_ENOUGH;

	m_EntireString += MultilineText;

	// Add new box
	m_pBox.push_back(new JWImage);
	m_pBox[m_pBox.size() - 1]->Create(m_pJWWindow, m_BaseDir);
	m_pBox[m_pBox.size() - 1]->SetAlpha(GetColorAlpha(m_BoxColor));
	m_pBox[m_pBox.size() - 1]->SetXRGB(GetColorXRGB(m_BoxColor));
	m_pBox[m_pBox.size() - 1]->SetPosition(Position);
	m_pBox[m_pBox.size() - 1]->SetSize(BoxSize);

	m_StringLines.clear();
	m_bIsStringLineFirstChar = true;
	m_ImageStringXAdvance = 0;
	m_ImageStringYAdvance = 0;

	int iterator_prev = 0;
	for (int iterator = 0; iterator <= MultilineText.length(); iterator++)
	{
		// Check new line('\n') and string end
		if ((MultilineText[iterator] == L'\n') || iterator == MultilineText.length())
		{
			m_StringLines.push_back(MultilineText.substr(iterator_prev, iterator - iterator_prev));
			iterator_prev = iterator + 1;
		}
	}

	int XPositionOffset = static_cast<int>(Position.x);
	int YPositionOffset = static_cast<int>(Position.y);
	m_ImageStringYAdvance = 0;

	switch (m_VerticalAlignment)
	{
	case JWENGINE::EVerticalAlignment::Top:
		break;
	case JWENGINE::EVerticalAlignment::Middle:
		YPositionOffset += static_cast<int>(m_pBox[m_pBox.size() - 1]->GetSize().y / 2.0f
			- static_cast<float>(m_FontData.Info.Size * m_StringLines.size()) / 2.0f);
		break;
	case JWENGINE::EVerticalAlignment::Bottom:
		YPositionOffset += static_cast<int>(m_pBox[m_pBox.size() - 1]->GetSize().y
			- static_cast<float>(m_FontData.Info.Size * m_StringLines.size()));
		break;
	default:
		break;
	}

	for (int iterator = 0; iterator < m_StringLines.size(); iterator++)
	{
		switch (m_HorizontalAlignment)
		{
		case JWENGINE::EHorizontalAlignment::Left:
			break;
		case JWENGINE::EHorizontalAlignment::Center:
			XPositionOffset += static_cast<int>(m_pBox[m_pBox.size() - 1]->GetSize().x / 2.0f
				- (static_cast<float>(GetImageStringLineLength(m_StringLines[iterator])) / 2.0f));
			break;
		case JWENGINE::EHorizontalAlignment::Right:
			XPositionOffset += static_cast<int>(m_pBox[m_pBox.size() - 1]->GetSize().x
				- static_cast<float>(GetImageStringLineLength(m_StringLines[iterator])));
			break;
		default:
			break;
		}

		wchar_t CharID = 0;
		wchar_t CharIDPrev = 0;
		int Kerning = 0;

		// MakeOutter text images from the text string
		for (size_t i = 0; i < m_StringLines[iterator].length(); i++)
		{
			// Find corresponding wchar_t item in m_FontData.CharMap (std::map structure)
			CharID = static_cast<wchar_t>(m_FontData.CharMap.find(m_StringLines[iterator][i])->second);
			Kerning = 0;

			if (i)
			{
				// Use kerning since 2nd letter
				auto iterator_kerning = m_FontData.KerningMap.find(std::make_pair(CharIDPrev, CharID));

				// Set kerning value only if the key exists
				if (iterator_kerning != m_FontData.KerningMap.end())
				{
					Kerning = iterator_kerning->second;
					m_ImageStringXAdvance += Kerning;
				}
			}

			// Add wchar_t to the text (in vertex)
			AddChar(CharID, CharIDPrev, m_StringLines[iterator][i], XPositionOffset, YPositionOffset, m_FontColor);

			CharIDPrev = CharID;
		}

		// New line setting
		m_bIsStringLineFirstChar = true;
		m_ImageStringXAdvance = 0;
		m_ImageStringYAdvance += m_FontData.Info.Size;
	}

	UpdateVertexBuffer();

	return EError::OK;
}

STATIC auto JWFont::GetImageStringLineLength(WSTRING LineText)->size_t
{
	if (!LineText.length())
		return 0;

	size_t Result = 0;
	wchar_t currentChar = 0;
	wchar_t CharID = 0;
	wchar_t CharIDPrev = 0;
	int Kerning = 0;

	for (size_t iterator = 0; iterator < LineText.length(); iterator++)
	{
		currentChar = LineText[iterator];
		CharID = static_cast<wchar_t>(m_FontData.CharMap.find(currentChar)->second);
		Result += m_FontData.Chars[CharID].XOffset;

		if (iterator)
		{
			// Sum previous letter's x advance since 2nd letter
			Result += m_FontData.Chars[CharIDPrev].XAdvance;

			// Use kerning since 2nd letter
			auto iterator_kerning = m_FontData.KerningMap.find(std::make_pair(CharIDPrev, CharID));

			// Set kerning value only if the key exists
			if (iterator_kerning != m_FontData.KerningMap.end())
			{
				Kerning = iterator_kerning->second;
				Result += Kerning;
			}

			if (iterator == LineText.length() - 1)
			{
				// If it's the last letter, and XAdvance for the last time
				Result += m_FontData.Chars[CharID].XAdvance;
			}
		}

		CharIDPrev = CharID;
	}

	return Result;
}

PRIVATE void JWFont::AddChar(wchar_t CharID, wchar_t CharIDPrev, wchar_t Character, int XOffsetBase, int YOffsetBase, DWORD Color)
{
	size_t vertexID = m_ImageStringLength * 4;

	float u1 = static_cast<float>(m_FontData.Chars[CharID].X) / static_cast<float>(m_FontData.Common.ScaleW);
	float v1 = static_cast<float>(m_FontData.Chars[CharID].Y) / static_cast<float>(m_FontData.Common.ScaleH);
	float u2 = u1 + static_cast<float>(m_FontData.Chars[CharID].Width) / static_cast<float>(m_FontData.Common.ScaleW);
	float v2 = v1 + static_cast<float>(m_FontData.Chars[CharID].Height) / static_cast<float>(m_FontData.Common.ScaleH);

	if (m_bIsStringLineFirstChar)
	{
		m_bIsStringLineFirstChar = false;
	}
	else
	{
		m_ImageStringXAdvance += m_FontData.Chars[CharIDPrev].XAdvance;
	}

	float x1 = static_cast<float>(XOffsetBase) + static_cast<float>(m_ImageStringXAdvance) +
		static_cast<float>(m_FontData.Chars[CharID].XOffset);
	float x2 = x1 + static_cast<float>(m_FontData.Chars[CharID].Width);

	float y1 = static_cast<float>(YOffsetBase) + static_cast<float>(m_ImageStringYAdvance) +
		static_cast<float>(m_FontData.Chars[CharID].YOffset);
	float y2 = y1 + static_cast<float>(m_FontData.Chars[CharID].Height);

	m_Vertices[vertexID] = SVertexImage(x1, y1, Color, u1, v1);
	m_Vertices[vertexID + 1] = SVertexImage(x2, y1, Color, u2, v1);
	m_Vertices[vertexID + 2] = SVertexImage(x1, y2, Color, u1, v2);
	m_Vertices[vertexID + 3] = SVertexImage(x2, y2, Color, u2, v2);

	m_ImageStringLength++;
}

void JWFont::Draw() const
{
	// Draw background box
	if (m_pBox.size())
	{
		for (JWImage* iterator : m_pBox)
		{
			iterator->Draw();
		}
	}

	// Set alpha blending on
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	if (ms_pTexture)
	{
		// Texture exists
		m_pDevice->SetTexture(0, ms_pTexture);

		// Texture alpha * Diffuse alpha
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

		// Texture color * Diffuse color
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	}

	m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SVertexImage));
	m_pDevice->SetFVF(D3DFVF_TEXTURE);
	m_pDevice->SetIndices(m_pIndexBuffer);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		static_cast<int>(m_Vertices.size()), 0, static_cast<int>(m_Indices.size()));
	
	m_pDevice->SetTexture(0, nullptr);
}