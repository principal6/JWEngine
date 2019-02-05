#include "JWFont.h"
#include "JWImage.h"
#include "JWWindow.h"

using namespace JWENGINE;

// Static member variable
LPDIRECT3DTEXTURE9 JWFont::ms_pFontTexture = nullptr;
const float JWFont::DEFAULT_BOUNDARY_STRIDE = 50.0f;

JWFont::JWFont()
{
	m_pJWWindow = nullptr;
	m_pBackgroundBox = nullptr;

	m_pDevice = nullptr;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;

	m_Vertices = nullptr;
	m_Indices = nullptr;

	m_VertexSize = 0;
	m_IndexSize = 0;
	m_MaximumCharacterCountInLine = 0;
	m_MaximumLineCount = 0;
	m_MaximumLetterBoxCount = 0;

	// Set default alignment
	m_HorizontalAlignment = EHorizontalAlignment::Left;
	m_VerticalAlignment = EVerticalAlignment::Top;

	// Set default colors
	m_FontColor = DEFAULT_COLOR_FONT;
	m_BoxColor = DEFAULT_COLOR_BOX;

	m_BoxPosition = D3DXVECTOR2(0, 0);
	m_BoxSize = D3DXVECTOR2(0, 0);

	m_bUseMultiline = false;

	m_CaretPosition = D3DXVECTOR2(0, 0);
	m_SinglelineXOffset = 0;
	m_MultilineYOffset = 0;
}

void JWFont::ClearText()
{
	m_ImageStringInfo.clear();
	m_ImageStringOriginalText.clear();
	m_LineLength.clear();

	if (m_Vertices)
	{
		for (size_t iterator = 0; iterator < (m_VertexSize / 4); iterator++)
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
	m_ImageStringAdjustedLength = 0;
	m_UsedLetterBoxCount = 0;
}

auto JWFont::Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir)->EError
{
	if (pJWWindow == nullptr)
		return EError::NULLPTR_WINDOW;

	m_pJWWindow = pJWWindow;
	m_pDevice = pJWWindow->GetDevice();
	m_pBaseDir = pBaseDir;

	// Create background box
	m_pBackgroundBox = new JWImage;
	m_pBackgroundBox->Create(m_pJWWindow, m_pBaseDir);

	return EError::OK;
}

void JWFont::Destroy()
{
	m_pDevice = nullptr;

	ClearText();

	JW_DELETE_ARRAY(m_Vertices);
	JW_DELETE_ARRAY(m_Indices);

	JW_DESTROY(m_pBackgroundBox);

	JW_RELEASE(ms_pFontTexture);
	JW_RELEASE(m_pIndexBuffer);
	JW_RELEASE(m_pVertexBuffer);
}

auto JWFont::MakeFont(WSTRING FileName_FNT)->EError
{
	WSTRING NewFileName;
	NewFileName = *m_pBaseDir;
	NewFileName += ASSET_DIR;
	NewFileName += FileName_FNT;

	if (!ms_FontData.bFontDataParsed)
	{
		// Font not yet parsed, then parse it
		// FontData is static, so we'll run this code only once per process
		if (Parse(NewFileName))
		{
			ms_FontData.bFontDataParsed = true;

			// JWFont will always use only one page in the BMFont, whose ID is '0'
			if (JW_FAILED(CreateTexture(ms_FontData.Pages[0].File)))
				return EError::TEXTURE_NOT_CREATED;
		}
	}

	// Create vertex and index buffer
	CreateMaxVertexIndexBuffer();
	
	return EError::FONT_NOT_CREATED;
}

PRIVATE auto JWFont::CreateMaxVertexIndexBuffer()->EError
{
	if (nullptr == (m_pWindowData = m_pJWWindow->GetWindowData()))
		return EError::NULLPTR_WINDOWDATA;

	UINT ScreenWidth = m_pWindowData->ScreenSize.x;
	UINT ScreenHeight = m_pWindowData->ScreenSize.y;

	UINT FullSize = ms_FontData.Info.Size;
	UINT HalfSize = ms_FontData.Info.Size / static_cast <UINT>(2);

	m_MaximumCharacterCountInLine = (ScreenWidth / HalfSize) + 1;
	m_MaximumLineCount = (ScreenHeight / FullSize) + 1;

	// Set letter-box count
	m_MaximumLetterBoxCount = m_MaximumCharacterCountInLine * m_MaximumLineCount;

	// Set maximum size
	m_VertexSize = m_MaximumLetterBoxCount * 4;
	m_IndexSize = m_MaximumLetterBoxCount * 2;

	// Make rectangles for characters with maximum size
	if (!m_Vertices)
	{
		m_Vertices = new SVertexImage[m_VertexSize];
		memset(m_Vertices, 0, sizeof(SVertexImage) * m_VertexSize);
		
		for (size_t iterator = 0; iterator < (m_VertexSize / 4); iterator++)
		{
			m_Vertices[iterator * 4] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 0);
			m_Vertices[iterator * 4 + 1] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 0);
			m_Vertices[iterator * 4 + 2] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 0, 1);
			m_Vertices[iterator * 4 + 3] = SVertexImage(0, 0, DEFAULT_COLOR_FONT, 1, 1);
		}
	}

	if (!m_Indices)
	{
		m_Indices = new SIndex3[m_IndexSize];

		for (size_t iterator = 0; iterator < (m_IndexSize / 2); iterator++)
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
		int temp_vertex_size = sizeof(SVertexImage) * m_VertexSize;
		if (FAILED(m_pDevice->CreateVertexBuffer(temp_vertex_size, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVertexBuffer, nullptr)))
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
		int temp_index_size = sizeof(SIndex3) * m_IndexSize;
		if (FAILED(m_pDevice->CreateIndexBuffer(temp_index_size, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, nullptr)))
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
		int temp_vertex_size = sizeof(SVertexImage) * m_VertexSize;
		VOID* pVertices;
		if (FAILED(m_pVertexBuffer->Lock(0, temp_vertex_size, (void**)&pVertices, 0)))
		{
			return EError::VERTEX_BUFFER_NOT_LOCKED;
		}
		memcpy(pVertices, &m_Vertices[0], temp_vertex_size);
		m_pVertexBuffer->Unlock();
		return EError::OK;
	}
	return EError::NULL_VERTEX;
}

PRIVATE auto JWFont::UpdateIndexBuffer()->EError
{
	if (m_Indices)
	{
		int temp_index_size = sizeof(SIndex3) * m_IndexSize;
		VOID* pIndices;
		if (FAILED(m_pIndexBuffer->Lock(0, temp_index_size, (void **)&pIndices, 0)))
		{
			return EError::INDEX_BUFFER_NOT_LOCKED;
		}
		memcpy(pIndices, &m_Indices[0], temp_index_size);
		m_pIndexBuffer->Unlock();
		return EError::OK;
	}
	return EError::NULL_INDEX;
}

PRIVATE auto JWFont::CreateTexture(WSTRING FileName)->EError
{
	if (ms_pFontTexture)
	{
		ms_pFontTexture->Release();
		ms_pFontTexture = nullptr;
	}

	WSTRING NewFileName;
	NewFileName = *m_pBaseDir;
	NewFileName += ASSET_DIR;
	NewFileName += FileName;

	// Craete texture without color key
	if (FAILED(D3DXCreateTextureFromFileExW(m_pDevice, NewFileName.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &ms_pFontTexture)))
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
	if (m_pBackgroundBox)
	{
		m_pBackgroundBox->SetAlpha(GetColorAlpha(m_BoxColor));
	}
}

void JWFont::SetBoxXRGB(DWORD XRGB)
{
	SetColorXRGB(&m_BoxColor, XRGB);
	if (m_pBackgroundBox)
	{
		m_pBackgroundBox->SetXRGB(GetColorXRGB(m_BoxColor));
	}
}

auto JWFont::SetText(WSTRING MultilineText, D3DXVECTOR2 Position, D3DXVECTOR2 BoxSize)->EError
{
	// Clear the text
	ClearText();

	// Save the original text
	m_ImageStringOriginalText = MultilineText;

	// Set backgrounnd box
	m_BoxPosition = Position;
	m_BoxSize = BoxSize;
	m_pBackgroundBox->SetPosition(m_BoxPosition);
	m_pBackgroundBox->SetSize(m_BoxSize);
	m_pBackgroundBox->SetAlpha(GetColorAlpha(m_BoxColor));
	m_pBackgroundBox->SetXRGB(GetColorXRGB(m_BoxColor));

	// Check if multiline is used
	if (!m_bUseMultiline)
	{
		// If multiline is not used, convert the text into single-line text
		size_t cmp_position = 0;
		while ((cmp_position = MultilineText.find(L'\n')) != std::string::npos)
		{
			MultilineText.replace(cmp_position, 1, L"");
		}
	}

	// Parse MultilineText into line_string
	WSTRING line_string;
	float line_width = 0;
	size_t line_character_index = 0;
	size_t line_count = 0;
	size_t iterator_in_text_prev = 0;
	bool b_split_end_line = false;
	bool b_should_add_line = false;
	for (size_t iterator_in_text = 0; iterator_in_text <= MultilineText.length(); iterator_in_text++)
	{
		line_string = MultilineText.substr(iterator_in_text_prev, iterator_in_text - iterator_in_text_prev);
		line_character_index = iterator_in_text - iterator_in_text_prev;
		line_width = 0;
		b_should_add_line = false;
		b_split_end_line = false;

		if (m_bUseMultiline)
		{
			if (CalculateLineWidth(line_string) >= BoxSize.x)
			{
				// If current character's x position exceeds the box's x size
				b_should_add_line = true;
				b_split_end_line = true;

				line_string = MultilineText.substr(iterator_in_text_prev, iterator_in_text - 1 - iterator_in_text_prev);
				line_character_index = iterator_in_text - iterator_in_text_prev - 1;
				line_width = CalculateLineWidth(line_string);

				m_LineLength.push_back(line_string.length() + 1); // includes '\0'

				iterator_in_text--;
				iterator_in_text_prev = iterator_in_text;

				line_count++;
			}
			else if ((MultilineText[iterator_in_text] == L'\n') || (iterator_in_text == MultilineText.length()))
			{
				// If we meet new line character or end of the text
				b_should_add_line = true;
				line_width = CalculateLineWidth(line_string);

				m_LineLength.push_back(line_string.length() + 1); // includes '\n' or '\0'!

				iterator_in_text_prev = iterator_in_text + 1;
				line_count++;
			}
		}
		else
		{
			if (iterator_in_text == MultilineText.length())
			{
				b_should_add_line = true;

				line_width = CalculateLineWidth(line_string);

				m_LineLength.push_back(line_string.length() + 1); // includes '\n' or '\0'!

				line_count++;
			}
		}

		if (b_should_add_line)
		{
			// Set vertical alignment offset (y position)
			float VerticalAlignmentOffset = Position.y;
			switch (m_VerticalAlignment)
			{
			case JWENGINE::EVerticalAlignment::Top:
				break;
			case JWENGINE::EVerticalAlignment::Middle:
				VerticalAlignmentOffset += (BoxSize.y - GetLineYPosition(line_count - 1) - GetLineHeight()) / 2.0f;
				break;
			case JWENGINE::EVerticalAlignment::Bottom:
				VerticalAlignmentOffset += BoxSize.y - GetLineYPosition(line_count - 1) - GetLineHeight();
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
				HorizontalAlignmentOffset += BoxSize.x / 2.0f - line_width / 2.0f;
				break;
			case JWENGINE::EHorizontalAlignment::Right:
				HorizontalAlignmentOffset += BoxSize.x - line_width;
				break;
			default:
				break;
			}

			// Make text images from the text string
			wchar_t Character = 0;
			size_t Chars_ID = 0;
			size_t Chars_ID_prev = 0;
			for (size_t iterator_in_line = 0; iterator_in_line <= line_string.length(); iterator_in_line++)
			{
				// Get Chars_ID from MappedCharacters
				Character = line_string[iterator_in_line];
				Chars_ID = ms_FontData.MappedCharacters[Character];

				if (iterator_in_line == line_string.length())
				{
					if (!b_split_end_line)
					{
						Character = L'\n';
					}
				}

				// Add wchar_t to the image string
				AddChar(Character, iterator_in_line, line_string, line_count - 1,
					Chars_ID, Chars_ID_prev, HorizontalAlignmentOffset, VerticalAlignmentOffset);

				Chars_ID_prev = Chars_ID;
			}
		}
	}

	UpdateLetterBoxes();

	return EError::OK;
}

auto JWFont::GetTextLength() const->size_t
{
	return m_ImageStringLength - 1;
}

void JWFont::SetUseMultiline(bool Value)
{
	m_bUseMultiline = Value;
}

auto JWFont::GetUseMultiline() const->bool
{
	return m_bUseMultiline;
}

auto JWFont::GetCharIndexByMousePosition(POINT Position) const->size_t
{
	size_t Result = 0;

	// First, find the line index with Mouse's y position
	size_t current_line_index = 0;
	for (size_t iterator = 0; iterator < m_ImageStringLength; iterator++)
	{
		if (m_ImageStringInfo[iterator].LineTop <= Position.y)
		{
			current_line_index = m_ImageStringInfo[iterator].LineIndex;
		}
	}

	// Now, find the right character with Mouse's x position
	for (size_t iterator = 0; iterator < m_ImageStringLength; iterator++)
	{
		if (m_ImageStringInfo[iterator].LineIndex == current_line_index)
		{
			if (m_ImageStringInfo[iterator].Left + m_SinglelineXOffset  <= Position.x)
			{
				Result = iterator;
			}
		}
	}

	return Result;
}

PRIVATE auto JWFont::CalculateCharPositionTop(size_t Chars_ID, size_t LineIndex) const->float
{
	return (LineIndex * GetLineHeight() + ms_FontData.Chars[Chars_ID].YOffset);
}

PRIVATE auto JWFont::CalculateCharPositionLeftInLine(size_t CharIndex, const WSTRING& LineText) const->float
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

	size_t Chars_ID = 0;
	size_t Chars_ID_prev = 0;

	for (size_t iterator = 0; iterator <= CharIndex; iterator++)
	{
		// Get Chars_ID from MappedCharacters
		Chars_ID = ms_FontData.MappedCharacters[LineText[iterator]];

		if (iterator)
		{
			Result += ms_FontData.Chars[Chars_ID_prev].XAdvance;
			Result += ms_FontData.Chars[Chars_ID].XOffset;
		}
		
		Chars_ID_prev = Chars_ID;
	}

	return Result;
}

PRIVATE auto JWFont::CalculateCharPositionRightInLine(size_t CharIndex, const WSTRING& LineText) const->float
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

	size_t Chars_ID = 0;
	size_t Chars_ID_prev = 0;

	for (size_t iterator = 0; iterator <= CharIndex; iterator++)
	{
		// Get Chars_ID from MappedCharacters
		Chars_ID = ms_FontData.MappedCharacters[LineText[iterator]];

		if (iterator)
		{
			Result += ms_FontData.Chars[Chars_ID_prev].XAdvance;
			Result += ms_FontData.Chars[Chars_ID].XOffset;
		}

		Chars_ID_prev = Chars_ID;
	}
	
	return Result;
}

auto JWFont::GetCharXPosition(size_t CharIndex) const->float
{
	float Result = 0;

	if (m_ImageStringInfo.size())
	{
		CharIndex = min(CharIndex, m_ImageStringInfo.size() - 1);

		if (!m_ImageStringInfo[CharIndex].Character)
		{
			Result = m_ImageStringInfo[CharIndex - 1].Right;
		}
		else
		{
			Result = m_ImageStringInfo[CharIndex].Left;
		}
	}

	return Result;
}

auto JWFont::GetCharXPositionInBox(size_t CharIndex) const->float
{
	CharIndex = min(CharIndex, m_ImageStringInfo.size() - 1);

	float Result = GetCharXPosition(CharIndex) + m_SinglelineXOffset;

	if (Result < m_BoxPosition.x)
		Result = m_BoxPosition.x;

	if (Result > m_BoxPosition.x + m_BoxSize.x)
		Result = m_BoxPosition.x + m_BoxSize.x;

	return Result;
}

auto JWFont::GetCharYPosition(size_t CharIndex) const->float
{
	float Result = 0;

	if (m_ImageStringInfo.size())
	{
		CharIndex = min(CharIndex, m_ImageStringInfo.size() - 1);

		Result = m_ImageStringInfo[CharIndex].LineTop;
	}

	return Result;
}

auto JWFont::GetCharYPositionInBox(size_t CharIndex) const->float
{
	CharIndex = min(CharIndex, m_ImageStringInfo.size() - 1);

	float Result = GetCharYPosition(CharIndex) + m_MultilineYOffset;

	if (Result < m_BoxPosition.y)
		Result = m_BoxPosition.y;

	if (Result > m_BoxPosition.y + m_BoxSize.y)
		Result = m_BoxPosition.y + m_BoxSize.y;

	return Result;
}

auto JWFont::GetCharacter(size_t CharIndex) const->wchar_t
{
	if (m_ImageStringInfo.size())
	{
		CharIndex = min(CharIndex, m_ImageStringInfo.size() - 1);

		return m_ImageStringInfo[CharIndex].Character;
	}

	return 0;
}

PRIVATE auto JWFont::CalculateLineWidth(const WSTRING& LineText)->float
{
	return CalculateCharPositionRightInLine(LineText.length(), LineText);
}

auto JWFont::GetLineLengthByCharIndex(size_t CharIndex) const->size_t
{
	if (m_LineLength.size())
	{
		CharIndex = min(CharIndex, m_ImageStringInfo.size() - 1);

		return m_LineLength[m_ImageStringInfo[CharIndex].LineIndex];
	}

	return 0;
}

auto JWFont::GetLineLength(size_t LineIndex) const->size_t
{
	if (LineIndex < m_LineLength.size())
	{
		LineIndex = min(LineIndex, m_LineLength.size() - 1);

		return m_LineLength[LineIndex];
	}

	return 0;
}

auto JWFont::GetLineSelPositionByCharIndex(size_t CharIndex) const->size_t
{
	CharIndex = min(CharIndex, m_ImageStringInfo.size() - 1);

	return m_ImageStringInfo[CharIndex].CharIndexInLine;
}

auto JWFont::GetLineIndexByCharIndex(size_t CharIndex) const->size_t
{
	CharIndex = min(CharIndex, m_ImageStringInfo.size() - 1);

	return m_ImageStringInfo[CharIndex].LineIndex;
}

auto JWFont::GetLineCount() const->size_t
{
	return m_LineLength.size();
}

auto JWFont::GetLineYPositionByCharIndex(size_t CharIndex) const->float
{
	if (m_LineLength.size())
	{
		CharIndex = min(CharIndex, m_ImageStringInfo.size() - 1);

		return m_ImageStringInfo[CharIndex].LineTop + m_MultilineYOffset;
	}

	return 0;
}

auto JWFont::GetLineYPosition(size_t LineIndex) const->float
{
	return static_cast<float>(LineIndex * ms_FontData.Info.Size + m_MultilineYOffset);
}

auto JWFont::GetLineWidth(size_t LineIndex) const->float
{
	float Result = 0;

	if (!IsTextEmpty())
	{
		size_t sel_end = GetLineGlobalSelEnd(LineIndex);
		Result = GetCharXPosition(sel_end) - m_BoxPosition.x;
	}

	return Result;
}

auto JWFont::GetLineWidthByCharIndex(size_t CharIndex) const->float
{
	float Result = 0;
	
	if (!IsTextEmpty())
	{
		size_t line_index = GetLineIndexByCharIndex(CharIndex);
		Result = GetLineWidth(line_index);
	}
	
	return Result;
}

auto JWFont::GetLineHeight() const->float
{
	return static_cast<float>(ms_FontData.Info.Size);
}

auto JWFont::GetLineGlobalSelStart(size_t LineIndex) const->size_t
{
	size_t Result = 0;

	if (!IsTextEmpty())
	{
		for (size_t iterator = 0; iterator < m_ImageStringLength; iterator++)
		{
			if (m_ImageStringInfo[iterator].LineIndex == LineIndex)
			{
				Result = iterator;
				break;
			}
		}
	}

	return Result;
}

auto JWFont::GetLineGlobalSelEnd(size_t LineIndex) const->size_t
{
	size_t Result = 0;

	if (!IsTextEmpty())
	{
		for (size_t iterator = 0; iterator < m_ImageStringLength; iterator++)
		{
			if (m_ImageStringInfo[iterator].LineIndex == LineIndex)
			{
				Result = iterator;
			}
		}
	}

	return Result;
}

auto JWFont::GetMaximumLineCount() const->const UINT
{
	return m_MaximumLineCount;
}

void JWFont::UpdateCaretPosition(size_t CaretSelPosition, D3DXVECTOR2* InOutPtrCaretPosition)
{
	InOutPtrCaretPosition->x = GetCharXPosition(CaretSelPosition);
	InOutPtrCaretPosition->y = GetCharYPosition(CaretSelPosition);

	float x_difference = InOutPtrCaretPosition->x - m_CaretPosition.x;
	float y_difference = InOutPtrCaretPosition->y - m_CaretPosition.y;
	m_CaretPosition = *InOutPtrCaretPosition;

	float temp_x_offset = m_SinglelineXOffset;
	float temp_y_offset = m_MultilineYOffset;
	
	UpdateSinglelineXOffset(x_difference);
	UpdateMultilineYOffset(y_difference);

	if ((temp_x_offset != m_SinglelineXOffset) || (temp_y_offset != m_MultilineYOffset))
	{
		UpdateLetterBoxes();
	}

	InOutPtrCaretPosition->x = GetCharXPositionInBox(CaretSelPosition);
	InOutPtrCaretPosition->y = GetCharYPositionInBox(CaretSelPosition);
}

PRIVATE void JWFont::UpdateSinglelineXOffset(float x_difference)
{
	if (!m_bUseMultiline)
	{
		float temp_caret_position_x = m_CaretPosition.x - m_BoxPosition.x;
		float compare_value = temp_caret_position_x + m_SinglelineXOffset;

		float line_width = GetLineWidth(0);
		float invisible_width = line_width - m_BoxSize.x;
		if (compare_value > m_BoxSize.x)
		{
			// Moving to right
			//m_SinglelineXOffset = box_width - temp_caret_position_x;
			if (x_difference < DEFAULT_BOUNDARY_STRIDE)
			{
				m_SinglelineXOffset -= DEFAULT_BOUNDARY_STRIDE;
			}
			else
			{
				m_SinglelineXOffset -= x_difference;
			}

			if (m_SinglelineXOffset < -invisible_width)
			{
				m_SinglelineXOffset = -invisible_width;
			}
		}
		else if (compare_value < 0)
		{
			// Moving to left
			//m_SinglelineXOffset = -temp_caret_position_x;
			if ((-x_difference) < DEFAULT_BOUNDARY_STRIDE)
			{
				m_SinglelineXOffset += DEFAULT_BOUNDARY_STRIDE;
			}
			else
			{
				m_SinglelineXOffset += (-x_difference);
			}

			if (m_SinglelineXOffset > 0)
			{
				m_SinglelineXOffset = 0;
			}
		}
	}
	else
	{
		m_SinglelineXOffset = 0;
	}
}

PRIVATE void JWFont::UpdateMultilineYOffset(float y_difference)
{
	if (m_bUseMultiline)
	{
		float temp_caret_position_y = m_CaretPosition.y - m_BoxPosition.y;
		float compare_value = temp_caret_position_y + m_MultilineYOffset;

		if (y_difference != 0)
		{
			if (y_difference > 0)
			{
				// Moving down
				compare_value += GetLineHeight();
			}
			else
			{
				// Moving up
				//compare_value -= GetLineHeight();
			}

			if (compare_value < 0)
			{
				m_MultilineYOffset -= compare_value;
			}
			else if (compare_value > m_BoxSize.y)
			{
				m_MultilineYOffset += (m_BoxSize.y - compare_value);
			}
		}
	}
	else
	{
		m_MultilineYOffset = 0;
	}
}

auto JWFont::GetAdjustedSelPosition(size_t SelPosition) const->size_t
{
	SelPosition = min(SelPosition, m_ImageStringInfo.size() - 1);
	return m_ImageStringInfo[SelPosition].AdjustedCharIndex;
}

PRIVATE auto JWFont::IsTextEmpty() const->bool
{
	if (m_ImageStringOriginalText.length())
	{
		return false;
	}

	return true;
}

PRIVATE void JWFont::AddChar(wchar_t Character, size_t CharIndexInLine, WSTRING& LineText, size_t LineIndex,
	size_t Chars_ID, size_t Chars_ID_prev, float HorizontalAlignmentOffset, float VerticalAlignmentOffset)
{
	// Set u, v values
	float u1 = static_cast<float>(ms_FontData.Chars[Chars_ID].X) / static_cast<float>(ms_FontData.Common.ScaleW);
	float u2 = u1 + static_cast<float>(ms_FontData.Chars[Chars_ID].Width) / static_cast<float>(ms_FontData.Common.ScaleW);

	float v1 = static_cast<float>(ms_FontData.Chars[Chars_ID].Y) / static_cast<float>(ms_FontData.Common.ScaleH);
	float v2 = v1 + static_cast<float>(ms_FontData.Chars[Chars_ID].Height) / static_cast<float>(ms_FontData.Common.ScaleH);

	// Set x, y positions
	float x1 = HorizontalAlignmentOffset + CalculateCharPositionLeftInLine(CharIndexInLine, LineText);
	float x2 = x1 + static_cast<float>(ms_FontData.Chars[Chars_ID].Width);

	float y1 = VerticalAlignmentOffset + CalculateCharPositionTop(Chars_ID, LineIndex);
	float y2 = y1 + static_cast<float>(ms_FontData.Chars[Chars_ID].Height);

	// If character is not '\0', adjusted length + 1
	if (Character)
		m_ImageStringAdjustedLength++;

	// Set text info
	STextInfo temp_info;
	temp_info.Character = Character;
	temp_info.Left = x1;
	temp_info.Right = x2;
	temp_info.Top = y1;
	temp_info.Bottom = y2;
	temp_info.LineTop = y1 - ms_FontData.Chars[Chars_ID].YOffset;
	temp_info.U1 = u1;
	temp_info.U2 = u2;
	temp_info.V1 = v1;
	temp_info.V2 = v2;
	temp_info.LineIndex = LineIndex;
	temp_info.CharIndexInLine = CharIndexInLine;
	temp_info.AdjustedCharIndex = m_ImageStringAdjustedLength - 1;
	m_ImageStringInfo.push_back(temp_info);

	// Image string length includes '\0' in the splitted text
	m_ImageStringLength++;
}

PRIVATE void JWFont::UpdateLetterBoxes()
{
	m_UsedLetterBoxCount = 0;

	float x1 = 0;
	float x2 = 0;
	float y1 = 0;
	float y2 = 0;

	float u1 = 0;
	float u2 = 0;
	float v1 = 0;
	float v2 = 0;

	float screen_width = static_cast<float>(m_pWindowData->ScreenSize.x);
	float screen_height = static_cast<float>(m_pWindowData->ScreenSize.y);

	memset(m_Vertices, 0, sizeof(SVertexImage) * m_VertexSize);

	for (size_t iterator = 0; iterator < m_ImageStringLength; iterator++)
	{
		// Set vertices only when it's inside the screen region
		// otherwise, skip the vertices setting
		x1 = m_ImageStringInfo[iterator].Left + m_SinglelineXOffset;
		x2 = m_ImageStringInfo[iterator].Right + m_SinglelineXOffset;
		y1 = m_ImageStringInfo[iterator].Top + m_MultilineYOffset;
		y2 = m_ImageStringInfo[iterator].Bottom + m_MultilineYOffset;

		u1 = m_ImageStringInfo[iterator].U1;
		u2 = m_ImageStringInfo[iterator].U2;
		v1 = m_ImageStringInfo[iterator].V1;
		v2 = m_ImageStringInfo[iterator].V2;

		if ((x2 >= 0) && (x1 <= screen_width))
		{
			if ((y2 >= 0) && (y2 <= screen_height))
			{
				size_t vertexID = m_UsedLetterBoxCount * 4;
				/*
				m_Vertices[vertexID] = SVertexImage(x1, y1, m_FontColor, u1, v1);
				m_Vertices[vertexID + 1] = SVertexImage(x2, y1, m_FontColor, u2, v1);
				m_Vertices[vertexID + 2] = SVertexImage(x1, y2, m_FontColor, u1, v2);
				m_Vertices[vertexID + 3] = SVertexImage(x2, y2, m_FontColor, u2, v2);
				*/
				m_Vertices[vertexID] = { x1, y1, 0, 1, m_FontColor, u1, v1 };
				m_Vertices[vertexID + 1] = { x2, y1, 0, 1, m_FontColor, u2, v1 };
				m_Vertices[vertexID + 2] = { x1, y2, 0, 1, m_FontColor, u1, v2 };
				m_Vertices[vertexID + 3] = { x2, y2, 0, 1, m_FontColor, u2, v2 };

				m_UsedLetterBoxCount++;
			}
		}
	}

	UpdateVertexBuffer();
}

void JWFont::Draw() const
{
	// Draw background box
	if (m_pBackgroundBox)
	{
		m_pBackgroundBox->Draw();
	}

	// Draw text when it is
	if (m_ImageStringOriginalText.length())
	{
		// Set alpha blending on
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

		if (ms_pFontTexture)
		{
			// Texture exists
			m_pDevice->SetTexture(0, ms_pFontTexture);

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

		// Draw call
		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_VertexSize, 0, m_IndexSize);

		m_pDevice->SetTexture(0, nullptr);
	}
}
