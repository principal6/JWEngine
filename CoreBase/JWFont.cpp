#include "JWFont.h"
#include "JWImage.h"
#include "JWWindow.h"

using namespace JWENGINE;

LPDIRECT3DTEXTURE9 JWFont::ms_pTexture;

JWFont::JWFont()
{
	m_pJWWindow = nullptr;
	m_pBox = nullptr;

	m_pDevice = nullptr;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;

	m_Vertices = nullptr;
	m_Indices = nullptr;

	// Set default alignment
	m_HorizontalAlignment = EHorizontalAlignment::Left;
	m_VerticalAlignment = EVerticalAlignment::Top;

	// Set default colors
	m_FontColor = DEFAULT_COLOR_FONT;
	m_BoxColor = DEFAULT_COLOR_BOX;

	ClearText();
}

void JWFont::ClearText()
{
	m_StringText.clear();

	if (m_Vertices)
	{
		for (size_t iterator = 0; iterator <= m_ImageStringLength; iterator++)
		{
			m_Vertices[iterator * 4].u = 0;
			m_Vertices[iterator * 4].v = 0;
			m_Vertices[iterator * 4 + 1].u = 0;
			m_Vertices[iterator * 4 + 1].v = 0;
			m_Vertices[iterator * 4 + 2].u = 0;
			m_Vertices[iterator * 4 + 2].v = 0;
			m_Vertices[iterator * 4 + 3].u = 0;
			m_Vertices[iterator * 4 + 3].v = 0;
		}
	}

	m_ImageStringLength = 0;
}

auto JWFont::Create(JWWindow* pJWWindow, WSTRING BaseDir)->EError
{
	if (pJWWindow == nullptr)
		return EError::NULLPTR_WINDOW;

	m_pJWWindow = pJWWindow;
	m_pDevice = pJWWindow->GetDevice();
	m_BaseDir = BaseDir;

	// Create background box
	m_pBox = new JWImage;
	m_pBox->Create(m_pJWWindow, m_BaseDir);

	return EError::OK;
}

void JWFont::Destroy()
{
	m_pDevice = nullptr;

	ClearText();

	JW_DELETE_ARRAY(m_Vertices);
	JW_DELETE_ARRAY(m_Indices);

	JW_DESTROY(m_pBox);

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

	if (ms_FontData.bFontDataParsed)
	{
		// Font already exists, no need to parse again
		return CreateMaxVertexIndexForFont();
	}
	else
	{
		// Font not yet created, then create one
		if (Parse(NewFileName))
		{
			ms_FontData.bFontDataParsed = true;

			if (JW_SUCCEEDED(CreateMaxVertexIndexForFont()))
			{
				// JWFont will always use only one page in the BMFont, whose ID is '0'
				if (JW_FAILED(CreateTexture(ms_FontData.Pages[0].File)))
					return EError::TEXTURE_NOT_CREATED;
			}
		}
	}
	
	return EError::FONT_NOT_CREATED;
}

PRIVATE auto JWFont::CreateMaxVertexIndexForFont()->EError
{
	// MakeOutter rectangles with max size (MAX_TEXT_LEN)
	if (!m_Vertices)
	{
		m_Vertices = new SVertexImage[MAX_TEXT_LEN * 4];
		memset(m_Vertices, 0, sizeof(m_Vertices));
		
		for (size_t iterator = 0; iterator < MAX_TEXT_LEN; iterator++)
		{
			m_Vertices[iterator * 4] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 0);
			m_Vertices[iterator * 4 + 1] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 0);
			m_Vertices[iterator * 4 + 2] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 1);
			m_Vertices[iterator * 4 + 3] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 1);
		}
	}

	if (!m_Indices)
	{
		m_Indices = new SIndex3[MAX_TEXT_LEN * 2];

		for (size_t iterator = 0; iterator < MAX_TEXT_LEN; iterator++)
		{
			m_Indices[iterator * 2] = SIndex3(iterator * 4, iterator * 4 + 1, iterator * 4 + 3);
			m_Indices[iterator * 2 + 1] = SIndex3(iterator * 4, iterator * 4 + 3, iterator * 4 + 2);
		}
	}

	CreateVertexBuffer();
	CreateIndexBuffer();
	UpdateVertexBuffer();
	UpdateIndexBuffer();

	return EError::OK;
}

PRIVATE auto JWFont::CreateVertexBuffer()->EError
{
	if (m_Vertices)
	{
		int rVertSize = sizeof(SVertexImage) * MAX_TEXT_LEN * 4;
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
	if (m_Indices)
	{
		int rIndSize = sizeof(SIndex3) * MAX_TEXT_LEN * 2;
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
	if (m_Vertices)
	{
		int rVertSize = sizeof(SVertexImage) * MAX_TEXT_LEN * 4;
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
	if (m_Indices)
	{
		int rIndSize = sizeof(SIndex3) * MAX_TEXT_LEN * 2;
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
	if (m_pBox)
	{
		m_pBox->SetAlpha(GetColorAlpha(m_BoxColor));
	}
}

void JWFont::SetBoxXRGB(DWORD XRGB)
{
	SetColorXRGB(&m_BoxColor, XRGB);
	if (m_pBox)
	{
		m_pBox->SetXRGB(GetColorXRGB(m_BoxColor));
	}
}

auto JWFont::SetText(WSTRING MultilineText, D3DXVECTOR2 Position, D3DXVECTOR2 BoxSize)->EError
{
	if (MultilineText.length() > MAX_TEXT_LEN)
		return EError::BUFFER_NOT_ENOUGH;

	if (!MultilineText.length())
	{
		return EError::NULL_STRING;
	}

	// Clear the text
	ClearText();

	m_StringText = MultilineText;

	// Set backgrounnd box
	m_pBox->SetPosition(Position);
	m_pBox->SetSize(BoxSize);
	m_pBox->SetAlpha(GetColorAlpha(m_BoxColor));
	m_pBox->SetXRGB(GetColorXRGB(m_BoxColor));

	// Parse MultilineText into line_string
	WSTRING line_string;

	size_t line_count = 0;
	size_t iterator_in_line_prev = 0;
	for (size_t iterator_in_line = 0; iterator_in_line <= MultilineText.length(); iterator_in_line++)
	{
		// Check new line('\n') and string end
		if ((MultilineText[iterator_in_line] == L'\n') || iterator_in_line == MultilineText.length())
		{
			// Get line string
			line_string = MultilineText.substr(iterator_in_line_prev, iterator_in_line - iterator_in_line_prev);

			/*
			// Set vertical alignment offset (y position)
			float VerticalAlignmentOffset = Position.y;
			switch (m_VerticalAlignment)
			{
			case JWENGINE::EVerticalAlignment::Top:
				break;
			case JWENGINE::EVerticalAlignment::Middle:
				VerticalAlignmentOffset += BoxSize.y / 2.0f - GetLineYPosition(line_count) / 2.0f;
				break;
			case JWENGINE::EVerticalAlignment::Bottom:
				VerticalAlignmentOffset += BoxSize.y - GetLineYPosition(line_count);
				break;
			default:
				break;
			}

			// Set horizontal alignment offset (x position)
			float HorizontalAlignmentOffset = Position.x;
			switch (m_HorizontalAlignment)
			{
			case JWENGINE::EHorizontalAlignment::Left:
				break;
			case JWENGINE::EHorizontalAlignment::Center:
				HorizontalAlignmentOffset += BoxSize.x / 2.0f - GetLineLength(line_string) / 2.0f;
				break;
			case JWENGINE::EHorizontalAlignment::Right:
				HorizontalAlignmentOffset += BoxSize.x - GetLineLength(line_string);
				break;
			default:
				break;
			}
			*/

			size_t Chars_index = 0;
			size_t Chars_index_prev = 0;

			// Make outter text images from the text string
			for (size_t iterator_char = 0; iterator_char < line_string.length(); iterator_char++)
			{
				// Get Chars_index from MappedCharacters
				Chars_index = ms_FontData.MappedCharacters[line_string[iterator_char]];
				
				AddChar(iterator_char, line_string, line_count, Chars_index, Chars_index_prev, Position.x, Position.y, Position, BoxSize);
				/*
				// Add wchar_t to the image string
				AddChar(iterator_char, line_string, line_count, Chars_index, Chars_index_prev,
					HorizontalAlignmentOffset, VerticalAlignmentOffset, Position, BoxSize);
				*/

				Chars_index_prev = Chars_index;
			}

			iterator_in_line_prev = iterator_in_line + 1;
			line_count++;
		}
	}

	UpdateVertexBuffer();

	return EError::OK;
}

auto JWFont::GetCharIndexInLine(LONG XPosition, const WSTRING& LineText) const->size_t
{
	size_t Result = 0;

	if (!LineText.length())
		return Result;

	LONG XPositionSum = 0;
	size_t Chars_index = 0;
	size_t Chars_index_prev = 0;

	for (size_t iterator = 0; iterator <= LineText.length(); iterator++)
	{
		// Get Chars_index in from MappedCharacters
		Chars_index = ms_FontData.MappedCharacters[LineText[iterator]];

		if (iterator)
		{
			XPositionSum += ms_FontData.Chars[Chars_index_prev].XAdvance;
			XPositionSum += ms_FontData.Chars[Chars_index].XOffset;
		}

		if (XPosition <= XPositionSum)
		{
			if (iterator)
				iterator--;

			Result = iterator;
			return Result;
		}

		Chars_index_prev = Chars_index;
	}

	Result = LineText.length();
	return Result;
}

auto JWFont::GetCharXPositionInLine(size_t CharIndex, const WSTRING& LineText) const->float
{
	float Result = 0;

	// If LineText is NULL, return 0
	if (!LineText.length())
		return Result;

	// If it's the first char of line, XPosition is 0
	if (CharIndex == 0)
		return Result;

	// CharIndex <= LineText.length()
	if (CharIndex > LineText.length())
	{
		CharIndex = LineText.length();
	}

	size_t Chars_index = 0;
	size_t Chars_index_prev = 0;

	for (size_t iterator = 0; iterator <= CharIndex; iterator++)
	{
		// Get Chars_index from MappedCharacters
		Chars_index = ms_FontData.MappedCharacters[LineText[iterator]];

		if (iterator)
		{
			Result += ms_FontData.Chars[Chars_index_prev].XAdvance;
			Result += ms_FontData.Chars[Chars_index].XOffset;
		}
		
		Chars_index_prev = Chars_index;
	}

	return Result;
}

auto JWFont::GetLineYPosition(size_t LineIndex) const->float
{
	return static_cast<float>(LineIndex * ms_FontData.Info.Size);
}

auto JWFont::GetCharYPosition(size_t Chars_index, size_t LineIndex) const->float
{
	return GetLineYPosition(LineIndex) + static_cast<float>(ms_FontData.Chars[Chars_index].YOffset);
}

auto JWFont::GetLineLength(const WSTRING& LineText)->float
{
	return GetCharXPositionInLine(LineText.length(), LineText);
}

auto JWFont::GetLineHeight() const->float
{
	return static_cast<float>(ms_FontData.Info.Size);
}

PRIVATE void JWFont::AddChar(size_t CharIndexInLine, WSTRING& LineText, size_t LineIndex, size_t Chars_index, size_t Chars_index_prev,
	float HorizontalAlignmentOffset, float VerticalAlignmentOffset, D3DXVECTOR2 Position, D3DXVECTOR2 BoxSize)
{
	// Set u, v values
	float u1 = static_cast<float>(ms_FontData.Chars[Chars_index].X) / static_cast<float>(ms_FontData.Common.ScaleW);
	float v1 = static_cast<float>(ms_FontData.Chars[Chars_index].Y) / static_cast<float>(ms_FontData.Common.ScaleH);
	float u2 = u1 + static_cast<float>(ms_FontData.Chars[Chars_index].Width) / static_cast<float>(ms_FontData.Common.ScaleW);
	float v2 = v1 + static_cast<float>(ms_FontData.Chars[Chars_index].Height) / static_cast<float>(ms_FontData.Common.ScaleH);

	// Set x, y positions
	float x1 = HorizontalAlignmentOffset + GetCharXPositionInLine(CharIndexInLine, LineText);
	float x2 = x1 + static_cast<float>(ms_FontData.Chars[Chars_index].Width);
	float y1 = VerticalAlignmentOffset + GetCharYPosition(Chars_index, LineIndex);
	float y2 = y1 + static_cast<float>(ms_FontData.Chars[Chars_index].Height);

	//if ((x2 <= Position.x + BoxSize.x) && (y2 <= Position.y + BoxSize.y))
	//{
		size_t vertexID = m_ImageStringLength * 4;
		m_Vertices[vertexID] = SVertexImage(x1, y1, m_FontColor, u1, v1);
		m_Vertices[vertexID + 1] = SVertexImage(x2, y1, m_FontColor, u2, v1);
		m_Vertices[vertexID + 2] = SVertexImage(x1, y2, m_FontColor, u1, v2);
		m_Vertices[vertexID + 3] = SVertexImage(x2, y2, m_FontColor, u2, v2);
	//}

	m_ImageStringLength++;
}

void JWFont::Draw() const
{
	// Draw background box
	if (m_pBox)
	{
		m_pBox->Draw();
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
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, MAX_TEXT_LEN * 4, 0, MAX_TEXT_LEN * 2);
	
	m_pDevice->SetTexture(0, nullptr);
}