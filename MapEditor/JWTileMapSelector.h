#pragma once

#include "../CoreBase/JWImage.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	struct SMouseData;
	struct SMapInfo;
	enum class EMapMode;
	// ***

	class JWTileMapSelector final
	{
	public:
		JWTileMapSelector();
		~JWTileMapSelector() {};

		auto Create(JWWindow* pTileSelectorWindow, JWWindow* pMapWindow, WSTRING* pBaseDir)->EError;
		void Destroy();

		void UpdateTileSelector();
		void UpdateMapSelector();
		void UpdateMapMode(EMapMode Mode);
		void UpdateOffset();
		void Draw();

		auto SetMapInfo(SMapInfo* pInfo)->EError;

		auto GetTileSelectorPositionInCells() const->POINT;
		auto GetMapSelectorPositionInCells() const->POINT;
		auto GetSelectionSizeInCells() const->POINT;

	private:
		auto ConvertPositionToCellXY(POINT Position)->POINT;
		void InitializeSelectorPositionAndSize();

	private:
		static const int SEL_ALPHA = 160;
		static const wchar_t* SEL_FN;

		SMapInfo* m_pMapInfo;
		JWWindow* m_pTileSelectorWindow;
		JWWindow* m_pMapWIndow;

		UNIQUE_PTR<JWImage> m_TileSelector;
		UNIQUE_PTR<JWImage> m_MapSelector;

		POINT m_SelectionSize;
		POINT m_TileSelectorOffset;
		POINT m_TileSelectorPositionInCells;
		POINT m_MapSelectorOffset;
		POINT m_MapSelectorPositionInCells;
	};
};