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

		int MapRows{};
		int MapCols{};
		int TileSize{};
		int TileSheetRows{};
		int TileSheetCols{};
		D3DXVECTOR2 MapSize{};
		D3DXVECTOR2 TileSheetSize{};
		D3DXVECTOR2 MoveSheetSize{};
	};

	struct SMapData
	{
		int TileID{};
		int MoveID{};

		SMapData() {};
		SMapData(int _TileID, int _MoveID) : TileID(_TileID), MoveID(_MoveID) {};
	};

	class JWMap final : protected JWImage
	{
	public:
		JWMap();
		~JWMap();

		void Create(const JWWindow& Window, const WSTRING& BaseDir) noexcept override;
		
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

		void ParseLoadedMapData(WSTRING* InOutPtr_STRING) noexcept; // For loading maps
		void MakeLoadedMap(WSTRING* InOutPtr_STRING) noexcept; // For loading maps

		void GetMapDataForSave(WSTRING* OutPtr_STRING) const noexcept; // For saving maps
		void GetMapDataPartForSave(int DataID, wchar_t* OutPtr_wchar, int Size) const noexcept; // For saving maps

		void CreateVertexBufferMove() noexcept; // IndexBuffer is not needed because they are the same
		void UpdateVertexBufferMove() noexcept;

		auto IsMovableTile(int MapID, EMapDirection Direction) const noexcept->bool;
		auto GetMapTileBoundary(int MapID, EMapDirection Direction) const noexcept->float;

		void SetMapFragmentTile(int TileID, int X, int Y) noexcept;
		void SetMapFragmentMove(int MoveID, int X, int Y) noexcept;

	private:
		static constexpr int MAX_LINE_LEN{ 1024 };
		static constexpr int MAX_TILEID_LEN{ 3 };
		static constexpr int MAX_MOVEID_LEN{ 2 };
		static constexpr int MOVE_ALPHA{ 100 };
		static constexpr int DEFAULT_TILE_SIZE{ 32 };
		static constexpr int DEPTH_HELL{ 10 };

		static constexpr wchar_t* MOVE_32{ L"move32.png" };
		static constexpr wchar_t* MOVE_64{ L"move64.png" };

		EMapMode m_CurrMapMode{ EMapMode::TileMode }; // For Map Editor
		bool m_bMapExist{ false };

		SMapInfo m_MapInfo;
		VECTOR<SMapData> m_MapData;

		bool m_bMoveTextureLoaded{ false };
		LPDIRECT3DTEXTURE9 m_pTextureMove{ nullptr };
		LPDIRECT3DVERTEXBUFFER9 m_pVBMove{ nullptr };
		VECTOR<SVertexImage> m_VertMove;

		D3DXVECTOR2 m_Offset{}; // For map movement
		int m_OffsetZeroY{}; // For map movement (& inversed Y values)
	};
};