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
		SMapData(int TILEID, int MOVEID) : TileID(TILEID), MoveID(MOVEID) {};
	};

	class JWMap final : protected JWImage
	{
	public:
		JWMap();
		~JWMap() {};

		void Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir) override;
		void Destroy() noexcept override;

		void CreateMap(const SMapInfo* InPtr_Info) noexcept;
		void LoadMap(const WSTRING& FileName) noexcept;
		void SaveMap(const WSTRING& FileName) noexcept;

		void Draw() noexcept override;

		void SetMode(EMapMode Mode) noexcept;
		void SetPosition(const D3DXVECTOR2& Offset) noexcept;

		// Global position for map movement in game (Position's Y value inversed)
		void SetGlobalPosition(const D3DXVECTOR2& Offset) noexcept;

		auto DoesMapExist() const noexcept->bool;
		auto GetMode() const noexcept->EMapMode;
		void GetMapInfo(SMapInfo* OutPtr_Info) const noexcept;
		auto GetMapOffset() const noexcept->const D3DXVECTOR2;
		auto GetMapOffsetZeroY() const noexcept->int;
		auto GetVelocityAfterCollision(SBoundingBox BB, D3DXVECTOR2 Velocity) const noexcept->const D3DXVECTOR2;

	private:
		static auto ConvertIDtoUV(int ID, int TileSize, const D3DXVECTOR2& SheetSize) noexcept->STextureUV;
		static auto ConvertIDToXY(int ID, int Cols) noexcept->D3DXVECTOR2;
		static auto ConvertXYToID(const D3DXVECTOR2& XY, int Cols) noexcept->int;
		static auto ConvertXYToID(const POINT& XY, int Cols) noexcept->int;
		static auto ConvertPositionToXY(const D3DXVECTOR2& Position, const D3DXVECTOR2& Offset, int TileSize, bool YRoundUp = false) noexcept->D3DXVECTOR2;

		void ClearAllData() noexcept;

		void MakeMapBase() noexcept;
		void AddMapFragmentTile(int TileID, int X, int Y) noexcept;
		void AddMapFragmentMove(int MoveID, int X, int Y) noexcept;
		void AddEnd() noexcept;

		void SetTileTexture(const WSTRING& FileName) noexcept;
		void SetMoveTexture(const WSTRING& FileName) noexcept;

		void ParseLoadedMapData(WSTRING* InOutPtr_WSTRING) noexcept; // For loading maps
		void MakeLoadedMap(WSTRING* InOutPtr_WSTRING) noexcept; // For loading maps

		void GetMapDataForSave(WSTRING* OutPtr_WSTRING) const noexcept; // For saving maps
		void GetMapDataPartForSave(int DataID, wchar_t* OutPtr_wchar, int Size) const noexcept; // For saving maps

		void CreateVertexBufferMove(); // IndexBuffer is not needed because they are the same
		void UpdateVertexBufferMove();

		auto IsMovableTile(int MapID, EMapDirection Direction) const noexcept->bool;
		auto GetMapTileBoundary(int MapID, EMapDirection Direction) const noexcept->float;

		void SetMapFragmentTile(int TileID, int X, int Y) noexcept;
		void SetMapFragmentMove(int MoveID, int X, int Y) noexcept;

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