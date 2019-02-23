#include "JWText.h"
#include "JWWindow.h"
#include "JWLine.h"
#include "JWRectangle.h"

using namespace JWENGINE;

// Static const
const float JWText::DEFAULT_SIDE_CONSTRAINT_STRIDE = 20.0f;

JWText::JWText()
{
	m_bIsInstantText = true;
	m_bUseAutomaticLineBreak = true;

	m_pJWWindow = nullptr;
	m_pBaseDir = nullptr;
	m_pDevice = nullptr;
	m_pFontTexture = nullptr;

	m_ConstraintPosition = D3DXVECTOR2(0, 0);
	m_ConstraintSize = D3DXVECTOR2(0, 0);

	m_pCaretLine = nullptr;
	m_CaretPosition = D3DXVECTOR2(0, 0);
	m_CaretSize = D3DXVECTOR2(0, 0);
	m_CaretSelPosition = 0;

	m_pSelectionBox = nullptr;
	m_CapturedSelPosition = SIZE_T_INVALID;
	m_SelectionStart = 0;
	m_SelectionEnd = 0;
	m_bIsTextSelected = false;

	m_NonInstantTextOffset = D3DXVECTOR2(0, 0);
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

	// This is an instant-text JWText.
	m_bIsInstantText = true;

	return EError::OK;
}

auto JWText::CreateNonInstantText(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const LPDIRECT3DTEXTURE9 pFontTexture)->EError
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

	if (m_pCaretLine = new JWLine)
	{
		m_pCaretLine->Create(m_pDevice);

		m_CaretSize.y = static_cast<float>(ms_FontData.Info.Size);
		m_pCaretLine->AddLine(m_CaretPosition, m_CaretSize, DEFAULT_COLOR_CARET);
		m_pCaretLine->AddEnd();
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Create a JWRectangle for selection box.
	if (m_pSelectionBox = new JWRectangle)
	{
		UINT max_box_num = static_cast<UINT>((m_pJWWindow->GetWindowData()->ScreenSize.y / GetLineHeight()) + 1);

		if (JW_FAILED(m_pSelectionBox->Create(m_pJWWindow, m_pBaseDir, max_box_num)))
			return EError::RECTANGLE_NOT_CREATED;

		m_pSelectionBox->SetRectangleColor(DEFAULT_COLOR_SELECTION);
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// This is a non-instant-text JWText.
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

		JW_DESTROY(m_pCaretLine);
		JW_DESTROY(m_pSelectionBox);
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

void JWText::UpdateNonInstantText(WSTRING Text, const D3DXVECTOR2 Position, const D3DXVECTOR2 AreaSize)
{
	m_ConstraintPosition = Position;
	m_ConstraintSize = AreaSize;
	
	m_NonInstantTextInfo.clear();

	// Convert each character of the text to Chars_ID in ms_FontData
	// in order to position them.
	SGlyphInfo curr_glyph_info;
		curr_glyph_info.line_index = 0;
		curr_glyph_info.left = Position.x;
		curr_glyph_info.top = Position.y;
		curr_glyph_info.glyph_index_in_line = 0;
	SGlyphInfo prev_glyph_info;
	bool b_is_line_first_glyph = true;

	for (size_t iterator_char = 0; iterator_char <= Text.length(); iterator_char++)
	{
		wchar_t Char = 0;

		// If iterator_char meets the end of the Text('\0'), Char must be 0.
		if (iterator_char < Text.length())
		{
			Char = Text[iterator_char];
		}

		curr_glyph_info.chars_id = GetCharsIDFromCharacter(Char);

		// In order to make '\n' invisible to users.
		if (Char == '\n')
		{
			curr_glyph_info.chars_id = 0;
		}

		SetNonInstantTextGlyph(b_is_line_first_glyph, &curr_glyph_info, &prev_glyph_info);

		if (m_bUseAutomaticLineBreak)
		{
			if (curr_glyph_info.left + curr_glyph_info.width >= m_ConstraintPosition.x + m_ConstraintSize.x)
			{
				curr_glyph_info.line_index++;

				b_is_line_first_glyph = true;
				curr_glyph_info.left = Position.x;
				curr_glyph_info.top += GetLineHeight();
				curr_glyph_info.glyph_index_in_line = 0;

				SetNonInstantTextGlyph(b_is_line_first_glyph, &curr_glyph_info, &prev_glyph_info);
			}
		}

		m_NonInstantTextInfo.push_back(curr_glyph_info);

		curr_glyph_info.glyph_index_in_line++;

		if (Char == '\n')
		{
			// IF,
			// the character was '\n'.
			curr_glyph_info.line_index++;

			b_is_line_first_glyph = true;
			curr_glyph_info.left = Position.x;
			curr_glyph_info.top += GetLineHeight();
			curr_glyph_info.glyph_index_in_line = 0;
		}
		else
		{
			b_is_line_first_glyph = false;
		}

		prev_glyph_info = curr_glyph_info;
	}

	UpdateNonInstantTextVertices();

	UpdateCaret();
}

PRIVATE void JWText::UpdateNonInstantTextVertices()
{
	// Clear the vertex buffer's uv data.
	for (size_t iterator = 0; iterator < m_NonInstantVertexData.VertexSize; iterator++)
	{
		m_NonInstantVertexData.Vertices[iterator].u = 0;
		m_NonInstantVertexData.Vertices[iterator].v = 0;
	}

	// Set vertex data (but only for the visible glyphs).
	size_t visible_glyph_count = 0;
	float u1 = 0, u2 = 0, v1 = 0, v2 = 0;
	float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	float constraint_top = m_ConstraintPosition.y - m_NonInstantTextOffset.y;
	float constraint_bottom = constraint_top + m_ConstraintSize.y;

	for (size_t iterator_glyph = 0; iterator_glyph < m_NonInstantTextInfo.size(); iterator_glyph++)
	{
		const SGlyphInfo& glyph = m_NonInstantTextInfo[iterator_glyph];

		// Bottom constraint
		if (glyph.top > constraint_bottom)
		{
			break;
		}

		// Top constraint
		if (glyph.top + glyph.height >= constraint_top)
		{
			// Set u, v values
			u1 = static_cast<float>(ms_FontData.Chars[glyph.chars_id].X) / static_cast<float>(ms_FontData.Common.ScaleW);
			u2 = u1 + static_cast<float>(ms_FontData.Chars[glyph.chars_id].Width) / static_cast<float>(ms_FontData.Common.ScaleW);

			v1 = static_cast<float>(ms_FontData.Chars[glyph.chars_id].Y) / static_cast<float>(ms_FontData.Common.ScaleH);
			v2 = v1 + static_cast<float>(ms_FontData.Chars[glyph.chars_id].Height) / static_cast<float>(ms_FontData.Common.ScaleH);

			// Set x, y values
			x1 = m_NonInstantTextOffset.x + glyph.left;
			x2 = x1 + glyph.width;

			y1 = m_NonInstantTextOffset.y + glyph.drawing_top;
			y2 = y1 + glyph.height;

			// Side constraints
			if ((x2 >= m_ConstraintPosition.x) && (x1 <= m_ConstraintPosition.x + m_ConstraintSize.x))
			{
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4].x = x1;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4].y = y1;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4].u = u1;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4].v = v1;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 1].x = x2;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 1].y = y1;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 1].u = u2;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 1].v = v1;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 2].x = x1;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 2].y = y2;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 2].u = u1;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 2].v = v2;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 3].x = x2;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 3].y = y2;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 3].u = u2;
				m_NonInstantVertexData.Vertices[visible_glyph_count * 4 + 3].v = v2;

				visible_glyph_count++;
			}
		}
	}

	// Update non-instant-text vertex buffer.
	UpdateVertexBuffer(&m_NonInstantVertexData);
}

void JWText::DrawNonInstantText()
{
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
	m_pDevice->SetStreamSource(0, m_NonInstantVertexData.pBuffer, 0, sizeof(SVertexImage));

	// Set index data.
	m_pDevice->SetIndices(m_NonInstantIndexData.pBuffer);

	// Draw-call.
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_NonInstantVertexData.VertexSize, 0, m_NonInstantIndexData.IndexSize);

	// Set texture to nullptr.
	m_pDevice->SetTexture(0, nullptr);
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
	if (SingleLineText.length() > MAX_INSTANT_TEXT_LENGTH)
	{
		SingleLineText = SingleLineText.substr(0, MAX_INSTANT_TEXT_LENGTH);
	}

	// Check if the text contains '\n',
	// and if it does, clip the text.
	size_t iterator_character = 0;
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

	// Clear the vertex buffer's uv data.
	for (size_t iterator = 0; iterator < MAX_INSTANT_TEXT_VERTEX_SIZE; iterator++)
	{
		m_InstantVertexData.Vertices[iterator].u = 0;
		m_InstantVertexData.Vertices[iterator].v = 0;
	}

	// Convert each character of the text to Chars_ID in ms_FontData
	// in order to position them.
	iterator_character = 0;
	SGlyphInfo curr_char_info;
	SGlyphInfo prev_char_info;
	prev_char_info.left = offset_position.x;
	for (const wchar_t& Char : SingleLineText)
	{
		curr_char_info.chars_id = GetCharsIDFromCharacter(Char);
		curr_char_info.top = offset_position.y;
		curr_char_info.line_index = 0;

		SetInstantTextGlyph(iterator_character, &curr_char_info, &prev_char_info);
		
		prev_char_info = curr_char_info;
		iterator_character++;
	}

	// Update instant-text vertex buffer.
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

PRIVATE void JWText::UpdateCaret()
{
	if (m_pCaretLine)
	{
		m_CaretSelPosition = min(m_CaretSelPosition, m_NonInstantTextInfo.size() - 1);

		// This is needed in case m_ConstraintSize.x is smaller than DEFAULT_SIDE_CONSTRAINT_STRIDE.
		float default_side_stride = DEFAULT_SIDE_CONSTRAINT_STRIDE;
		default_side_stride = min(default_side_stride, m_ConstraintSize.x);

		// Left constraint
		float caret_x = m_NonInstantTextInfo[m_CaretSelPosition].left + m_NonInstantTextOffset.x;
		float constraint_left = m_ConstraintPosition.x;
		if (caret_x < constraint_left)
		{
			float stride = constraint_left - caret_x;
			stride = max(stride, default_side_stride);

			m_NonInstantTextOffset.x += stride;
			m_NonInstantTextOffset.x = min(m_NonInstantTextOffset.x, 0);
		}

		// Right constraint
		float constraint_right = m_ConstraintPosition.x + m_ConstraintSize.x;
		size_t current_line_index = m_NonInstantTextInfo[m_CaretSelPosition].line_index;
		if (caret_x > constraint_right)
		{
			float stride = caret_x - constraint_right;
			stride = max(stride, default_side_stride);

			m_NonInstantTextOffset.x -= stride;

			size_t line_end_glyph_index = GetLineEndGlyphIndex(current_line_index);
			float line_end_glyph_x = m_NonInstantTextInfo[line_end_glyph_index].left;

			m_NonInstantTextOffset.x = max(m_NonInstantTextOffset.x, constraint_right - line_end_glyph_x);
		}

		// Top constraint
		float caret_top = m_NonInstantTextInfo[m_CaretSelPosition].top;
		float constraint_top = m_ConstraintPosition.y;
		if (caret_top <= constraint_top)
		{
			m_NonInstantTextOffset.y = constraint_top - caret_top;
		}

		// Bottom constraint
		float caret_bottom = m_NonInstantTextInfo[m_CaretSelPosition].top + m_CaretSize.y;
		float constraint_bottom = m_ConstraintPosition.y + m_ConstraintSize.y;
		if (caret_bottom >= constraint_bottom)
		{
			m_NonInstantTextOffset.y = constraint_bottom - caret_bottom;
		}

		m_CaretPosition.x = m_NonInstantTextOffset.x + m_NonInstantTextInfo[m_CaretSelPosition].left;
		m_CaretPosition.y = m_NonInstantTextOffset.y + m_NonInstantTextInfo[m_CaretSelPosition].top;

		UpdateNonInstantTextVertices();

		m_pCaretLine->SetLine(0, m_CaretPosition, m_CaretSize);
	}
}

void JWText::DrawCaret()
{
	if (m_pCaretLine)
	{
		m_pCaretLine->Draw();
	}
}

void JWText::DrawSelectionBox()
{
	if (m_pSelectionBox)
	{
		m_pSelectionBox->Draw();
	}
}

PRIVATE void JWText::SetInstantTextGlyph(size_t Character_index, SGlyphInfo* pCurrInfo, const SGlyphInfo* pPrevInfo)
{
	if (Character_index)
	{
		// This is NOT the first character of the text.
		pCurrInfo->left = pPrevInfo->left + ms_FontData.Chars[pPrevInfo->chars_id].XAdvance;
	}
	else
	{
		// This is the first character of the text.
		pCurrInfo->left = pPrevInfo->left;
	}
	pCurrInfo->drawing_top = pCurrInfo->top + ms_FontData.Chars[pCurrInfo->chars_id].YOffset;
	pCurrInfo->width = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Width);
	pCurrInfo->height = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Height);

	// Set u, v values
	float u1 = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].X) / static_cast<float>(ms_FontData.Common.ScaleW);
	float u2 = u1 + static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Width) / static_cast<float>(ms_FontData.Common.ScaleW);

	float v1 = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Y) / static_cast<float>(ms_FontData.Common.ScaleH);
	float v2 = v1 + static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Height) / static_cast<float>(ms_FontData.Common.ScaleH);

	// Set x, y values
	float x1 = pCurrInfo->left;
	float x2 = pCurrInfo->left + pCurrInfo->width;

	float y1 = pCurrInfo->drawing_top;
	float y2 = pCurrInfo->drawing_top + pCurrInfo->height;

	m_InstantVertexData.Vertices[Character_index * 4].x = x1;
	m_InstantVertexData.Vertices[Character_index * 4].y = y1;
	m_InstantVertexData.Vertices[Character_index * 4].u = u1;
	m_InstantVertexData.Vertices[Character_index * 4].v = v1;
	m_InstantVertexData.Vertices[Character_index * 4 + 1].x = x2;
	m_InstantVertexData.Vertices[Character_index * 4 + 1].y = y1;
	m_InstantVertexData.Vertices[Character_index * 4 + 1].u = u2;
	m_InstantVertexData.Vertices[Character_index * 4 + 1].v = v1;
	m_InstantVertexData.Vertices[Character_index * 4 + 2].x = x1;
	m_InstantVertexData.Vertices[Character_index * 4 + 2].y = y2;
	m_InstantVertexData.Vertices[Character_index * 4 + 2].u = u1;
	m_InstantVertexData.Vertices[Character_index * 4 + 2].v = v2;
	m_InstantVertexData.Vertices[Character_index * 4 + 3].x = x2;
	m_InstantVertexData.Vertices[Character_index * 4 + 3].y = y2;
	m_InstantVertexData.Vertices[Character_index * 4 + 3].u = u2;
	m_InstantVertexData.Vertices[Character_index * 4 + 3].v = v2;
}

PRIVATE void JWText::SetNonInstantTextGlyph(bool bIsLineFirstGlyph, SGlyphInfo* pCurrInfo, const SGlyphInfo* pPrevInfo)
{
	if (!bIsLineFirstGlyph)
	{
		// This is NOT the first character of the text.
		pCurrInfo->left = pPrevInfo->left + ms_FontData.Chars[pPrevInfo->chars_id].XAdvance;
	}
	pCurrInfo->drawing_top = pCurrInfo->top + ms_FontData.Chars[pCurrInfo->chars_id].YOffset;

	pCurrInfo->width = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Width);
	pCurrInfo->height = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Height);
}

auto JWText::GetFontTexturePtr() const-> const LPDIRECT3DTEXTURE9
{
	return m_pFontTexture;
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

void JWText::MoveCaretTo(size_t SelPosition)
{
	m_CaretSelPosition = SelPosition;

	UpdateCaret();

	ReleaseSelection();
}

void JWText::MoveCaretToLeft()
{
	if (m_CaretSelPosition)
	{
		m_CaretSelPosition--;

		UpdateCaret();
	}
}

void JWText::MoveCaretToRight()
{
	m_CaretSelPosition++;

	UpdateCaret();
}

PRIVATE auto JWText::GetLineStartGlyphIndex(const size_t LineIndex)->size_t
{
	size_t result = SIZE_T_INVALID;

	for (size_t iterator_glyph = 0; iterator_glyph < m_NonInstantTextInfo.size(); iterator_glyph++)
	{
		if (m_NonInstantTextInfo[iterator_glyph].line_index == LineIndex)
		{
			result = iterator_glyph;
			break;
		}
	}

	return result;
}

PRIVATE auto JWText::GetLineEndGlyphIndex(const size_t LineIndex)->size_t
{
	size_t result = 0;

	for (size_t iterator_glyph = 0; iterator_glyph < m_NonInstantTextInfo.size(); iterator_glyph++)
	{
		if (m_NonInstantTextInfo[iterator_glyph].line_index == LineIndex)
		{
			result = iterator_glyph;
		}
		else if (m_NonInstantTextInfo[iterator_glyph].line_index > LineIndex)
		{
			break;
		}
	}

	return result;
}

void JWText::MoveCaretUp()
{
	size_t curr_index_in_line = m_NonInstantTextInfo[m_CaretSelPosition].glyph_index_in_line;
	size_t curr_line_index = m_NonInstantTextInfo[m_CaretSelPosition].line_index;
	if (!curr_line_index)
	{
		return;
	}
	size_t goal_line_index = curr_line_index - 1;

	size_t line_start = GetLineStartGlyphIndex(goal_line_index);
	size_t line_end = GetLineEndGlyphIndex(goal_line_index);
	size_t line_length = line_end - line_start;

	curr_index_in_line = min(curr_index_in_line, line_length);

	m_CaretSelPosition = line_start + curr_index_in_line;

	UpdateCaret();
}

void JWText::MoveCaretDown()
{
	size_t curr_index_in_line = m_NonInstantTextInfo[m_CaretSelPosition].glyph_index_in_line;
	size_t curr_line_index = m_NonInstantTextInfo[m_CaretSelPosition].line_index;
	size_t goal_line_index = curr_line_index + 1;

	size_t line_start = GetLineStartGlyphIndex(goal_line_index);
	if (line_start == SIZE_T_INVALID)
	{
		return;
	}
	size_t line_end = GetLineEndGlyphIndex(goal_line_index);
	size_t line_length = line_end - line_start;

	curr_index_in_line = min(curr_index_in_line, line_length);

	m_CaretSelPosition = line_start + curr_index_in_line;

	UpdateCaret();
}

void JWText::MoveCaretHome()
{
	size_t curr_line_index = m_NonInstantTextInfo[m_CaretSelPosition].line_index;
	m_CaretSelPosition = GetLineStartGlyphIndex(curr_line_index);

	UpdateCaret();
}

void JWText::MoveCaretEnd()
{
	size_t curr_line_index = m_NonInstantTextInfo[m_CaretSelPosition].line_index;
	m_CaretSelPosition = GetLineEndGlyphIndex(curr_line_index);

	UpdateCaret();
}

void JWText::ReleaseSelection()
{
	m_CapturedSelPosition = SIZE_T_INVALID;

	m_pSelectionBox->ClearAllRectangles();
	
	m_bIsTextSelected = false;
}

void JWText::SelectToLeft()
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}

	MoveCaretToLeft();

	UpdateSelectionBox();
}

void JWText::SelectToRight()
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}

	MoveCaretToRight();

	UpdateSelectionBox();
}

void JWText::SelectUp()
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}

	MoveCaretUp();

	UpdateSelectionBox();
}

void JWText::SelectDown()
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}
	
	MoveCaretDown();

	UpdateSelectionBox();
}

void JWText::SelectHome()
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}
	
	MoveCaretHome();

	UpdateSelectionBox();
}

void JWText::SelectEnd()
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}

	MoveCaretEnd();

	UpdateSelectionBox();
}

void JWText::SelectAll()
{
	m_CapturedSelPosition = 0;
	m_CaretSelPosition = m_NonInstantTextInfo.size() - 1;

	UpdateCaret();

	UpdateSelectionBox();
}

PRIVATE void JWText::UpdateSelectionBox()
{
	if (m_CapturedSelPosition != SIZE_T_INVALID)
	{
		m_bIsTextSelected = true;

		size_t sel_start = min(m_CapturedSelPosition, m_CaretSelPosition);
		size_t sel_end = max(m_CapturedSelPosition, m_CaretSelPosition);

		m_SelectionStart = sel_start;
		m_SelectionEnd = sel_end;

		size_t line_start = m_NonInstantTextInfo[sel_start].line_index;
		size_t line_end = m_NonInstantTextInfo[sel_end].line_index;

		D3DXVECTOR2 selection_position = D3DXVECTOR2(0, 0);
		D3DXVECTOR2 selection_size = D3DXVECTOR2(0, 0);
		selection_size.y = GetLineHeight();

		m_pSelectionBox->ClearAllRectangles();

		if (line_start == line_end)
		{
			// Single-line selection.

			selection_position.x = m_NonInstantTextOffset.x + m_NonInstantTextInfo[sel_start].left;
			selection_position.y = m_NonInstantTextOffset.y + m_NonInstantTextInfo[sel_start].top;

			selection_size.x = m_NonInstantTextInfo[sel_end].left - m_NonInstantTextInfo[sel_start].left;

			m_pSelectionBox->AddRectangle(selection_size, selection_position);
		}
		else
		{
			// Multi-line selection.

			for (size_t iterator_line = line_start; iterator_line <= line_end; iterator_line++)
			{
				size_t line_start_glyph = GetLineStartGlyphIndex(iterator_line);
				size_t line_end_glyph = GetLineEndGlyphIndex(iterator_line);

				if (iterator_line == line_start)
				{
					// Select from sel_start to line_end_glyph.
					selection_position.x = m_NonInstantTextOffset.x + m_NonInstantTextInfo[sel_start].left;
					selection_position.y = m_NonInstantTextOffset.y + m_NonInstantTextInfo[sel_start].top;

					selection_size.x = m_NonInstantTextInfo[line_end_glyph].left - m_NonInstantTextInfo[sel_start].left;
				}
				else if (iterator_line == line_end)
				{
					// Select from line_start_glyph to sel_end.
					selection_position.x = m_NonInstantTextOffset.x + m_NonInstantTextInfo[line_start_glyph].left;
					selection_position.y = m_NonInstantTextOffset.y + m_NonInstantTextInfo[line_start_glyph].top;

					selection_size.x = m_NonInstantTextInfo[sel_end].left - m_NonInstantTextInfo[line_start_glyph].left;
				}
				else
				{
					// IF,
					// this line isn't the start line nor the end line,
					// select the whole line.
					selection_position.x = m_NonInstantTextOffset.x + m_NonInstantTextInfo[line_start_glyph].left;
					selection_position.y = m_NonInstantTextOffset.y + m_NonInstantTextInfo[line_start_glyph].top;

					selection_size.x = m_NonInstantTextInfo[line_end_glyph].left - m_NonInstantTextInfo[line_start_glyph].left;
				}

				// Top & bottom constraint
				if ((selection_position.y + selection_size.y >= m_ConstraintPosition.y)
					&& (selection_position.y <= m_ConstraintPosition.y + m_ConstraintSize.y))
				{
					m_pSelectionBox->AddRectangle(selection_size, selection_position);
				}

			}
		}
	}
}

auto JWText::GetCaretSelPosition() const->const size_t
{
	return m_CaretSelPosition;
}

void JWText::ShouldUseAutomaticLineBreak(bool Value)
{
	m_bUseAutomaticLineBreak = Value;
}

auto JWText::GetSelectionStart() const->const size_t
{
	return m_SelectionStart;
}

auto JWText::GetSelectionEnd() const->const size_t
{
	return m_SelectionEnd;
}

auto JWText::IsTextSelected() const->const bool
{
	return m_bIsTextSelected;
}