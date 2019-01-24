#include "../CoreBase/JWWindow.h"
#include "../MapEditor/JWTileMapSelector.h"
#include "JWMap.h"

using namespace JWENGINE;

// Static member variables declaration
const int JWMap::MAX_LINE_LEN = 1024;
const int JWMap::MAX_TILEID_LEN = 3;
const int JWMap::MAX_MOVEID_LEN = 2;
const int JWMap::MOVE_ALPHA = 100;
const int JWMap::DEF_TILE_SIZE = 32;
const int JWMap::DEPTH_HELL = 10;
const wchar_t* JWMap::MOVE_32 = L"move32.png";
const wchar_t* JWMap::MOVE_64 = L"move64.png";

/*-----------------------------------------------------------------------------
	Static method declaration
-----------------------------------------------------------------------------*/
auto JWMap::ConvertIDtoUV(int ID, int TileSize, D3DXVECTOR2 SheetSize)->STextureUV
{
	STextureUV Result;
	int tTileCols, tTileRows;
	GetTileCols(static_cast<int>(SheetSize.x), TileSize, &tTileCols);
	GetTileRows(static_cast<int>(SheetSize.y), TileSize, &tTileRows);

	if (ID == -1)
	{
		Result = STextureUV(0, 0, 0, 0);
	}
	else
	{
		ConvertFrameIDIntoUV(ID, D3DXVECTOR2{ static_cast<float>(TileSize), static_cast<float>(TileSize) },
			SheetSize, tTileCols, tTileRows, &Result);
	}

	return Result;
}

auto JWMap::ConvertIDToXY(int ID, int Cols)->D3DXVECTOR2
{
	D3DXVECTOR2 Result = D3DXVECTOR2(0, 0);

	Result.x = static_cast<float>(ID % Cols);
	Result.y = static_cast<float>(ID / Cols);

	return Result;
}

auto JWMap::ConvertXYToID(D3DXVECTOR2 XY, int Cols)->int
{
	return static_cast<int>(XY.x) + (static_cast<int>(XY.y) * Cols);
}

auto JWMap::ConvertXYToID(POINT XY, int Cols)->int
{
	return XY.x + (XY.y * Cols);
}

auto JWMap::ConvertPositionToXY(D3DXVECTOR2 Position, D3DXVECTOR2 Offset, int TileSize, bool YRoundUp)->D3DXVECTOR2
{
	D3DXVECTOR2 Result;

	float tX = -Offset.x + Position.x;
	float tY = -Offset.y + Position.y;

	int tYRemain = static_cast<int>(tY) % TileSize;

	int tMapX = static_cast<int>(tX / TileSize);
	int tMapY = static_cast<int>(tY / TileSize);

	if (YRoundUp)
	{
		// @warning: round-up the Y value (If it gets a little bit down, it should be recognized as in the next row)
		if (tYRemain)
			tMapY++;
	}

	Result.x = static_cast<float>(tMapX);
	Result.y = static_cast<float>(tMapY);

	return Result;
}

/*-----------------------------------------------------------------------------
	Non-static method declaration
-----------------------------------------------------------------------------*/
JWMap::JWMap()
{
	m_CurrMapMode = EMapMode::TileMode;
	m_bMapExist = false;

	m_MapInfo.TileSize = DEF_TILE_SIZE;
	m_MapInfo.TileSheetSize = D3DXVECTOR2(0, 0);
	m_MapInfo.MoveSheetSize = D3DXVECTOR2(0, 0);

	m_bMoveTextureLoaded = false;
	m_pTextureMove = nullptr;
	m_pVBMove = nullptr;

	m_Offset = D3DXVECTOR2(0, 0);
	m_OffsetZeroY = 0;
}

auto JWMap::Create(JWWindow* pJWWindow, WSTRING BaseDir)->EError
{
	if (pJWWindow == nullptr)
		return EError::NULLPTR_WINDOW;

	m_pJWWindow = pJWWindow;
	m_pDevice = m_pJWWindow->GetDevice();
	m_BaseDir = BaseDir;

	ClearAllData();
	m_Vertices.clear();
	m_Indices.clear();

	return EError::OK;
}

PRIVATE void JWMap::ClearAllData()
{
	JWImage::ClearVertexAndIndexData();

	m_VertMove.clear();
	m_MapData.clear();
}

void JWMap::Destroy()
{
	DX_RELEASE(m_pTextureMove);
	DX_RELEASE(m_pVBMove);

	JWImage::Destroy();
}

void JWMap::CreateMap(SMapInfo* InPtr_Info)
{
	m_MapInfo = *InPtr_Info;

	MakeMapBase();

	for (int i = 0; i < m_MapInfo.MapRows; i++)
	{
		for (int j = 0; j < m_MapInfo.MapCols; j++)
		{
			AddMapFragmentTile(-1, j, i);
			AddMapFragmentMove(0, j, i);
			m_MapData.push_back(SMapData(-1, 0));
		}
	}
	AddEnd();
}

void JWMap::LoadMap(WSTRING FileName)
{
	WSTRING NewFileName;
	NewFileName = m_BaseDir;
	NewFileName += ASSET_DIR;
	NewFileName += FileName;

	WIFSTREAM filein;
	filein.open(NewFileName, WIFSTREAM::in);

	if (!filein.is_open())
		return;

	WSTRING fileText;
	wchar_t tempText[MAX_LINE_LEN];
	while (!filein.eof())
	{
		filein.getline(tempText, MAX_LINE_LEN);
		fileText += tempText;
		fileText += '\n';
	}
	fileText = fileText.substr(0, fileText.size() - 1);

	// Parse the loaded map data
	ParseLoadedMapData(&fileText);

	// Create map with loaded data
	MakeLoadedMap(&fileText);

	filein.close();
}

PRIVATE void JWMap::ParseLoadedMapData(WSTRING* InoutPtr_WSTRING)
{
	size_t tempFind = -1;
	int tempInt = 0;
	tempFind = InoutPtr_WSTRING->find_first_of('#');
	if (tempFind)
	{
		m_MapInfo.MapName = InoutPtr_WSTRING->substr(0, tempFind);
		*InoutPtr_WSTRING = InoutPtr_WSTRING->substr(tempFind + 1);
	}

	tempFind = InoutPtr_WSTRING->find_first_of('#');
	if (tempFind)
	{
		tempInt = _wtoi(InoutPtr_WSTRING->substr(0, tempFind).c_str());
		m_MapInfo.TileSize = tempInt;
		*InoutPtr_WSTRING = InoutPtr_WSTRING->substr(tempFind + 1);
	}

	tempFind = InoutPtr_WSTRING->find_first_of('#');
	if (tempFind)
	{
		tempInt = _wtoi(InoutPtr_WSTRING->substr(0, tempFind).c_str());
		m_MapInfo.MapCols = tempInt;
		*InoutPtr_WSTRING = InoutPtr_WSTRING->substr(tempFind + 1);
	}

	tempFind = InoutPtr_WSTRING->find_first_of('#');
	if (tempFind)
	{
		tempInt = _wtoi(InoutPtr_WSTRING->substr(0, tempFind).c_str());
		m_MapInfo.MapRows = tempInt;
		*InoutPtr_WSTRING = InoutPtr_WSTRING->substr(tempFind + 1);
	}

	tempFind = InoutPtr_WSTRING->find_first_of('#');
	if (tempFind)
	{
		m_MapInfo.TileSheetName = InoutPtr_WSTRING->substr(0, tempFind);
		*InoutPtr_WSTRING = InoutPtr_WSTRING->substr(tempFind + 2);
	}
}

// @warning: this fuction must be called in LoadMap()
PRIVATE void JWMap::MakeLoadedMap(WSTRING* InoutPtr_WSTRING)
{
	MakeMapBase();

	int tTileID = 0;
	int tMoveID = 0;
	for (int i = 0; i < m_MapInfo.MapRows; i++)
	{
		for (int j = 0; j < m_MapInfo.MapCols; j++)
		{
			tTileID = _wtoi(InoutPtr_WSTRING->substr(0, MAX_TILEID_LEN).c_str());
			if (tTileID == 999)
				tTileID = -1;

			tMoveID = _wtoi(InoutPtr_WSTRING->substr(MAX_TILEID_LEN, MAX_MOVEID_LEN).c_str());

			AddMapFragmentTile(tTileID, j, i);
			AddMapFragmentMove(tMoveID, j, i);
			m_MapData.push_back(SMapData(tTileID, tMoveID));

			*InoutPtr_WSTRING = InoutPtr_WSTRING->substr(MAX_TILEID_LEN);
			*InoutPtr_WSTRING = InoutPtr_WSTRING->substr(MAX_MOVEID_LEN);
		}
		*InoutPtr_WSTRING = InoutPtr_WSTRING->substr(1); // Delete '\n' in the string data
	}
	InoutPtr_WSTRING->clear();
	AddEnd();
}

void JWMap::SaveMap(WSTRING FileName)
{
	WOFSTREAM fileout;
	fileout.open(FileName, WOFSTREAM::out);

	if (!fileout.is_open())
		return;

	WSTRING FileText;
	GetMapDataForSave(&FileText);

	fileout.write(FileText.c_str(), FileText.size());

	fileout.close();
}

PRIVATE void JWMap::GetMapDataForSave(WSTRING *OutPtr_WSTRING) const
{
	wchar_t tempWC[255] = { 0 };
	*OutPtr_WSTRING = m_MapInfo.MapName;
	*OutPtr_WSTRING += L'#';
	_itow_s(m_MapInfo.TileSize, tempWC, 10);
	*OutPtr_WSTRING += tempWC;
	*OutPtr_WSTRING += L'#';
	_itow_s(m_MapInfo.MapCols, tempWC, 10);
	*OutPtr_WSTRING += tempWC;
	*OutPtr_WSTRING += L'#';
	_itow_s(m_MapInfo.MapRows, tempWC, 10);
	*OutPtr_WSTRING += tempWC;
	*OutPtr_WSTRING += L'#';
	*OutPtr_WSTRING += m_MapInfo.TileSheetName;
	*OutPtr_WSTRING += L'#';
	*OutPtr_WSTRING += L'\n';

	int tDataID = 0;
	for (int i = 0; i < m_MapInfo.MapRows; i++)
	{
		for (int j = 0; j < m_MapInfo.MapCols; j++)
		{
			tDataID = j + (i * m_MapInfo.MapCols);
			GetMapDataPartForSave(tDataID, tempWC, 255);
			*OutPtr_WSTRING += tempWC;
		}

		if (i < m_MapInfo.MapRows) // To avoid '\n' at the end
			*OutPtr_WSTRING += L'\n';
	}
}

PRIVATE void JWMap::GetMapDataPartForSave(int DataID, wchar_t* OutPtr_wchar, int size) const
{
	WSTRING tempStr;
	wchar_t tempWC[255] = { 0 };

	int tTileID = m_MapData[DataID].TileID;
	if (tTileID == -1) tTileID = 999;
	_itow_s(tTileID, tempWC, 10);
	size_t tempLen = wcslen(tempWC);

	switch (tempLen)
	{
	case 1:
		tempStr = L"00";
		tempStr += tempWC;
		break;
	case 2:
		tempStr = L"0";
		tempStr += tempWC;
		break;
	case 3:
		tempStr = tempWC;
		break;
	default:
		// @warning: Invalid length
		return;
	}

	int tMoveID = m_MapData[DataID].MoveID;
	_itow_s(tMoveID, tempWC, 10);
	tempLen = wcslen(tempWC);

	switch (tempLen)
	{
	case 1:
		tempStr += L"0";
		tempStr += tempWC;
		break;
	case 2:
		tempStr += tempWC;
		break;
	default:
		// @warning: Invalid length
		return;
	}

	wcscpy_s(OutPtr_wchar, size, tempStr.c_str());
}

void JWMap::EditMap(const JWTileMapSelector* InPtr_Selector, bool bErase)
{
	POINT TilePos = InPtr_Selector->GetTileSelectorPositionInCells();
	POINT MapPos = InPtr_Selector->GetMapSelectorPositionInCells();
	POINT Size = InPtr_Selector->GetSelectionSizeInCells();

	int TileID = 0;
	POINT iter_map_pos{ 0, 0 };
	POINT iter_tile_pos{ 0, 0 };
	for (int iter_x = 0; iter_x <= Size.x; iter_x++)
	{
		for (int iter_y = 0; iter_y <= Size.y; iter_y++)
		{
			iter_map_pos.x = MapPos.x + iter_x;
			iter_map_pos.y = MapPos.y + iter_y;

			iter_tile_pos.x = TilePos.x + iter_x;
			iter_tile_pos.y = TilePos.y + iter_y;
			TileID = ConvertXYToID(iter_tile_pos, m_MapInfo.TileSheetCols);

			if (bErase)
				TileID = -1;

			switch (m_CurrMapMode)
			{
			case JWENGINE::EMapMode::TileMode:
				SetMapFragmentTile(TileID, iter_map_pos.x, iter_map_pos.y);
				break;
			case JWENGINE::EMapMode::MoveMode:
				// @warning
				// Erase tile ID is not -1 but 0 for MoveMode
				if (TileID == -1)
					TileID = 0;

				SetMapFragmentMove(TileID, iter_map_pos.x, iter_map_pos.y);
				break;
			default:
				break;
			}
		}
	}

	
	
}

PRIVATE void JWMap::SetTileTexture(WSTRING FileName)
{
	JWImage::SetTexture(FileName);

	assert(m_Size.x);

	if ((m_Size.x) && (m_Size.y))
	{
		m_MapInfo.TileSheetName = FileName;

		m_MapInfo.TileSheetSize = m_Size; // 'm_Width = TileSheetWidth' after SetTexture() being called
	}
}

PRIVATE void JWMap::SetMoveTexture(WSTRING FileName)
{
	if (m_pTextureMove)
	{
		m_pTextureMove->Release();
		m_pTextureMove = nullptr;
	}

	WSTRING NewFileName;
	NewFileName = m_BaseDir;
	NewFileName += ASSET_DIR;
	NewFileName += FileName;
	
	D3DXIMAGE_INFO tImageInfo;
	if (FAILED(D3DXCreateTextureFromFileEx(m_pDevice, NewFileName.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, &tImageInfo, nullptr, &m_pTextureMove)))
		return;

	m_MapInfo.MoveSheetName = FileName;

	m_MapInfo.MoveSheetSize.x = static_cast<float>(tImageInfo.Width);
	m_MapInfo.MoveSheetSize.y = static_cast<float>(tImageInfo.Height);
	m_bMoveTextureLoaded = true;
}

// @warning
// CreateMapBase() is called when a map is created or loaded
// so if you want to set a generally-used data (e.g. SMapInfo)
// for the map, do it here.
PRIVATE void JWMap::MakeMapBase()
{
	// Clear buffers
	ClearAllData();

	// Set map width & height
	m_MapInfo.MapSize.x = static_cast<float>(m_MapInfo.MapCols * m_MapInfo.TileSize);
	m_MapInfo.MapSize.y = static_cast<float>(m_MapInfo.MapRows * m_MapInfo.TileSize);

	// Set tilesheet texture of the map
	SetTileTexture(m_MapInfo.TileSheetName);

	// Calculate tilesheet's cols and rows IAW the loaded tile texture size and the tile size
	m_MapInfo.TileSheetCols = static_cast<int>(m_MapInfo.TileSheetSize.x) / m_MapInfo.TileSize;
	m_MapInfo.TileSheetRows = static_cast<int>(m_MapInfo.TileSheetSize.y) / m_MapInfo.TileSize;

	// Set movesheet texture of the map
	switch (m_MapInfo.TileSize)
	{
	case 32:
		SetMoveTexture(JWMap::MOVE_32);
		break;
	case 64:
		SetMoveTexture(JWMap::MOVE_64);
		break;
	default:
		break;
	}
}

PRIVATE void JWMap::AddMapFragmentTile(int TileID, int X, int Y)
{
	STextureUV tUV = ConvertIDtoUV(TileID, m_MapInfo.TileSize, m_MapInfo.TileSheetSize);

	DWORD tColor;
	if (TileID == -1)
	{
		tColor = D3DCOLOR_ARGB(0, 255, 255, 255); // The tile is transparent (Alpha = 0)
	}
	else
	{
		tColor = D3DCOLOR_ARGB(255, 255, 255, 255);
	}

	float tX = static_cast<float>(X * m_MapInfo.TileSize);
	float tY = static_cast<float>(Y * m_MapInfo.TileSize);

	m_Vertices.push_back(SVertexImage(tX, tY, 0, 1, tColor, tUV.u1, tUV.v1));
	m_Vertices.push_back(SVertexImage(tX + m_MapInfo.TileSize, tY, 0, 1, tColor, tUV.u2, tUV.v1));
	m_Vertices.push_back(SVertexImage(tX, tY + m_MapInfo.TileSize, 0, 1, tColor, tUV.u1, tUV.v2));
	m_Vertices.push_back(SVertexImage(tX + m_MapInfo.TileSize, tY + m_MapInfo.TileSize, 0, 1, tColor, tUV.u2, tUV.v2));
	int tVertCount = static_cast<int>(m_Vertices.size());

	m_Indices.push_back(SIndex3(tVertCount - 4, tVertCount - 3, tVertCount - 1));
	m_Indices.push_back(SIndex3(tVertCount - 4, tVertCount - 1, tVertCount - 2));
}

PRIVATE void JWMap::AddMapFragmentMove(int MoveID, int X, int Y)
{
	// @warning: This function should be called only if MoveSheet is loaded first
	if (m_MapInfo.MoveSheetSize.x && m_MapInfo.MoveSheetSize.y)
	{
		STextureUV tUV = ConvertIDtoUV(MoveID, m_MapInfo.TileSize, m_MapInfo.MoveSheetSize);

		DWORD Color = D3DCOLOR_ARGB(MOVE_ALPHA, 255, 255, 255);
		float tX = static_cast<float>(X * m_MapInfo.TileSize);
		float tY = static_cast<float>(Y * m_MapInfo.TileSize);

		m_VertMove.push_back(SVertexImage(tX, tY, 0, 1, Color, tUV.u1, tUV.v1));
		m_VertMove.push_back(SVertexImage(tX + m_MapInfo.TileSize, tY, 0, 1, Color, tUV.u2, tUV.v1));
		m_VertMove.push_back(SVertexImage(tX, tY + m_MapInfo.TileSize, 0, 1, Color, tUV.u1, tUV.v2));
		m_VertMove.push_back(SVertexImage(tX + m_MapInfo.TileSize, tY + m_MapInfo.TileSize, 0, 1, Color, tUV.u2, tUV.v2));
	}
}

// @warning: AddEnd() is always called after creating any kind of maps
PRIVATE void JWMap::AddEnd()
{
	JWImage::CreateVertexBuffer();
	JWImage::CreateIndexBuffer();
	JWImage::UpdateVertexBuffer();
	JWImage::UpdateIndexBuffer();

	if (m_bMoveTextureLoaded)
	{
		CreateVertexBufferMove();
		UpdateVertexBufferMove();
	}

	m_bMapExist = true;
	m_OffsetZeroY = m_pJWWindow->GetWindowData()->WindowHeight - (m_MapInfo.MapRows * m_MapInfo.TileSize);

	SetGlobalPosition(D3DXVECTOR2(0, 0));
}

PRIVATE void JWMap::CreateVertexBufferMove()
{
	int rVertSize = sizeof(SVertexImage) * static_cast<int>(m_VertMove.size());
	if (FAILED(m_pDevice->CreateVertexBuffer(rVertSize, 0,
		D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVBMove, nullptr)))
	{
		return;
	}
}

PRIVATE void JWMap::UpdateVertexBufferMove()
{
	int rVertSize = sizeof(SVertexImage) * static_cast<int>(m_VertMove.size());
	VOID* pVertices;
	if (FAILED(m_pVBMove->Lock(0, rVertSize, (void**)&pVertices, 0)))
		return;
	memcpy(pVertices, &m_VertMove[0], rVertSize);
	m_pVBMove->Unlock();
}

PRIVATE auto JWMap::GetMapTileBoundary(int MapID, EMapDirection Dir) const->float
{
	float Result = 0;

	D3DXVECTOR2 tMapXY = ConvertIDToXY(MapID, m_MapInfo.MapCols);

	float tX = m_Offset.x + tMapXY.x * m_MapInfo.TileSize;
	float tY = m_Offset.y + tMapXY.y * m_MapInfo.TileSize;

	switch (Dir)
	{
	case EMapDirection::Up:
		Result = tY;
		break;
	case EMapDirection::Down:
		Result = tY + m_MapInfo.TileSize;
		break;
	case EMapDirection::Left:
		Result = tX;
		break;
	case EMapDirection::Right:
		Result = tX + m_MapInfo.TileSize;
		break;
	default:
		break;
	}

	return Result;
}

PRIVATE auto JWMap::IsMovableTile(int MapID, EMapDirection Dir) const->bool
{
	if ((MapID >= (m_MapInfo.MapCols * m_MapInfo.MapRows)) || (MapID < 0))
		return true;

	int tMoveID = m_MapData[MapID].MoveID;
	switch (Dir)
	{
	case EMapDirection::Up:
		if ((tMoveID == 2) || (tMoveID == 7) || (tMoveID == 8) || (tMoveID == 9) ||
			(tMoveID == 12) || (tMoveID == 13) || (tMoveID == 14) || (tMoveID == 15))
			return false;
		return true;
	case EMapDirection::Down:
		if ((tMoveID == 1) || (tMoveID == 5) || (tMoveID == 6) || (tMoveID == 9) ||
			(tMoveID == 11) || (tMoveID == 12) || (tMoveID == 14) || (tMoveID == 15))
			return false;
		return true;
	case EMapDirection::Left:
		if ((tMoveID == 4) || (tMoveID == 5) || (tMoveID == 7) || (tMoveID == 10) ||
			(tMoveID == 11) || (tMoveID == 12) || (tMoveID == 13) || (tMoveID == 15))
			return false;
		return true;
	case EMapDirection::Right:
		if ((tMoveID == 3) || (tMoveID == 6) || (tMoveID == 8) || (tMoveID == 10) ||
			(tMoveID == 11) || (tMoveID == 13) || (tMoveID == 14) || (tMoveID == 15))
			return false;
		return true;
	default:
		return false;
	}
}

void JWMap::SetMode(EMapMode Mode)
{
	switch (Mode)
	{
	case EMapMode::TileMode:
		m_CurrMapMode = Mode;
		return;
	case EMapMode::MoveMode:
		if (m_bMoveTextureLoaded)
		{
			m_CurrMapMode = Mode;
			return;
		}
	default:
		break;
	}

	// @warning SetMoveTexture() should have been called first
	//assert(0);
}

void JWMap::SetPosition(D3DXVECTOR2 Offset)
{
	int VertID0 = 0;
	float tX = 0;
	float tY = 0;

	m_Offset = Offset;

	for (int i = 0; i < m_MapInfo.MapRows; i++)
	{
		for (int j = 0; j < m_MapInfo.MapCols; j++)
		{
			VertID0 = (j + (i * m_MapInfo.MapCols)) * 4;
			tX = static_cast<float>(j * m_MapInfo.TileSize) + m_Offset.x;
			tY = static_cast<float>(i * m_MapInfo.TileSize) + m_Offset.y;
			m_Vertices[VertID0].x = tX;
			m_Vertices[VertID0].y = tY;
			m_Vertices[VertID0 + 1].x = tX + m_MapInfo.TileSize;
			m_Vertices[VertID0 + 1].y = tY;
			m_Vertices[VertID0 + 2].x = tX;
			m_Vertices[VertID0 + 2].y = tY + m_MapInfo.TileSize;
			m_Vertices[VertID0 + 3].x = tX + m_MapInfo.TileSize;
			m_Vertices[VertID0 + 3].y = tY + m_MapInfo.TileSize;

			if (m_bMoveTextureLoaded)
			{
				m_VertMove[VertID0].x = tX;
				m_VertMove[VertID0].y = tY;
				m_VertMove[VertID0 + 1].x = tX + m_MapInfo.TileSize;
				m_VertMove[VertID0 + 1].y = tY;
				m_VertMove[VertID0 + 2].x = tX;
				m_VertMove[VertID0 + 2].y = tY + m_MapInfo.TileSize;
				m_VertMove[VertID0 + 3].x = tX + m_MapInfo.TileSize;
				m_VertMove[VertID0 + 3].y = tY + m_MapInfo.TileSize;
			}
		}
	}

	UpdateVertexBuffer();
	if (m_bMoveTextureLoaded)
	{
		UpdateVertexBufferMove();
	}
}

void JWMap::SetGlobalPosition(D3DXVECTOR2 Offset)
{
	float MapH = static_cast<float>(m_MapInfo.MapRows * m_MapInfo.TileSize);
	float NewOffsetY = m_pJWWindow->GetWindowData()->WindowHeight - MapH + Offset.y;

	SetPosition(D3DXVECTOR2(Offset.x, NewOffsetY));
}

PRIVATE void JWMap::SetMapFragmentTile(int TileID, int X, int Y)
{
	if ((X < m_MapInfo.MapCols) && (Y < m_MapInfo.MapRows))
	{
		int MapID = X + (Y * m_MapInfo.MapCols);
		int VertID0 = MapID * 4;

		STextureUV tUV = ConvertIDtoUV(TileID, m_MapInfo.TileSize, m_MapInfo.TileSheetSize);

		DWORD tColor;
		if (TileID == -1)
		{
			tColor = D3DCOLOR_ARGB(0, 255, 255, 255);
		}
		else
		{
			tColor = D3DCOLOR_ARGB(255, 255, 255, 255);
		}

		m_Vertices[VertID0].u = tUV.u1;
		m_Vertices[VertID0].v = tUV.v1;
		m_Vertices[VertID0].color = tColor;
		m_Vertices[VertID0 + 1].u = tUV.u2;
		m_Vertices[VertID0 + 1].v = tUV.v1;
		m_Vertices[VertID0 + 1].color = tColor;
		m_Vertices[VertID0 + 2].u = tUV.u1;
		m_Vertices[VertID0 + 2].v = tUV.v2;
		m_Vertices[VertID0 + 2].color = tColor;
		m_Vertices[VertID0 + 3].u = tUV.u2;
		m_Vertices[VertID0 + 3].v = tUV.v2;
		m_Vertices[VertID0 + 3].color = tColor;

		m_MapData[MapID].TileID = TileID;

		UpdateVertexBuffer();
	}
}

PRIVATE void JWMap::SetMapFragmentMove(int MoveID, int X, int Y)
{
	if ((X < m_MapInfo.MapCols) && (Y < m_MapInfo.MapRows))
	{
		int MapID = X + (Y * m_MapInfo.MapCols);
		int VertID0 = MapID * 4;

		STextureUV tUV = ConvertIDtoUV(MoveID, m_MapInfo.TileSize, m_MapInfo.MoveSheetSize);

		m_VertMove[VertID0].u = tUV.u1;
		m_VertMove[VertID0].v = tUV.v1;
		m_VertMove[VertID0 + 1].u = tUV.u2;
		m_VertMove[VertID0 + 1].v = tUV.v1;
		m_VertMove[VertID0 + 2].u = tUV.u1;
		m_VertMove[VertID0 + 2].v = tUV.v2;
		m_VertMove[VertID0 + 3].u = tUV.u2;
		m_VertMove[VertID0 + 3].v = tUV.v2;

		m_MapData[MapID].MoveID = MoveID;

		UpdateVertexBufferMove();
	}
}

void JWMap::Draw()
{
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	if (m_pTexture)
	{
		m_pDevice->SetTexture(0, m_pTexture);

		// Texture alpha * Diffuse color alpha
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

		m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SVertexImage));
		m_pDevice->SetFVF(D3DFVF_TEXTURE);
		m_pDevice->SetIndices(m_pIndexBuffer);
		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, static_cast<int>(m_Vertices.size()), 0, static_cast<int>(m_Indices.size()));
	}

	if ((m_CurrMapMode == EMapMode::MoveMode) && m_pTextureMove)
	{
		m_pDevice->SetTexture(0, m_pTextureMove);

		// Texture alpha * Diffuse color alpha
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

		m_pDevice->SetStreamSource(0, m_pVBMove, 0, sizeof(SVertexImage));
		m_pDevice->SetFVF(D3DFVF_TEXTURE);
		m_pDevice->SetIndices(m_pIndexBuffer);
		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, static_cast<int>(m_VertMove.size()), 0, static_cast<int>(m_Indices.size()));
	}

	m_pDevice->SetTexture(0, nullptr);
}

auto JWMap::DoesMapExist() const->bool
{ 
	return m_bMapExist;
};

auto JWMap::GetMode() const->EMapMode
{
	return m_CurrMapMode;
}

void JWMap::GetMapInfo(SMapInfo* Outptr_Info) const
{
	*Outptr_Info = m_MapInfo;
}

auto JWMap::GetMapOffset() const->D3DXVECTOR2
{ 
	return m_Offset;
}

auto JWMap::GetMapOffsetZeroY() const->int
{ 
	return m_OffsetZeroY;
}

auto JWMap::GetVelocityAfterCollision(SBoundingBox BB, D3DXVECTOR2 Velocity) const->D3DXVECTOR2
{
	D3DXVECTOR2 NewVelocity = Velocity;

	D3DXVECTOR2 tSprPosS{ 0, 0 };
	D3DXVECTOR2 tSprPosE{ 0, 0 };
	D3DXVECTOR2 tMapXYS{ 0, 0 };
	D3DXVECTOR2 tMapXYE{ 0, 0 };

	if (Velocity.x > 0)
	{
		// Go Right
		tSprPosS = BB.PositionOffset;
		tSprPosS.x += BB.Size.x; // ¦¤ (Right Up)
		tSprPosE = tSprPosS;
		tSprPosE.x += Velocity.x;
		tSprPosE.y += BB.Size.y;

		tMapXYS = ConvertPositionToXY(tSprPosS, m_Offset, m_MapInfo.TileSize);
		tMapXYE = ConvertPositionToXY(tSprPosE, m_Offset, m_MapInfo.TileSize);
	}
	else if (Velocity.x < 0)
	{
		// Go Left
		tSprPosS = BB.PositionOffset; // ¦£ (Left Up)
		tSprPosE = tSprPosS;
		tSprPosE.x += Velocity.x;
		tSprPosE.y += BB.Size.y;

		tMapXYS = ConvertPositionToXY(tSprPosS, m_Offset, m_MapInfo.TileSize);
		tMapXYE = ConvertPositionToXY(tSprPosE, m_Offset, m_MapInfo.TileSize);
	}
	else if (Velocity.y > 0)
	{
		// Go Down
		tSprPosS = BB.PositionOffset;
		tSprPosS.y += BB.Size.y; // ¦¦ (Left Down)
		tSprPosE = tSprPosS;
		tSprPosE.x += BB.Size.x; // ¦¥ (Right down)
		tSprPosE.y += Velocity.y;

		tMapXYS = ConvertPositionToXY(tSprPosS, m_Offset, m_MapInfo.TileSize, true);
		tMapXYE = ConvertPositionToXY(tSprPosE, m_Offset, m_MapInfo.TileSize, true);
	}
	else if (Velocity.y < 0)
	{
		// Go Up
		tSprPosS = BB.PositionOffset; // ¦£ (Left Up)
		tSprPosE = tSprPosS;
		tSprPosE.x += BB.Size.x; // ¦¤ (Right up)
		tSprPosE.y += Velocity.y;

		tMapXYS = ConvertPositionToXY(tSprPosS, m_Offset, m_MapInfo.TileSize);
		tMapXYE = ConvertPositionToXY(tSprPosE, m_Offset, m_MapInfo.TileSize);
	}

	int tXS = static_cast<int>(tMapXYS.x);
	int tYS = static_cast<int>(tMapXYS.y);
	int tXE = static_cast<int>(tMapXYE.x);
	int tYE = static_cast<int>(tMapXYE.y);

	float fWall = 0;
	float fWallCmp = 0;

	int tMapID = 0;

	// Check if the life is inside the map's Y range (rows)
	if ((tYS > m_MapInfo.MapRows + DEPTH_HELL) && (tYE > m_MapInfo.MapRows + DEPTH_HELL))
	{
		// If the life reaches the bottom of the hell, make it stop falling
		NewVelocity.y = 0;
	}

	// Check if the life is inside the map's X range (cols)
	// If it's outside, it should fall (= no changes in NewVelocity)
	if (((tXS >= 0) || (tXE >= 0)) && ((tXS < m_MapInfo.MapCols) || (tXE < m_MapInfo.MapCols)))
	{
		if (Velocity.x > 0)
		{
			// Go Right
			for (int i = tXS; i <= tXE; i++)
			{
				for (int j = tYS; j <= tYE; j++)
				{
					tMapID = ConvertXYToID(D3DXVECTOR2(static_cast<float>(i), static_cast<float>(j)), m_MapInfo.MapCols);
					if (IsMovableTile(tMapID, EMapDirection::Right) == false)
					{
						fWallCmp = GetMapTileBoundary(tMapID, EMapDirection::Left);
						if (fWall == 0)
						{
							fWall = fWallCmp;
						}
						else if (fWall && fWallCmp)
						{
							fWall = min(fWall, fWallCmp);
						}
					}
				}
			}

			if (fWall)
			{
				float fCurr = tSprPosS.x + Velocity.x;
				float fDist = fWall - tSprPosS.x - 0.1f;
				NewVelocity.x = fDist;
			}
		}
		else if (Velocity.x < 0)
		{
			// Go Left
			for (int i = tXS; i >= tXE; i--)
			{
				for (int j = tYS; j <= tYE; j++)
				{
					tMapID = ConvertXYToID(D3DXVECTOR2(static_cast<float>(i), static_cast<float>(j)), m_MapInfo.MapCols);
					if (IsMovableTile(tMapID, EMapDirection::Left) == false)
					{
						fWallCmp = GetMapTileBoundary(tMapID, EMapDirection::Right);
						if (fWall == 0)
						{
							fWall = fWallCmp;
						}
						else if (fWall && fWallCmp)
						{
							fWall = max(fWall, fWallCmp);
						}
					}
				}
			}

			if (fWall)
			{
				float fCurr = tSprPosS.x + Velocity.x;
				float fDist = fWall - tSprPosS.x;
				NewVelocity.x = fDist;
			}
		}
		else if (Velocity.y > 0)
		{
			// Go Down
			for (int j = tYS; j <= tYE; j++)
			{
				fWallCmp = 0;

				for (int i = tXS; i <= tXE; i++)
				{
					tMapID = ConvertXYToID(D3DXVECTOR2(static_cast<float>(i), static_cast<float>(j)), m_MapInfo.MapCols);
					if (IsMovableTile(tMapID, EMapDirection::Down) == false)
					{
						fWallCmp = GetMapTileBoundary(tMapID, EMapDirection::Up);
						if (fWall == 0)
						{
							fWall = fWallCmp;
						}
						else if (fWall && fWallCmp)
						{
							fWall = min(fWall, fWallCmp);
						}
					}
				}

				if (fWall)
				{
					float fCurr = tSprPosS.y + Velocity.y;
					float fDist = fWall - tSprPosS.y - 0.1f;
					NewVelocity.y = min(NewVelocity.y, fDist);
				}
			}
		}
		else if (Velocity.y < 0)
		{
			// Go Up
			for (int i = tXS; i <= tXE; i++)
			{
				for (int j = tYS; j >= tYE; j--)
				{
					tMapID = ConvertXYToID(D3DXVECTOR2(static_cast<float>(i), static_cast<float>(j)), m_MapInfo.MapCols);
					if (IsMovableTile(tMapID, EMapDirection::Up) == false)
					{
						fWallCmp = GetMapTileBoundary(tMapID, EMapDirection::Down);
						if (fWall == 0)
						{
							fWall = fWallCmp;
						}
						else if (fWall && fWallCmp)
						{
							fWall = max(fWall, fWallCmp);
						}
					}
				}
			}

			if (fWall)
			{
				float fCurr = tSprPosS.y + Velocity.y;
				float fDist = fWall - tSprPosS.y;
				NewVelocity.y = max(NewVelocity.y, fDist);
			}
		}
	}
	
	return NewVelocity;
}