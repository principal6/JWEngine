#include "JWFont.h"
#include "JWImage.h"
#include "JWWindow.h"

using namespace JWENGINE;

JWFont::JWFont()
{
	m_pDevice = nullptr;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pTexture = nullptr;

	// Set default alignment
	m_HorizontalAlignment = EHorizontalAlignment::Left;
	m_VerticalAlignment = EVerticalAlignment::Bottom;

	ClearString();
}

void JWFont::ClearString()
{
	m_StringLines.clear();

	m_ImageStringLength = 0;
	m_bIsStringLineFirstChar = true;
	m_ImageStringXAdvance = 0;
	m_ImageStringYAdvance = 0;
}

void JWFont::ClearVertexAndIndexData()
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

	// Create box
	m_pBox = new JWImage;
	m_pBox->Create(m_pJWWindow, m_BaseDir);
	m_pBox->SetXRGB(DEFAULT_BG_COLOR);

	ClearVertexAndIndexData();

	return EError::OK;
}

void JWFont::Destroy()
{
	m_pDevice = nullptr;

	ClearVertexAndIndexData();
	ClearString();

	JW_DESTROY(m_pBox);

	JW_RELEASE(m_pTexture);
	JW_RELEASE(m_pIndexBuffer);
	JW_RELEASE(m_pVertexBuffer);
}

auto JWFont::MakeFont(WSTRING FileName_FNT)->EError
{
	WSTRING NewFileName;
	NewFileName = m_BaseDir;
	NewFileName += ASSET_DIR;
	NewFileName += FileName_FNT;

	if (Parse(NewFileName))
	{
		// MakeOutter rectangles with max size (MAX_TEXT_LEN)
		for (UINT i = 0; i < MAX_TEXT_LEN; i++)
		{
			m_Vertices.push_back(SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 0));
			m_Vertices.push_back(SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 0));
			m_Vertices.push_back(SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 1));
			m_Vertices.push_back(SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 1));
			m_Indices.push_back(SIndex3(i * 4, i * 4 + 1, i * 4 + 3));
			m_Indices.push_back(SIndex3(i * 4, i * 4 + 3, i * 4 + 2));
		}

		CreateVertexBuffer();
		CreateIndexBuffer();
		UpdateVertexBuffer();
		UpdateIndexBuffer();

		// JWFont will always use only one page in the BMFont, whose ID is '0'
		if (JW_FAILED(CreateTexture(m_FontData.Pages[0].File)))
			return EError::TEXTURE_NOT_CREATED;

		return EError::OK;
	}

	return EError::FONT_NOT_CREATED;
}

auto JWFont::CreateTexture(WSTRING FileName)->EError
{
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = nullptr;
	}

	WSTRING NewFileName;
	NewFileName = m_BaseDir;
	NewFileName += ASSET_DIR;
	NewFileName += FileName;

	// Craete texture without color key
	if (FAILED(D3DXCreateTextureFromFileExW(m_pDevice, NewFileName.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &m_pTexture)))
		return EError::TEXTURE_NOT_CREATED;

	return EError::OK;
}

auto JWFont::CreateVertexBuffer()->EError
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

auto JWFont::CreateIndexBuffer()->EError
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

auto JWFont::UpdateVertexBuffer()->EError
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

auto JWFont::UpdateIndexBuffer()->EError
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

auto JWFont::SetText(WSTRING MultilineText)->EError
{
	if (!MultilineText.size())
		return EError::NULL_STRING;

	if (MultilineText.length() > MAX_TEXT_LEN)
		return EError::BUFFER_NOT_ENOUGH;

	ClearString();
	
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

	int XPositionOffset = 0;
	int YPositionOffset = 0;
	m_ImageStringYAdvance = 0;

	switch (m_VerticalAlignment)
	{
	case JWENGINE::EVerticalAlignment::Top:
		break;
	case JWENGINE::EVerticalAlignment::Middle:
		YPositionOffset = static_cast<int>(m_pBox->GetSize().y / 2.0f
			- static_cast<float>(m_FontData.Info.Size * m_StringLines.size()) / 2.0f);
		break;
	case JWENGINE::EVerticalAlignment::Bottom:
		YPositionOffset = static_cast<int>(m_pBox->GetSize().y
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
			XPositionOffset = static_cast<int>(m_pBox->GetSize().x / 2.0f
				- (static_cast<float>(GetLineImageLength(m_StringLines[iterator])) / 2.0f));
			break;
		case JWENGINE::EHorizontalAlignment::Right:
			XPositionOffset = static_cast<int>(m_pBox->GetSize().x
				- static_cast<float>(GetLineImageLength(m_StringLines[iterator])));
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
			AddChar(CharID, CharIDPrev, m_StringLines[iterator][i], XPositionOffset, YPositionOffset);

			CharIDPrev = CharID;
		}

		// New line setting
		m_bIsStringLineFirstChar = true;
		m_ImageStringXAdvance = 0;
		m_ImageStringYAdvance += m_FontData.Info.Size;
	}

	UpdateVertexBuffer();

	//m_MultilineString;
	return EError::OK;
}

auto JWFont::GetLineImageLength(WSTRING LineText)->size_t
{
	if (!LineText.length())
		return 0;

	size_t Result = 0;
	wchar_t currentChar = 0;
	wchar_t CharID = 0;
	wchar_t CharIDPrev = 0;
	int Kerning = 0;

	for(size_t iterator = 0; iterator < LineText.length(); iterator++)
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

void JWFont::AddChar(wchar_t CharID, wchar_t CharIDPrev, wchar_t Character, int XOffsetBase, int YOffsetBase)
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

	m_Vertices[vertexID] = SVertexImage(x1, y1, m_Vertices[vertexID].color, u1, v1);
	m_Vertices[vertexID + 1] = SVertexImage(x2, y1, m_Vertices[vertexID + 1].color, u2, v1);
	m_Vertices[vertexID + 2] = SVertexImage(x1, y2, m_Vertices[vertexID + 2].color, u1, v2);
	m_Vertices[vertexID + 3] = SVertexImage(x2, y2, m_Vertices[vertexID + 3].color, u2, v2);

	m_ImageStringLength++;
}

auto JWFont::SetPosition(D3DXVECTOR2 Offset)->EError
{
	// Set box position
	m_pBox->SetPosition(Offset);

	if (m_Vertices.size())
	{
		// Set position as offset
		for (SVertexImage& iterator : m_Vertices)
		{
			iterator.x = iterator.x - m_PositionOffset.x;
			iterator.y = iterator.y - m_PositionOffset.y;
		}

		m_PositionOffset = Offset;

		for (SVertexImage& iterator : m_Vertices)
		{
			iterator.x = iterator.x + m_PositionOffset.x;
			iterator.y = iterator.y + m_PositionOffset.y;
		}

		UpdateVertexBuffer();
		return EError::OK;
	}
	return EError::NULL_VERTEX;
}

auto JWFont::SetFontAlpha(BYTE Alpha)->EError
{
	if (m_Vertices.size())
	{
		for (SVertexImage& iterator : m_Vertices)
		{
			SetColorAlpha(&iterator.color, Alpha);
		}

		UpdateVertexBuffer();
		return EError::OK;
	}
	return EError::NULL_VERTEX;
}

auto JWFont::SetFontXRGB(DWORD Color)->EError
{
	if (m_Vertices.size())
	{
		for (SVertexImage& iterator : m_Vertices)
		{
			SetColorXRGB(&iterator.color, Color);
		}

		UpdateVertexBuffer();
		return EError::OK;
	}
	return EError::NULL_VERTEX;
}

auto JWFont::SetBackgroundAlpha(BYTE Alpha)->EError
{
	if (m_pBox)
	{
		m_pBox->SetAlpha(Alpha);
		return EError::OK;
	}

	return EError::NULLPTR_IMAGE;
}

auto JWFont::SetBackgroundXRGB(DWORD Color)->EError
{
	if (m_pBox)
	{
		m_pBox->SetXRGB(Color);
		return EError::OK;
	}

	return EError::NULLPTR_IMAGE;
}

void JWFont::SetHorizontalAlignment(EHorizontalAlignment Alignment)
{
	m_HorizontalAlignment = Alignment;
}

void JWFont::SetVerticalAlignment(EVerticalAlignment Alignment)
{
	m_VerticalAlignment = Alignment;
}

void JWFont::SetBoxSize(D3DXVECTOR2 Size)
{
	// Set box size
	m_pBox->SetSize(Size);
}

void JWFont::Draw() const
{
	// Draw box
	m_pBox->Draw();

	// Set alpha blending on
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	if (m_pTexture)
	{
		// Texture exists
		m_pDevice->SetTexture(0, m_pTexture);

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
}