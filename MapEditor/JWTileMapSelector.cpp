#include "JWTileMapSelector.h"
#include "../CoreBase/JWWindow.h"
#include "../Core/JWMap.h"

using namespace JWENGINE;

// Static member variable
const wchar_t* JWTileMapSelector::SEL_FN = L"tilesel.png";

PRIVATE auto JWTileMapSelector::ConvertPositionToCellXY(POINT Position)->POINT
{
	POINT Result{ 0, 0 };
	
	if (m_pMapInfo && m_pMapInfo->TileSize)
	{
		Result.x = (static_cast<int>(Position.x) / m_pMapInfo->TileSize);
		Result.y = (static_cast<int>(Position.y) / m_pMapInfo->TileSize);

		Result.x = max(Result.x, 0);
		Result.y = max(Result.y, 0);
	}

	return Result;
}

JWTileMapSelector::JWTileMapSelector()
{
	m_pMapInfo = nullptr;
	m_pTileSelectorWindow = nullptr;
	m_pMapWIndow = nullptr;

	m_SelectionSize = { 0, 0 };
	m_TileSelectorOffset = { 0, 0 };
	m_TileSelectorPositionInCells = { 0, 0 };
	m_MapSelectorOffset = { 0, 0 };
	m_MapSelectorPositionInCells = { 0, 0 };
}

auto JWTileMapSelector::Create(JWWindow* pTileSelectorWindow, JWWindow* pMapWindow, WSTRING BaseDir)->EError
{
	if (nullptr == (m_pTileSelectorWindow = pTileSelectorWindow))
		return EError::NULLPTR_WINDOW;

	if (nullptr == (m_pMapWIndow = pMapWindow))
		return EError::NULLPTR_WINDOW;

	// Create tile selector image
	if (m_TileSelector = MAKE_UNIQUE(JWImage)())
	{
		if (DX_FAILED(m_TileSelector->Create(m_pTileSelectorWindow, BaseDir)))
			return EError::IMAGE_NOT_CREATED;
	}
	m_TileSelector->SetTexture(SEL_FN);
	m_TileSelector->SetAlpha(SEL_ALPHA);
	m_TileSelector->SetSize(D3DXVECTOR2(0, 0));

	// Create map selector image
	// Map selector's texture will be set when SetMapInfo() is called
	if (m_MapSelector = MAKE_UNIQUE(JWImage)())
	{
		if (DX_FAILED(m_MapSelector->Create(m_pMapWIndow, BaseDir)))
			return EError::IMAGE_NOT_CREATED;
	}
	m_MapSelector->SetAlpha(SEL_ALPHA);

	return EError::OK;
}

void JWTileMapSelector::Destroy()
{

}

void JWTileMapSelector::UpdateTileSelector()
{
	SMouseData* MouseData = m_pTileSelectorWindow->GetMouseData();

	if (m_pMapInfo)
	{
		if (m_pMapInfo->TileSize)
		{
			if (MouseData->bMouseLeftButtonPressed)
			{
				POINT PositionInCells = ConvertPositionToCellXY(MouseData->MousePosition);
				POINT DownPositionInCells = ConvertPositionToCellXY(MouseData->MouseDownPosition);

				m_SelectionSize.x = abs(DownPositionInCells.x - PositionInCells.x);
				m_SelectionSize.y = abs(DownPositionInCells.y - PositionInCells.y);

				m_TileSelectorPositionInCells = DownPositionInCells;

				if ((DownPositionInCells.x - PositionInCells.x) > 0)
				{
					// X position flip
					m_TileSelectorPositionInCells.x = DownPositionInCells.x - m_SelectionSize.x;
				}
				if ((DownPositionInCells.y - PositionInCells.y) > 0)
				{
					// Y position flip
					m_TileSelectorPositionInCells.y = DownPositionInCells.y - m_SelectionSize.y;
				}

				POINT ScreenPositionInCells = m_TileSelectorPositionInCells;

				// Set offset for scrollbar movement
				m_TileSelectorPositionInCells.x += m_TileSelectorOffset.x;
				m_TileSelectorPositionInCells.y += m_TileSelectorOffset.y;

				D3DXVECTOR2 TileSelectorNewPosition;
				TileSelectorNewPosition.x = static_cast<float>(ScreenPositionInCells.x * m_pMapInfo->TileSize);
				TileSelectorNewPosition.y = static_cast<float>(ScreenPositionInCells.y * m_pMapInfo->TileSize);

				D3DXVECTOR2 NewSize{ 0, 0 };
				NewSize.x = m_SelectionSize.x + 1.0f;
				NewSize.y = m_SelectionSize.y + 1.0f;
				NewSize *= static_cast<float>(m_pMapInfo->TileSize);

				m_TileSelector->SetPosition(TileSelectorNewPosition);
				m_TileSelector->SetSize(NewSize);

				D3DXVECTOR2 MapSelectorNewPosition = TileSelectorNewPosition;
				MapSelectorNewPosition.x += static_cast<float>(m_TileSelectorOffset.x * m_pMapInfo->TileSize);
				MapSelectorNewPosition.y += static_cast<float>(m_TileSelectorOffset.y * m_pMapInfo->TileSize);

				m_MapSelector->SetSize(NewSize);
				m_MapSelector->SetAtlasUV(MapSelectorNewPosition, NewSize);
			}
		}
	}
}

void JWTileMapSelector::UpdateMapSelector()
{
	SMouseData* MouseData = m_pMapWIndow->GetMouseData();

	if (m_pMapInfo)
	{
		m_MapSelectorPositionInCells = ConvertPositionToCellXY(MouseData->MousePosition);
		POINT ScreenPositionInCells = m_MapSelectorPositionInCells;

		// Set offset for scrollbar movement
		m_MapSelectorPositionInCells.x += m_MapSelectorOffset.x;
		m_MapSelectorPositionInCells.y += m_MapSelectorOffset.y;

		// @warning
		// Restrict position in cells IAW map's max rows and cols
		ScreenPositionInCells.x = min(ScreenPositionInCells.x, m_pMapInfo->MapCols - 1);
		ScreenPositionInCells.y = min(ScreenPositionInCells.y, m_pMapInfo->MapRows - 1);

		D3DXVECTOR2 NewPosition;
		NewPosition.x = static_cast<float>(ScreenPositionInCells.x * m_pMapInfo->TileSize);
		NewPosition.y = static_cast<float>(ScreenPositionInCells.y * m_pMapInfo->TileSize);

		m_MapSelector->SetPosition(NewPosition);
	}
}

void JWTileMapSelector::UpdateMapMode(EMapMode Mode)
{
	switch (Mode)
	{
	case JWENGINE::EMapMode::TileMode:
		m_TileSelector->SetSize(m_pMapInfo->TileSheetSize);
		m_MapSelector->SetTexture(m_pMapInfo->TileSheetName);
		
		break;
	case JWENGINE::EMapMode::MoveMode:
		m_TileSelector->SetSize(m_pMapInfo->MoveSheetSize);
		m_MapSelector->SetTexture(m_pMapInfo->MoveSheetName);
		
		break;
	default:
		break;
	}

	InitializeSelectorPositionAndSize();
}

void JWTileMapSelector::UpdateOffset()
{
	if (m_pTileSelectorWindow)
	{
		m_TileSelectorOffset.x = m_pTileSelectorWindow->GetHorizontalScrollbarPosition();
		m_TileSelectorOffset.y = m_pTileSelectorWindow->GetVerticalScrollbarPosition();
	}

	if (m_pMapWIndow)
	{
		m_MapSelectorOffset.x = m_pMapWIndow->GetHorizontalScrollbarPosition();
		m_MapSelectorOffset.y = m_pMapWIndow->GetVerticalScrollbarPosition();
	}
}

void JWTileMapSelector::Draw()
{
	if (m_TileSelector)
	{
		m_TileSelector->Draw();
	}

	if (m_MapSelector)
	{
		m_MapSelector->Draw();
		m_MapSelector->DrawBoundingBox();
	}
}

auto JWTileMapSelector::SetMapInfo(SMapInfo* pInfo)->EError
{
	if (nullptr == (m_pMapInfo = pInfo))
		return EError::NULLPTR_MAP_INFO;

	m_TileSelector->SetSize(D3DXVECTOR2(static_cast<float>(m_pMapInfo->TileSize), static_cast<float>(m_pMapInfo->TileSize)));
	m_MapSelector->SetTexture(m_pMapInfo->TileSheetName);
	
	InitializeSelectorPositionAndSize();

	return EError::OK;
}

PRIVATE void JWTileMapSelector::InitializeSelectorPositionAndSize()
{
	if (m_pMapInfo)
	{
		m_TileSelector->SetSize(D3DXVECTOR2(static_cast<float>(m_pMapInfo->TileSize),
			static_cast<float>(m_pMapInfo->TileSize)));
		m_TileSelector->SetPosition(D3DXVECTOR2(0, 0));

		m_MapSelector->SetSize(m_TileSelector->GetSize());
		m_MapSelector->SetAtlasUV(D3DXVECTOR2(0, 0), m_TileSelector->GetSize());
	}
}

auto JWTileMapSelector::GetTileSelectorPositionInCells() const->POINT
{
	return m_TileSelectorPositionInCells;
}

auto JWTileMapSelector::GetMapSelectorPositionInCells() const->POINT
{
	return m_MapSelectorPositionInCells;
}

auto JWTileMapSelector::GetSelectionSizeInCells() const->POINT
{
	return m_SelectionSize;
}