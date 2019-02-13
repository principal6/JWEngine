#include "JWText.h"
#include "JWWindow.h"

using namespace JWENGINE;

// Static const
const float JWText::DEFAULT_SINGLE_LINE_STRIDE = 50.0f;

JWText::JWText()
{
	m_bIsInstantText = true;

	m_pJWWindow = nullptr;
	m_pBaseDir = nullptr;

	m_pDevice = nullptr;

	m_pFontTexture = nullptr;

	m_bIsInstantText = false;

	m_bShowCaret = false;
	m_CaretSelPosition = 0;
	m_CaretPosition = D3DXVECTOR2(0, 0);

	m_bUseMultiline = false;
	m_SinglelineXOffset = 0;
	m_MultilineYOffset = 0;
}

auto JWText::CreateInstantText(const JWWindow* pJWWindow, const WSTRING* pBaseDir)->EError
{
	if (m_pFontTexture)
	{
		return EError::DUPLICATE_CREATION;
	}

	if ((m_pJWWindow = pJWWindow) == nullptr)
	{
		return EError::NULLPTR_WINDOW;
	}

	m_pBaseDir = pBaseDir;
	m_pDevice = pJWWindow->GetDevice();

	// Create font texture.
	CreateFontTexture(DEFAULT_FONT);

	CreateInstantTextBuffers();

	m_bIsInstantText = true;

	return EError::OK;
}

auto JWText::CreateNonInstantText(const JWWindow* pJWWindow, const WSTRING* pBaseDir, LPDIRECT3DTEXTURE9 pFontTexture)->EError
{
	if (m_pFontTexture)
	{
		return EError::DUPLICATE_CREATION;
	}

	if ((m_pJWWindow = pJWWindow) == nullptr)
	{
		return EError::NULLPTR_WINDOW;
	}

	m_pBaseDir = pBaseDir;
	m_pDevice = pJWWindow->GetDevice();

	// Set font texture.
	m_pFontTexture = pFontTexture;

	CreateNonInstantTextBuffers();

	m_bIsInstantText = false;

	return EError::OK;
}

void JWText::Destroy()
{
	m_pDevice = nullptr;

	if (m_bIsInstantText)
	{
		JW_RELEASE(m_pFontTexture);
	}
	else
	{
		m_pFontTexture = nullptr;
	}

	JW_RELEASE(m_InstantVertexData.pBuffer);
	JW_DELETE_ARRAY(m_InstantVertexData.Vertices);

	JW_RELEASE(m_InstantIndexData.pBuffer);
	JW_DELETE_ARRAY(m_InstantIndexData.Indices);

	JW_RELEASE(m_NonInstantVertexData.pBuffer);
	JW_DELETE_ARRAY(m_NonInstantVertexData.Vertices);

	JW_RELEASE(m_NonInstantIndexData.pBuffer);
	JW_DELETE_ARRAY(m_NonInstantIndexData.Indices);
}

PRIVATE auto JWText::CreateFontTexture(const WSTRING FileName_FNT)->EError
{
	if (m_pFontTexture)
	{
		return EError::DUPLICATE_CREATION;
	}

	// Set file name to Parse FNT file.
	WSTRING new_file_name;
	new_file_name = *m_pBaseDir;
	new_file_name += ASSET_DIR;
	new_file_name += FileName_FNT;

	if (!ms_FontData.bFontDataParsed)
	{
		// Font is not yet parsed, then parse it.
		// The font data(ms_FontData) is static, so we can run this code only once per process,
		// no matter how many JWTexts or windows are made.

		if (Parse(new_file_name))
		{
			ms_FontData.bFontDataParsed = true;
		}
	}

	// Set file name to create font texture.
	new_file_name = *m_pBaseDir;
	new_file_name += ASSET_DIR;
	new_file_name += ms_FontData.Pages[0].File; // @warning: We only use 1 page (Pages[0]) for our JWText.

	// Craete texture without color key
	if (FAILED(D3DXCreateTextureFromFileExW(m_pDevice, new_file_name.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &m_pFontTexture)))
		return EError::TEXTURE_NOT_CREATED;

	return EError::OK;
}

PRIVATE auto JWText::CreateInstantTextBuffers()->EError
{
	if (m_InstantVertexData.pBuffer)
	{
		return EError::DUPLICATE_CREATION;
	}

	// Set maximum size
	m_InstantVertexData.VertexSize = MAX_INSTANT_TEXT_VERTEX_SIZE;
	m_InstantIndexData.IndexSize = MAX_INSTANT_TEXT_INDEX_SIZE;

	// Make rectangles for characters with maximum characters' count
	if (!m_InstantVertexData.Vertices)
	{
		m_InstantVertexData.Vertices = new SVertexImage[m_InstantVertexData.VertexSize];

		for (size_t iterator = 0; iterator < MAX_INSTANT_TEXT_LENGTH; iterator++)
		{
			m_InstantVertexData.Vertices[iterator * 4] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 0);
			m_InstantVertexData.Vertices[iterator * 4 + 1] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 0);
			m_InstantVertexData.Vertices[iterator * 4 + 2] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 1);
			m_InstantVertexData.Vertices[iterator * 4 + 3] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 1);
		}
	}

	if (!m_InstantIndexData.Indices)
	{
		m_InstantIndexData.Indices = new SIndex3[m_InstantIndexData.IndexSize];

		for (size_t iterator = 0; iterator < MAX_INSTANT_TEXT_LENGTH; iterator++)
		{
			m_InstantIndexData.Indices[iterator * 2] = SIndex3(iterator * 4, iterator * 4 + 1, iterator * 4 + 3);
			m_InstantIndexData.Indices[iterator * 2 + 1] = SIndex3(iterator * 4, iterator * 4 + 3, iterator * 4 + 2);
		}
	}

	CreateVertexBuffer(&m_InstantVertexData);
	CreateIndexBuffer(&m_InstantIndexData);
	UpdateVertexBuffer(&m_InstantVertexData);
	UpdateIndexBuffer(&m_InstantIndexData);

	return EError::OK;
}

PRIVATE auto JWText::CreateNonInstantTextBuffers()->EError
{
	if (m_NonInstantVertexData.pBuffer)
	{
		return EError::DUPLICATE_CREATION;
	}

	UINT ScreenWidth = m_pJWWindow->GetWindowData()->ScreenSize.x;
	UINT ScreenHeight = m_pJWWindow->GetWindowData()->ScreenSize.y;

	UINT FullSize = ms_FontData.Info.Size;
	UINT HalfSize = ms_FontData.Info.Size / static_cast <UINT>(2);

	UINT max_character_count_in_line = (ScreenWidth / HalfSize) + 1;
	UINT max_line_count = (ScreenHeight / FullSize) + 1;
	UINT max_character_total_count = max_character_count_in_line * max_line_count;

	// Set maximum size
	m_NonInstantVertexData.VertexSize = max_character_total_count * 4;
	m_NonInstantIndexData.IndexSize = max_character_total_count * 2;

	// Make rectangles for characters with maximum characters' count
	if (!m_NonInstantVertexData.Vertices)
	{
		m_NonInstantVertexData.Vertices = new SVertexImage[m_NonInstantVertexData.VertexSize];

		for (size_t iterator = 0; iterator < max_character_total_count; iterator++)
		{
			m_NonInstantVertexData.Vertices[iterator * 4] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 0);
			m_NonInstantVertexData.Vertices[iterator * 4 + 1] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 0);
			m_NonInstantVertexData.Vertices[iterator * 4 + 2] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 1);
			m_NonInstantVertexData.Vertices[iterator * 4 + 3] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 1);
		}
	}

	if (!m_NonInstantIndexData.Indices)
	{
		m_NonInstantIndexData.Indices = new SIndex3[m_NonInstantIndexData.IndexSize];

		for (size_t iterator = 0; iterator < max_character_total_count; iterator++)
		{
			m_NonInstantIndexData.Indices[iterator * 2] = SIndex3(iterator * 4, iterator * 4 + 1, iterator * 4 + 3);
			m_NonInstantIndexData.Indices[iterator * 2 + 1] = SIndex3(iterator * 4, iterator * 4 + 3, iterator * 4 + 2);
		}
	}

	CreateVertexBuffer(&m_NonInstantVertexData);
	CreateIndexBuffer(&m_NonInstantIndexData);
	UpdateVertexBuffer(&m_NonInstantVertexData);
	UpdateIndexBuffer(&m_NonInstantIndexData);

	m_bIsInstantText = true;

	return EError::OK;
}

PRIVATE auto JWText::CreateVertexBuffer(SVertexData* pVertexData)->EError
{
	int vertex_size_in_byte = sizeof(SVertexImage) * pVertexData->VertexSize;
	if (FAILED(m_pDevice->CreateVertexBuffer(vertex_size_in_byte, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &pVertexData->pBuffer, nullptr)))
	{
		return EError::VERTEX_BUFFER_NOT_CREATED;
	}
	return EError::OK;
}

PRIVATE auto JWText::CreateIndexBuffer(SIndexData* pIndexData)->EError
{
	int index_size_in_byte = sizeof(SIndex3) * pIndexData->IndexSize;
	if (FAILED(m_pDevice->CreateIndexBuffer(index_size_in_byte, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIndexData->pBuffer, nullptr)))
	{
		return EError::INDEX_BUFFER_NOT_CREATED;
	}
	return EError::OK;
}

PRIVATE auto JWText::UpdateVertexBuffer(SVertexData* pVertexData)->EError
{
	if (pVertexData->Vertices)
	{
		int temp_vertex_size = sizeof(SVertexImage) * pVertexData->VertexSize;
		VOID* pVertices;
		if (FAILED(pVertexData->pBuffer->Lock(0, temp_vertex_size, (void**)&pVertices, 0)))
		{
			return EError::VERTEX_BUFFER_NOT_LOCKED;
		}
		memcpy(pVertices, &pVertexData->Vertices[0], temp_vertex_size);
		pVertexData->pBuffer->Unlock();
		return EError::OK;
	}
	return EError::NULLPTR_VERTEX;
}

PRIVATE auto JWText::UpdateIndexBuffer(SIndexData* pIndexData)->EError
{
	if (pIndexData->Indices)
	{
		int temp_index_size = sizeof(SIndex3) * pIndexData->IndexSize;
		VOID* pIndices;
		if (FAILED(pIndexData->pBuffer->Lock(0, temp_index_size, (void **)&pIndices, 0)))
		{
			return EError::INDEX_BUFFER_NOT_LOCKED;
		}
		memcpy(pIndices, &pIndexData->Indices[0], temp_index_size);
		pIndexData->pBuffer->Unlock();
		return EError::OK;
	}
	return EError::NULLPTR_INDEX;
}

void JWText::DrawInstantText(WSTRING SingleLineText, const D3DXVECTOR2 Position, const EHorizontalAlignment HorizontalAlignment)
{
	// If SingleLineText is null, we don't need to draw it.
	if (!SingleLineText.length())
	{
		return;
	}

	// Check if the text length exceeds the maximum length (MAX_INSTANT_TEXT_LENGTH)
	// and if it does, clip the text.
	size_t iterator_character = 0;
	for (const wchar_t& Char : SingleLineText)
	{
		if (iterator_character == MAX_INSTANT_TEXT_LENGTH - 1)
		{
			SingleLineText = SingleLineText.substr(0, iterator_character);
			break;
		}

		iterator_character++;
	}

	// Check if the text contains '\n',
	// and if it does, clip the text.
	iterator_character = 0;
	for (const wchar_t& Char : SingleLineText)
	{
		if (Char == L'\n')
		{
			SingleLineText = SingleLineText.substr(0, iterator_character);
			break;
		}

		iterator_character++;
	}

	// Offset position according to the HorizontalAlignment.
	D3DXVECTOR2 offset_position = Position;
	switch (HorizontalAlignment)
	{
	case EHorizontalAlignment::Left:
		break;
	case EHorizontalAlignment::Center:
		offset_position.x = Position.x - (GetLineWidth(&SingleLineText) / 2.0f);
		break;
	case EHorizontalAlignment::Right:
		offset_position.x = Position.x - GetLineWidth(&SingleLineText);
		break;
	default:
		break;
	}

	// Convert each character of the text to Chars_ID in ms_FontData
	// in order to position them.
	iterator_character = 0;
	STextCharacterInfo curr_char_info;
	STextCharacterInfo prev_char_info;
	prev_char_info.x = offset_position.x;
	for (const wchar_t& Char : SingleLineText)
	{
		curr_char_info.chars_id = GetCharsIDFromCharacter(Char);
		curr_char_info.line_top_y = offset_position.y;
		curr_char_info.line_index = 0;

		SetInstantCharacter(iterator_character, &curr_char_info, &prev_char_info);
		
		prev_char_info = curr_char_info;
		iterator_character++;
	}

	// Update instant vertex buffer.
	UpdateVertexBuffer(&m_InstantVertexData);

	// Set alpha blending on.
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	if (m_pFontTexture)
	{
		// IF,
		// texture exists.

		// Set texture.
		m_pDevice->SetTexture(0, m_pFontTexture);

		// Texture alpha * Diffuse alpha
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

		// Texture color * Diffuse color
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	}

	// Set vertex format.
	m_pDevice->SetFVF(D3DFVF_TEXTURE);

	// Set vertex data.
	m_pDevice->SetStreamSource(0, m_InstantVertexData.pBuffer, 0, sizeof(SVertexImage));

	// Set index data.
	m_pDevice->SetIndices(m_InstantIndexData.pBuffer);

	// Draw-call.
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_InstantVertexData.VertexSize, 0, m_InstantIndexData.IndexSize);

	// Set texture to nullptr.
	m_pDevice->SetTexture(0, nullptr);
}

PRIVATE void JWText::SetInstantCharacter(size_t Character_index, STextCharacterInfo* pCurrInfo, const STextCharacterInfo* pPrevInfo)
{
	if (Character_index)
	{
		// This is NOT the first character of the text.
		pCurrInfo->x = pPrevInfo->x + ms_FontData.Chars[pPrevInfo->chars_id].XAdvance;
	}
	else
	{
		// This is the first character of the text.
		pCurrInfo->x = pPrevInfo->x;
	}
	pCurrInfo->y = pCurrInfo->line_top_y + ms_FontData.Chars[pCurrInfo->chars_id].YOffset;
	pCurrInfo->width = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Width);
	pCurrInfo->height = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Height);

	// Set u, v values
	float u1 = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].X) / static_cast<float>(ms_FontData.Common.ScaleW);
	float u2 = u1 + static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Width) / static_cast<float>(ms_FontData.Common.ScaleW);

	float v1 = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Y) / static_cast<float>(ms_FontData.Common.ScaleH);
	float v2 = v1 + static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Height) / static_cast<float>(ms_FontData.Common.ScaleH);

	m_InstantVertexData.Vertices[Character_index * 4].x = pCurrInfo->x;
	m_InstantVertexData.Vertices[Character_index * 4].y = pCurrInfo->y;
	m_InstantVertexData.Vertices[Character_index * 4].u = u1;
	m_InstantVertexData.Vertices[Character_index * 4].v = v1;
	m_InstantVertexData.Vertices[Character_index * 4 + 1].x = pCurrInfo->x + pCurrInfo->width;
	m_InstantVertexData.Vertices[Character_index * 4 + 1].y = pCurrInfo->y;
	m_InstantVertexData.Vertices[Character_index * 4 + 1].u = u2;
	m_InstantVertexData.Vertices[Character_index * 4 + 1].v = v1;
	m_InstantVertexData.Vertices[Character_index * 4 + 2].x = pCurrInfo->x;
	m_InstantVertexData.Vertices[Character_index * 4 + 2].y = pCurrInfo->y + pCurrInfo->height;
	m_InstantVertexData.Vertices[Character_index * 4 + 2].u = u1;
	m_InstantVertexData.Vertices[Character_index * 4 + 2].v = v2;
	m_InstantVertexData.Vertices[Character_index * 4 + 3].x = pCurrInfo->x + pCurrInfo->width;
	m_InstantVertexData.Vertices[Character_index * 4 + 3].y = pCurrInfo->y + pCurrInfo->height;
	m_InstantVertexData.Vertices[Character_index * 4 + 3].u = u2;
	m_InstantVertexData.Vertices[Character_index * 4 + 3].v = v2;
}

auto JWText::GetLineHeight() const->float
{
	return static_cast<float>(ms_FontData.Info.Size);
}

PRIVATE auto JWText::GetLineWidth(const WSTRING* pLineText) const->float
{
	size_t iterator_character = 0;
	size_t chars_id = 0;
	float width = 0;
	for (const wchar_t& Char : *pLineText)
	{
		chars_id = GetCharsIDFromCharacter(Char);
		width += ms_FontData.Chars[chars_id].XAdvance;

		iterator_character++;
	}
	return width;
}