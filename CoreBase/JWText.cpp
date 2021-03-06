#include "JWText.h"
#include "JWWindow.h"
#include "JWLine.h"
#include "JWRectangle.h"

using namespace JWENGINE;

JWText::~JWText()
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
	JW_RELEASE(m_InstantIndexData.pBuffer);

	JW_RELEASE(m_NonInstantVertexData.pBuffer);
	JW_RELEASE(m_NonInstantIndexData.pBuffer);
}

void JWText::CreateInstantText(const JWWindow& Window, const WSTRING& BaseDir) noexcept
{
	if (m_pFontTexture)
	{
		// Avoid duplicate creation.
		return;
	}

	m_pJWWindow = &Window;
	m_pDevice = Window.GetDevice();
	m_pBaseDir = &BaseDir;

	// Create font texture.
	CreateFontTexture(DEFAULT_FONT);

	CreateInstantTextBuffers();

	// This is an instant-text JWText.
	m_bIsInstantText = true;
}

void JWText::CreateNonInstantText(const JWWindow& Window, const WSTRING& BaseDir, const LPDIRECT3DTEXTURE9 pFontTexture) noexcept
{
	if (m_pFontTexture)
	{
		// Avoid duplicate creation.
		return;
	}

	m_pJWWindow = &Window;
	m_pDevice = Window.GetDevice();
	m_pBaseDir = &BaseDir;

	// Set font texture.
	m_pFontTexture = pFontTexture;

	CreateNonInstantTextBuffers();

	m_pCaretLine = MAKE_UNIQUE(JWLine)();
	m_pCaretLine->Create(m_pDevice);
	m_CaretSize.y = static_cast<float>(ms_FontData.Info.Size);
	m_pCaretLine->AddLine(m_CaretPosition, m_CaretSize, DEFAULT_COLOR_CARET);
	m_pCaretLine->AddEnd();

	// Create a JWRectangle for selection box.
	m_pSelectionBox = MAKE_UNIQUE(JWRectangle)();
	UINT max_box_num = static_cast<UINT>((m_pJWWindow->GetWindowData()->ScreenSize.y / GetLineHeight()) + 1);
	m_pSelectionBox->Create(*m_pJWWindow, *m_pBaseDir, max_box_num);
	m_pSelectionBox->SetRectangleColor(DEFAULT_COLOR_SELECTION);

	// This is a non-instant-text JWText.
	m_bIsInstantText = false;
}

PRIVATE void JWText::CreateFontTexture(const WSTRING& FileName_FNT) noexcept
{
	if (m_pFontTexture)
	{
		// Avoid duplicate creation.
		return;
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
	{
		assert(false);
	}
}

PRIVATE void JWText::CreateInstantTextBuffers() noexcept
{
	if (m_InstantVertexData.pBuffer)
	{
		// Avoid duplicate creation.
		return;
	}

	// Make rectangles for characters with maximum characters' count
	m_InstantVertexData.Vertices.clear();
	m_InstantVertexData.Vertices.reserve(MAX_INSTANT_TEXT_LENGTH * 4);
	m_InstantVertexData.Vertices.resize(MAX_INSTANT_TEXT_LENGTH * 4);

	m_InstantIndexData.Indices.clear();
	m_InstantIndexData.Indices.reserve(MAX_INSTANT_TEXT_LENGTH * 2);
	for (size_t iterator = 0; iterator < MAX_INSTANT_TEXT_LENGTH; iterator++)
	{
		m_InstantIndexData.Indices.push_back(SIndex3(iterator * 4, iterator * 4 + 1, iterator * 4 + 3));
		m_InstantIndexData.Indices.push_back(SIndex3(iterator * 4, iterator * 4 + 3, iterator * 4 + 2));
	}

	CreateVertexBuffer(&m_InstantVertexData);
	CreateIndexBuffer(&m_InstantIndexData);
	UpdateVertexBuffer(&m_InstantVertexData);
	UpdateIndexBuffer(&m_InstantIndexData);
}

PRIVATE void JWText::CreateNonInstantTextBuffers() noexcept
{
	if (m_NonInstantVertexData.pBuffer)
	{
		// Avoid duplicate creation.
		return;
	}

	UINT ScreenWidth = m_pJWWindow->GetWindowData()->ScreenSize.x;
	UINT ScreenHeight = m_pJWWindow->GetWindowData()->ScreenSize.y;

	UINT FullSize = ms_FontData.Info.Size;
	UINT HalfSize = ms_FontData.Info.Size / static_cast <UINT>(2);

	UINT max_character_count_in_line = (ScreenWidth / HalfSize) + 1;
	UINT max_line_count = (ScreenHeight / FullSize) + 1;
	UINT max_character_total_count = max_character_count_in_line * max_line_count;

	// Make rectangles for characters with maximum characters' count
	m_NonInstantVertexData.Vertices.clear();
	m_NonInstantVertexData.Vertices.reserve(max_character_total_count * 4);
	m_NonInstantVertexData.Vertices.resize(max_character_total_count * 4);

	m_NonInstantIndexData.Indices.clear();
	m_NonInstantIndexData.Indices.reserve(max_character_total_count * 2);
	for (size_t iterator = 0; iterator < max_character_total_count; iterator++)
	{
		m_NonInstantIndexData.Indices.push_back(SIndex3(iterator * 4, iterator * 4 + 1, iterator * 4 + 3));
		m_NonInstantIndexData.Indices.push_back(SIndex3(iterator * 4, iterator * 4 + 3, iterator * 4 + 2));
	}

	CreateVertexBuffer(&m_NonInstantVertexData);
	CreateIndexBuffer(&m_NonInstantIndexData);
	UpdateVertexBuffer(&m_NonInstantVertexData);
	UpdateIndexBuffer(&m_NonInstantIndexData);

	m_bIsInstantText = true;
}

PRIVATE void JWText::CreateVertexBuffer(SVertexData* pVertexData) noexcept
{
	pVertexData->SizeInByte = static_cast<int>(sizeof(SVertexImage) * pVertexData->Vertices.size());
	if (FAILED(m_pDevice->CreateVertexBuffer(pVertexData->SizeInByte, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &pVertexData->pBuffer, nullptr)))
	{
		assert(&pVertexData->pBuffer);
	}
}

PRIVATE void JWText::CreateIndexBuffer(SIndexData* pIndexData) noexcept
{
	pIndexData->SizeInByte = static_cast<int>(sizeof(SIndex3) * pIndexData->Indices.size());
	if (FAILED(m_pDevice->CreateIndexBuffer(pIndexData->SizeInByte, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIndexData->pBuffer, nullptr)))
	{
		assert(&pIndexData->pBuffer);
	}
}

PRIVATE void JWText::UpdateVertexBuffer(SVertexData* pVertexData) noexcept
{
	if (pVertexData->SizeInByte)
	{
		void* pVertices;
		if (SUCCEEDED(pVertexData->pBuffer->Lock(0, pVertexData->SizeInByte, (void**)&pVertices, 0)))
		{
			memcpy(pVertices, &pVertexData->Vertices[0], pVertexData->SizeInByte);
			pVertexData->pBuffer->Unlock();
		}
	}
}

PRIVATE void JWText::UpdateIndexBuffer(SIndexData* pIndexData) noexcept
{
	if (pIndexData->SizeInByte)
	{
		void* pIndices;
		if (SUCCEEDED(pIndexData->pBuffer->Lock(0, pIndexData->SizeInByte, (void **)&pIndices, 0)))
		{
			memcpy(pIndices, &pIndexData->Indices[0], pIndexData->SizeInByte);
			pIndexData->pBuffer->Unlock();
		}
	}
}

void JWText::SetNonInstantText(const WSTRING& Text, const D3DXVECTOR2& Position, const D3DXVECTOR2& AreaSize) noexcept
{
	m_NonInstantText = Text;

	m_ConstraintPosition = Position;
	m_ConstraintSize = AreaSize;
	
	m_NonInstantTextGlyphInfo.clear();
	m_NonInstantTextLineInfo.clear();

	// Convert each character of the text to Chars_ID in ms_FontData
	// in order to position them.
	SGlyphInfo curr_glyph_info = SGlyphInfo(m_ConstraintPosition.x, m_ConstraintPosition.y);
	SGlyphInfo prev_glyph_info;
	wchar_t character = 0;

	for (size_t iterator_char = 0; iterator_char <= m_NonInstantText.length(); iterator_char++)
	{
		// If iterator_char meets the end of the Text('\0'), Char must be 0.
		if (iterator_char < m_NonInstantText.length())
		{
			character = m_NonInstantText[iterator_char];
		}
		else
		{
			character = 0;
		}
		
		// Get chars_id.
		curr_glyph_info.chars_id = GetCharsIDFromCharacter(character);

		// This is required in order to make '\n' invisible to users.
		if (character == '\n')
		{
			curr_glyph_info.chars_id = 0;
		}

		SetNonInstantTextGlyph(&curr_glyph_info, &prev_glyph_info);

		// Save glyph info.
		m_NonInstantTextGlyphInfo.push_back(curr_glyph_info);

		if (iterator_char)
		{
			if ((curr_glyph_info.glyph_index_in_line == 0) || (iterator_char == m_NonInstantText.length()))
			{
				// IF,
				// this is the first glyph in the line, or the last glyph of the text.

				size_t line_start = 0;
				size_t line_end = iterator_char - 1;

				if (m_NonInstantTextLineInfo.size())
				{
					// This is not the first line.
					line_start = m_NonInstantTextLineInfo[m_NonInstantTextLineInfo.size() - 1].end_glyph_index + 1;
				}

				if (iterator_char == m_NonInstantText.length())
				{
					line_end++;
				}

				m_NonInstantTextLineInfo.push_back(SLineInfo(line_start, line_end));
			}
		}
		else if (iterator_char == m_NonInstantText.length())
		{
			// IF,
			// the text is null.

			m_NonInstantTextLineInfo.push_back(SLineInfo(0, 0));
		}
	}

	UpdateNonInstantTextVisibleVertices();

	UpdateCaret();
}

void JWText::SetNonInstantTextString(const WSTRING& Text) noexcept
{
	m_NonInstantText = Text;
}

void JWText::SetNonInstantTextColor(DWORD FontColor) noexcept
{
	m_NonInstantTextColor = FontColor;

	UpdateNonInstantTextVisibleVertices();
}

void JWText::InsertInNonInstantText(const WSTRING& String) noexcept
{
	m_NonInstantText = m_NonInstantText.substr(0, m_CaretSelPosition) + String + m_NonInstantText.substr(m_CaretSelPosition);

	SGlyphInfo curr_glyph_info = SGlyphInfo(m_ConstraintPosition.x, m_ConstraintPosition.y);
	SGlyphInfo prev_glyph_info;
	if (m_CaretSelPosition)
	{
		prev_glyph_info = m_NonInstantTextGlyphInfo[m_CaretSelPosition - 1];
	}
	wchar_t character = 0;

	for (size_t iterator_char = m_CaretSelPosition; iterator_char <= m_NonInstantText.length(); iterator_char++)
	{
		// If iterator_char meets the end of the Text('\0'), Char must be 0.
		if (iterator_char < m_NonInstantText.length())
		{
			character = m_NonInstantText[iterator_char];
		}
		else
		{
			character = 0;
		}

		// Get chars_id.
		curr_glyph_info.chars_id = GetCharsIDFromCharacter(character);

		// This is required in order to make '\n' invisible to users.
		if (character == '\n')
		{
			curr_glyph_info.chars_id = 0;
		}

		SetNonInstantTextGlyph(&curr_glyph_info, &prev_glyph_info);

		// Save glyph info.
		if (iterator_char == m_NonInstantTextGlyphInfo.size())
		{
			m_NonInstantTextGlyphInfo.push_back(curr_glyph_info);
		}
		else
		{
			m_NonInstantTextGlyphInfo[iterator_char] = curr_glyph_info;
		}

		if (iterator_char)
		{
			if ((curr_glyph_info.glyph_index_in_line == 0) || (iterator_char == m_NonInstantText.length()))
			{
				// IF,
				// this is the first glyph in the line, or the last glyph of the text.

				size_t line_index = curr_glyph_info.line_index - 1;

				if (iterator_char == m_NonInstantText.length())
				{
					line_index++;
				}

				size_t line_start = 0;
				size_t line_end = iterator_char - 1;

				if (line_index)
				{
					// This is not the first line.
					line_start = m_NonInstantTextLineInfo[line_index - 1].end_glyph_index + 1;
				}

				if (iterator_char == m_NonInstantText.length())
				{
					line_end++;
				}

				if (line_index == m_NonInstantTextLineInfo.size())
				{
					m_NonInstantTextLineInfo.push_back(SLineInfo(line_start, line_end));
				}
				else
				{
					m_NonInstantTextLineInfo[line_index] = SLineInfo(line_start, line_end);
				}
			}
		}
	}

	UpdateNonInstantTextVisibleVertices();

	UpdateCaret();
}

PRIVATE void JWText::UpdateNonInstantTextVisibleVertices() noexcept
{
	// Clear the vertex buffer's uv data & set font color.
	for (auto& iterator: m_NonInstantVertexData.Vertices)
	{
		iterator.u = 0;
		iterator.v = 0;
		iterator.color = m_NonInstantTextColor;
	}

	// Set vertex data (but only for the visible glyphs).
	size_t visible_glyph_count = 0;
	float u1 = 0, u2 = 0, v1 = 0, v2 = 0;
	float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	float constraint_top = m_ConstraintPosition.y - m_NonInstantTextOffset.y;
	float constraint_bottom = constraint_top + m_ConstraintSize.y;

	for (const auto& iterator_glyph : m_NonInstantTextGlyphInfo)
	{
		// Bottom constraint
		if (iterator_glyph.top > constraint_bottom)
		{
			break;
		}

		// Top constraint
		if (iterator_glyph.top + iterator_glyph.height >= constraint_top)
		{
			// Set u, v values
			u1 = static_cast<float>(ms_FontData.Chars[iterator_glyph.chars_id].X) / static_cast<float>(ms_FontData.Common.ScaleW);
			u2 = u1 + static_cast<float>(ms_FontData.Chars[iterator_glyph.chars_id].Width) / static_cast<float>(ms_FontData.Common.ScaleW);

			v1 = static_cast<float>(ms_FontData.Chars[iterator_glyph.chars_id].Y) / static_cast<float>(ms_FontData.Common.ScaleH);
			v2 = v1 + static_cast<float>(ms_FontData.Chars[iterator_glyph.chars_id].Height) / static_cast<float>(ms_FontData.Common.ScaleH);

			// Set x, y values
			x1 = m_NonInstantTextOffset.x + iterator_glyph.left;
			x2 = x1 + iterator_glyph.width;

			y1 = m_NonInstantTextOffset.y + iterator_glyph.drawing_top;
			y2 = y1 + iterator_glyph.height;

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

void JWText::DrawNonInstantText() const noexcept
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
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, static_cast<int>(m_NonInstantVertexData.Vertices.size()),
		0, static_cast<int>(m_NonInstantIndexData.Indices.size()));

	// Set texture to nullptr.
	m_pDevice->SetTexture(0, nullptr);
}

void JWText::DrawInstantText(WSTRING SingleLineText, const D3DXVECTOR2& Position, EHorizontalAlignment HorizontalAlignment, DWORD FontColor) noexcept
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
	for (const auto& Char : SingleLineText)
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

	// Clear the vertex buffer's uv data & set font color.
	for (size_t iterator = 0; iterator < MAX_INSTANT_TEXT_VERTEX_SIZE; iterator++)
	{
		m_InstantVertexData.Vertices[iterator].u = 0;
		m_InstantVertexData.Vertices[iterator].v = 0;

		m_InstantVertexData.Vertices[iterator].color = FontColor;
	}

	// Convert each character of the text to Chars_ID in ms_FontData
	// in order to position them.
	iterator_character = 0;
	SGlyphInfo curr_char_info;
	SGlyphInfo prev_char_info;
	prev_char_info.left = offset_position.x;
	for (const auto& Char : SingleLineText)
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
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, static_cast<int>(m_InstantVertexData.Vertices.size()),
		0, static_cast<int>(m_InstantIndexData.Indices.size()));

	// Set texture to nullptr.
	m_pDevice->SetTexture(0, nullptr);
}

PRIVATE void JWText::UpdateCaret() noexcept
{
	if (m_pCaretLine)
	{
		m_CaretSelPosition = min(m_CaretSelPosition, m_NonInstantTextGlyphInfo.size() - 1);

		// This is needed in case m_ConstraintSize.x is smaller than DEFAULT_SIDE_CONSTRAINT_STRIDE.
		float default_side_stride = DEFAULT_SIDE_CONSTRAINT_STRIDE;
		default_side_stride = min(default_side_stride, m_ConstraintSize.x);

		// Left constraint
		float caret_x = m_NonInstantTextGlyphInfo[m_CaretSelPosition].left + m_NonInstantTextOffset.x;
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
		size_t current_line_index = m_NonInstantTextGlyphInfo[m_CaretSelPosition].line_index;
		if (caret_x > constraint_right)
		{
			float stride = caret_x - constraint_right;
			stride = max(stride, default_side_stride);

			m_NonInstantTextOffset.x -= stride;

			size_t line_end_glyph_index = GetLineEndGlyphIndex(current_line_index);
			float line_end_glyph_x = m_NonInstantTextGlyphInfo[line_end_glyph_index].left;

			m_NonInstantTextOffset.x = max(m_NonInstantTextOffset.x, constraint_right - line_end_glyph_x);
		}

		// Top constraint
		float caret_top = m_NonInstantTextGlyphInfo[m_CaretSelPosition].top;
		float constraint_top = m_ConstraintPosition.y;
		if (caret_top + m_NonInstantTextOffset.y <= constraint_top)
		{
			m_NonInstantTextOffset.y = constraint_top - caret_top;
		}

		// Bottom constraint
		float caret_bottom = m_NonInstantTextGlyphInfo[m_CaretSelPosition].top + m_CaretSize.y;
		float constraint_bottom = m_ConstraintPosition.y + m_ConstraintSize.y;
		if (caret_bottom + m_NonInstantTextOffset.y >= constraint_bottom)
		{
			m_NonInstantTextOffset.y = constraint_bottom - caret_bottom;
		}

		m_CaretPosition.x = m_NonInstantTextOffset.x + m_NonInstantTextGlyphInfo[m_CaretSelPosition].left;
		m_CaretPosition.y = m_NonInstantTextOffset.y + m_NonInstantTextGlyphInfo[m_CaretSelPosition].top;

		UpdateNonInstantTextVisibleVertices();

		m_pCaretLine->SetLine(0, m_CaretPosition, m_CaretSize);
	}
}

void JWText::DrawCaret() const noexcept
{
	if (m_pCaretLine)
	{
		m_pCaretLine->Draw();
	}
}

void JWText::DrawSelectionBox() const noexcept
{
	if (m_pSelectionBox)
	{
		m_pSelectionBox->Draw();
	}
}

PRIVATE void JWText::SetInstantTextGlyph(size_t Character_index, SGlyphInfo* pCurrInfo, const SGlyphInfo* pPrevInfo) noexcept
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

PRIVATE void JWText::SetNonInstantTextGlyph(SGlyphInfo* pCurrInfo, SGlyphInfo* pPrevInfo) noexcept
{
	pCurrInfo->width = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Width);
	pCurrInfo->height = static_cast<float>(ms_FontData.Chars[pCurrInfo->chars_id].Height);

	pCurrInfo->line_index = pPrevInfo->line_index;
	pCurrInfo->glyph_index_in_line = pPrevInfo->glyph_index_in_line + 1;

	if (pCurrInfo->glyph_index_in_line)
	{
		// This is NOT the first character of the text.
		pCurrInfo->left = pPrevInfo->left + ms_FontData.Chars[pPrevInfo->chars_id].XAdvance;
		pCurrInfo->top = pPrevInfo->top;
	}

	// Check previous '\n'.
	if (pPrevInfo->chars_id == 0)
	{
		pCurrInfo->line_index++;

		pCurrInfo->glyph_index_in_line = 0;

		pCurrInfo->left = m_ConstraintPosition.x;
		pCurrInfo->top = pPrevInfo->top + GetLineHeight();
	}

	// Check automatic line break.
	if (m_bUseAutomaticLineBreak)
	{
		if (pCurrInfo->left + pCurrInfo->width >= m_ConstraintPosition.x + m_ConstraintSize.x)
		{
			pCurrInfo->line_index++;

			pCurrInfo->glyph_index_in_line = 0;

			pCurrInfo->left = m_ConstraintPosition.x;
			pCurrInfo->top = pPrevInfo->top + GetLineHeight();
		}
	}

	pCurrInfo->drawing_top = pCurrInfo->top + ms_FontData.Chars[pCurrInfo->chars_id].YOffset;

	// Advance glyph.
	*pPrevInfo = *pCurrInfo;
}

auto JWText::GetFontTexturePtr() const noexcept->const LPDIRECT3DTEXTURE9
{
	return m_pFontTexture;
}

auto JWText::GetLineHeight() const noexcept->float
{
	return static_cast<float>(ms_FontData.Info.Size);
}

PRIVATE auto JWText::GetLineWidth(const WSTRING* pLineText) const noexcept->float
{
	size_t iterator_character = 0;
	size_t chars_id = 0;
	float width = 0;
	for (const auto& Char : *pLineText)
	{
		chars_id = GetCharsIDFromCharacter(Char);
		width += ms_FontData.Chars[chars_id].XAdvance;

		iterator_character++;
	}
	return width;
}

void JWText::MoveCaretToLeft() noexcept
{
	if (m_CaretSelPosition)
	{
		m_CaretSelPosition--;

		UpdateCaret();
	}
}

void JWText::MoveCaretToRight() noexcept
{
	m_CaretSelPosition++;

	UpdateCaret();
}

PRIVATE auto JWText::GetLineStartGlyphIndex(size_t LineIndex) const noexcept->size_t
{
	size_t result = SIZE_T_INVALID;

	if (LineIndex < m_NonInstantTextLineInfo.size())
	{
		result = m_NonInstantTextLineInfo[LineIndex].start_glyph_index;
	}

	return result;
}

PRIVATE auto JWText::GetLineEndGlyphIndex(size_t LineIndex) const noexcept->size_t
{
	size_t result = 0;

	if (LineIndex < m_NonInstantTextLineInfo.size())
	{
		result = m_NonInstantTextLineInfo[LineIndex].end_glyph_index;
	}

	return result;
}

void JWText::MoveCaretUp() noexcept
{
	size_t curr_index_in_line = m_NonInstantTextGlyphInfo[m_CaretSelPosition].glyph_index_in_line;
	size_t curr_line_index = m_NonInstantTextGlyphInfo[m_CaretSelPosition].line_index;
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

void JWText::MoveCaretDown() noexcept
{
	size_t curr_index_in_line = m_NonInstantTextGlyphInfo[m_CaretSelPosition].glyph_index_in_line;
	size_t curr_line_index = m_NonInstantTextGlyphInfo[m_CaretSelPosition].line_index;
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

void JWText::MoveCaretHome() noexcept
{
	size_t curr_line_index = m_NonInstantTextGlyphInfo[m_CaretSelPosition].line_index;
	m_CaretSelPosition = GetLineStartGlyphIndex(curr_line_index);

	UpdateCaret();
}

void JWText::MoveCaretEnd() noexcept
{
	size_t curr_line_index = m_NonInstantTextGlyphInfo[m_CaretSelPosition].line_index;
	m_CaretSelPosition = GetLineEndGlyphIndex(curr_line_index);

	UpdateCaret();
}

void JWText::MoveCaretTo(size_t SelPosition) noexcept
{
	m_CaretSelPosition = SelPosition;

	UpdateCaret();

	ReleaseSelection();
}

void JWText::SelectToLeft() noexcept
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}

	MoveCaretToLeft();

	UpdateSelectionBox();
}

void JWText::SelectToRight() noexcept
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}

	MoveCaretToRight();

	UpdateSelectionBox();
}

void JWText::SelectUp() noexcept
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}

	MoveCaretUp();

	UpdateSelectionBox();
}

void JWText::SelectDown() noexcept
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}
	
	MoveCaretDown();

	UpdateSelectionBox();
}

void JWText::SelectHome() noexcept
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}
	
	MoveCaretHome();

	UpdateSelectionBox();
}

void JWText::SelectEnd() noexcept
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}

	MoveCaretEnd();

	UpdateSelectionBox();
}

void JWText::SelectTo(size_t SelPosition) noexcept
{
	if (m_CapturedSelPosition == SIZE_T_INVALID)
	{
		m_CapturedSelPosition = m_CaretSelPosition;
	}

	m_CaretSelPosition = SelPosition;

	UpdateCaret();

	UpdateSelectionBox();
}

void JWText::SelectAll() noexcept
{
	m_CapturedSelPosition = 0;
	m_CaretSelPosition = m_NonInstantTextGlyphInfo.size() - 1;

	UpdateCaret();

	UpdateSelectionBox();
}

void JWText::ReleaseSelection() noexcept
{
	m_CapturedSelPosition = SIZE_T_INVALID;

	m_pSelectionBox->ClearAllRectangles();

	m_bIsTextSelected = false;
}

PRIVATE void JWText::UpdateSelectionBox() noexcept
{
	if (m_CapturedSelPosition != SIZE_T_INVALID)
	{
		m_bIsTextSelected = true;

		size_t sel_start = min(m_CapturedSelPosition, m_CaretSelPosition);
		size_t sel_end = max(m_CapturedSelPosition, m_CaretSelPosition);

		m_SelectionStart = sel_start;
		m_SelectionEnd = sel_end;

		size_t line_start = m_NonInstantTextGlyphInfo[sel_start].line_index;
		size_t line_end = m_NonInstantTextGlyphInfo[sel_end].line_index;

		D3DXVECTOR2 selection_position{ 0, 0 };
		D3DXVECTOR2 selection_size{ 0, 0 };
		selection_size.y = GetLineHeight();

		m_pSelectionBox->ClearAllRectangles();

		if (line_start == line_end)
		{
			// Single-line selection.

			selection_position.x = m_NonInstantTextOffset.x + m_NonInstantTextGlyphInfo[sel_start].left;
			selection_position.y = m_NonInstantTextOffset.y + m_NonInstantTextGlyphInfo[sel_start].top;

			selection_size.x = m_NonInstantTextGlyphInfo[sel_end].left - m_NonInstantTextGlyphInfo[sel_start].left;

			m_pSelectionBox->AddRectangle(selection_size, selection_position);
		}
		else
		{
			// Multi-line selection.

			size_t line_start_glyph = 0;
			size_t line_end_glyph = 0;
			float line_end_x = 0;

			for (size_t iterator_line = line_start; iterator_line <= line_end; iterator_line++)
			{
				line_start_glyph = GetLineStartGlyphIndex(iterator_line);
				line_end_glyph = GetLineEndGlyphIndex(iterator_line);
				line_end_x = m_NonInstantTextGlyphInfo[line_end_glyph].left;

				// Check if this is a automatically borken line
				// if it is, it doesn't end with '\0',
				// and we must selet the last character with its right position, not left.
				if (m_NonInstantTextGlyphInfo[line_end_glyph].chars_id != 0)
				{
					line_end_x += m_NonInstantTextGlyphInfo[line_end_glyph].width;
				}

				if (iterator_line == line_start)
				{
					// Select from sel_start to line_end_glyph.
					selection_position.x = m_NonInstantTextOffset.x + m_NonInstantTextGlyphInfo[sel_start].left;
					selection_position.y = m_NonInstantTextOffset.y + m_NonInstantTextGlyphInfo[sel_start].top;

					selection_size.x = line_end_x - m_NonInstantTextGlyphInfo[sel_start].left;
				}
				else if (iterator_line == line_end)
				{
					// Select from line_start_glyph to sel_end.
					selection_position.x = m_NonInstantTextOffset.x + m_NonInstantTextGlyphInfo[line_start_glyph].left;
					selection_position.y = m_NonInstantTextOffset.y + m_NonInstantTextGlyphInfo[line_start_glyph].top;

					selection_size.x = m_NonInstantTextGlyphInfo[sel_end].left - m_NonInstantTextGlyphInfo[line_start_glyph].left;
				}
				else
				{
					// IF,
					// this line isn't the start line nor the end line,
					// select the whole line.
					selection_position.x = m_NonInstantTextOffset.x + m_NonInstantTextGlyphInfo[line_start_glyph].left;
					selection_position.y = m_NonInstantTextOffset.y + m_NonInstantTextGlyphInfo[line_start_glyph].top;

					selection_size.x = line_end_x - m_NonInstantTextGlyphInfo[line_start_glyph].left;
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

auto JWText::GetCaretSelPosition() const noexcept->size_t
{
	return m_CaretSelPosition;
}

auto JWText::GetMousePressedSelPosition(const POINT MousePosition) const noexcept->size_t
{
	size_t result = 0;

	float cmp_left = 0;
	float cmp_top = 0;

	for (size_t iterator_glpyh = 0; iterator_glpyh < m_NonInstantTextGlyphInfo.size(); iterator_glpyh++)
	{
		cmp_left = m_NonInstantTextGlyphInfo[iterator_glpyh].left + m_NonInstantTextOffset.x;
		cmp_top = m_NonInstantTextGlyphInfo[iterator_glpyh].top + m_NonInstantTextOffset.y;

		if ((cmp_left <= MousePosition.x) && (cmp_top <= MousePosition.y))
		{
			result = iterator_glpyh;
		}
	}

	return result;
}

auto JWText::GetSelectionStart() const noexcept->size_t
{
	return m_SelectionStart;
}

auto JWText::GetSelectionEnd() const noexcept->size_t
{
	return m_SelectionEnd;
}

auto JWText::IsTextSelected() const noexcept->bool
{
	return m_bIsTextSelected;
}

void JWText::ShouldUseAutomaticLineBreak(bool Value) noexcept
{
	m_bUseAutomaticLineBreak = Value;
}