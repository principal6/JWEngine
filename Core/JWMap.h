#pragma once

#include <fstream>
#include "../CoreBase/JWImage.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	// ***

	enum class EMapMode
	{
		TileMode,
		MoveMode,
	};

	enum class EMapDirection
	{
		Up,
		Down,
		Left,
		Right,
	};

	struct SMapInfo
	{
		WSTRING MapName;
		WSTRING TileSheetName;
		WSTRING MoveSheetName;

		int MapRows;
		int MapCols;
		int TileSize;
		int TileSheetRows;
		int TileSheetCols;
		D3DXVECTOR2 MapSize;
		D3DXVECTOR2 TileSheetSize;
		D3DXVECTOR2 MoveSheetSize;

		SMapInfo() : MapRows(0), MapCols(0), TileSize(0), TileSheetRows(0), TileSheetCols(0),
			MapSize(0, 0), TileSheetSize(0, 0), MoveSheetSize(0, 0) {};
	};

	struct SMapData
	{
		int TileID;
		int MoveID;

		SMapData() : TileID(0), MoveID(0) {};
		SMapData(CINT TILEID, CINT MOVEID) : TileID(TILEID), MoveID(MOVEID) {};
	};

	class JWMap final : protected JWImage
	{
	public:
		JWMap();
		~JWMap() {};

		auto JWMap::Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir)->EError;
		void JWMap::Destroy() override;

		void JWMap::CreateMap(const SMapInfo* InPtr_Info);
		void JWMap::LoadMap(const WSTRING FileName);
		void JWMap::SaveMap(const WSTRING FileName);

		void JWMap::Draw() override;

		void JWMap::SetMode(const EMapMode Mode);
		void JWMap::SetPosition(const D3DXVECTOR2 Offset);

		// Global position for map movement in game (Position's Y value inversed)
		void JWMap::SetGlobalPosition(const D3DXVECTOR2 Offset);

		auto JWMap::DoesMapExist() const->const bool;
		auto JWMap::GetMode() const->const EMapMode;
		void JWMap::GetMapInfo(SMapInfo* OutPtr_Info) const;
		auto JWMap::GetMapOffset() const->const D3DXVECTOR2;
		auto JWMap::GetMapOffsetZeroY() const->CINT;
		auto JWMap::GetVelocityAfterCollision(SBoundingBox BB, D3DXVECTOR2 Velocity) const->const D3DXVECTOR2;

	private:
		static auto JWMap::ConvertIDtoUV(const int ID, const int TileSize, const D3DXVECTOR2 SheetSize)->STextureUV;
		static auto JWMap::ConvertIDToXY(const int ID, const int Cols)->D3DXVECTOR2;
		static auto JWMap::ConvertXYToID(const D3DXVECTOR2 XY, const int Cols)->int;
		static auto JWMap::ConvertXYToID(const POINT XY, const int Cols)->int;
		static auto JWMap::ConvertPositionToXY(const D3DXVECTOR2 Position, const D3DXVECTOR2 Offset,
			const int TileSize, const bool YRoundUp = false)->D3DXVECTOR2;

		void JWMap::ClearAllData();

		void JWMap::MakeMapBase();
		void JWMap::AddMapFragmentTile(const int TileID, const int X, const int Y);
		void JWMap::AddMapFragmentMove(const int MoveID, const int X, const int Y);
		void JWMap::AddEnd();
		void JWMap::SetTileTexture(const WSTRING FileName);
		void JWMap::SetMoveTexture(const WSTRING FileName);
		void JWMap::ParseLoadedMapData(WSTRING* InOutPtr_WSTRING); // For loading maps
		void JWMap::MakeLoadedMap(WSTRING* InOutPtr_WSTRING); // For loading maps
		void JWMap::GetMapDataForSave(WSTRING* OutPtr_WSTRING) const; // For saving maps
		void JWMap::GetMapDataPartForSave(const int DataID, wchar_t* OutPtr_wchar, const int Size) const; // For saving maps

		void JWMap::CreateVertexBufferMove(); // IndexBuffer is not needed because they are the same
		void JWMap::UpdateVertexBufferMove();

		auto JWMap::IsMovableTile(const int MapID, const EMapDirection Direction) const->bool;
		auto JWMap::GetMapTileBoundary(const int MapID, const EMapDirection Direction) const->float;

		void JWMap::SetMapFragmentTile(const int TileID, const int X, const int Y);
		void JWMap::SetMapFragmentMove(const int MoveID, const int X, const int Y);

	private:
		static const int MAX_LINE_LEN;
		static const int MAX_TILEID_LEN;
		static const int MAX_MOVEID_LEN;
		static const int MOVE_ALPHA;
		static const int DEF_TILE_SIZE;
		static const int DEPTH_HELL;

		static const wchar_t* MOVE_32;
		static const wchar_t* MOVE_64;

		EMapMode m_CurrMapMode; // For Map Editor
		bool m_bMapExist;

		SMapInfo m_MapInfo;
		VECTOR<SMapData> m_MapData;

		bool m_bMoveTextureLoaded;
		LPDIRECT3DTEXTURE9 m_pTextureMove;
		LPDIRECT3DVERTEXBUFFER9 m_pVBMove;
		VECTOR<SVertexImage> m_VertMove;

		D3DXVECTOR2 m_Offset; // For map movement
		int m_OffsetZeroY; // For map movement (& inversed Y values)
	};
};