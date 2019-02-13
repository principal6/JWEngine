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

		SMapInfo() : MapRows(0), MapCols(0), TileSize(0) {};
	};

	struct SMapData
	{
		int TileID;
		int MoveID;

		SMapData() : TileID(0), MoveID(0) {};
		SMapData(int TILEID, int MOVEID) : TileID(TILEID), MoveID(MOVEID) {};
	};

	class JWMap final : protected JWImage
	{
	public:
		JWMap();
		~JWMap() {};

		auto JWMap::Create(JWWindow* pJWWindow, WSTRING* pBaseDir)->EError;
		void JWMap::Destroy() override;

		void JWMap::CreateMap(SMapInfo* InPtr_Info);
		void JWMap::LoadMap(WSTRING FileName);
		void JWMap::SaveMap(WSTRING FileName);
		//void JWMap::EditMap(const JWTileMapSelector* InPtr_Selector, bool bErase = false);

		void JWMap::Draw() override;

		void JWMap::SetMode(EMapMode Mode);
		void JWMap::SetPosition(D3DXVECTOR2 Offset);

		// Global position for map movement in game (Position's Y value inversed)
		void JWMap::SetGlobalPosition(D3DXVECTOR2 Offset);

		auto JWMap::DoesMapExist() const->bool;
		auto JWMap::GetMode() const->EMapMode;
		void JWMap::GetMapInfo(SMapInfo* Outptr_Info) const;
		auto JWMap::GetMapOffset() const->D3DXVECTOR2;
		auto JWMap::GetMapOffsetZeroY() const->int;
		auto JWMap::GetVelocityAfterCollision(SBoundingBox BB, D3DXVECTOR2 Velocity) const->D3DXVECTOR2;

	private:
		static auto JWMap::ConvertIDtoUV(int ID, int TileSize, D3DXVECTOR2 SheetSize)->STextureUV;
		static auto JWMap::ConvertIDToXY(int ID, int Cols)->D3DXVECTOR2;
		static auto JWMap::ConvertXYToID(D3DXVECTOR2 XY, int Cols)->int;
		static auto JWMap::ConvertXYToID(POINT XY, int Cols)->int;
		static auto JWMap::ConvertPositionToXY(D3DXVECTOR2 Position, D3DXVECTOR2 Offset,
			int TileSize, bool YRoundUp = false)->D3DXVECTOR2;

		void JWMap::ClearAllData();

		void JWMap::MakeMapBase();
		void JWMap::AddMapFragmentTile(int TileID, int X, int Y);
		void JWMap::AddMapFragmentMove(int MoveID, int X, int Y);
		void JWMap::AddEnd();
		void JWMap::SetTileTexture(WSTRING FileName);
		void JWMap::SetMoveTexture(WSTRING FileName);
		void JWMap::ParseLoadedMapData(WSTRING* InoutPtr_WSTRING); // For loading maps
		void JWMap::MakeLoadedMap(WSTRING* InoutPtr_WSTRING); // For loading maps
		void JWMap::GetMapDataForSave(WSTRING* OutPtr_WSTRING) const; // For saving maps
		void JWMap::GetMapDataPartForSave(int DataID, wchar_t* OutPtr_wchar, int size) const; // For saving maps

		void JWMap::CreateVertexBufferMove(); // IndexBuffer is not needed because they are the same
		void JWMap::UpdateVertexBufferMove();

		auto JWMap::IsMovableTile(int MapID, EMapDirection Direction) const->bool;
		auto JWMap::GetMapTileBoundary(int MapID, EMapDirection Direction) const->float;

		void JWMap::SetMapFragmentTile(int TileID, int X, int Y);
		void JWMap::SetMapFragmentMove(int MoveID, int X, int Y);

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