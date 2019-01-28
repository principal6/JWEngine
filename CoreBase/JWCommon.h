#pragma once

#include <iostream>
#include <Windows.h>
#include <windowsx.h>
#include <d3dx9.h>
#include <cassert>
#include <vector>
#include <memory>
#include <map>
//#include <string>

#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")

#ifndef CONVENIENT_STD
#define CONVENIENT_STD
	using STRING = std::string;
	using WSTRING = std::wstring;

	using WIFSTREAM = std::wifstream;
	using WOFSTREAM = std::wofstream;

	template <typename T>
	using VECTOR = std::vector<T>;

	template <typename T>
	using UNIQUE_PTR = std::unique_ptr<T>;

	template <typename T>
	using SHARED_PTR = std::shared_ptr<T>;

	#define MAKE_UNIQUE(T) std::make_unique<T>
	#define MAKE_SHARED(T) std::make_shared<T>
#endif

// Static function
#define STATIC
// Protected method
#define PROTECTED
// Private method
#define PRIVATE

namespace JWENGINE
{
	#define JW_SUCCEEDED(func) (func == EError::OK)
	#define JW_FAILED(func) (func != EError::OK)
	#define JW_DESTROY(obj) {if(obj) {obj->Destroy(); delete obj; obj = nullptr;}}
	#define JW_DESTROY_SMART(obj) {if(obj) {obj->Destroy();}}
	#define JW_RELEASE(obj) {if(obj) {obj->Release(); obj = nullptr;}}

	using CINT = const int;

	static constexpr int MAX_FILE_LEN = 260;
	static constexpr int MAX_TEXT_LEN = 512;

	// @warning: This value must be 256 for Direct Input
	static constexpr int NUM_KEYS = 256;

	static constexpr int MAX_UNIT_COUNT = 100;

	static const DWORD D3DFVF_TEXTURE = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	static const DWORD D3DFVF_LINE = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

	const wchar_t ASSET_DIR[] = L"\\Asset\\";
	const wchar_t DEFAULT_FONT[] = L"megt20all.fnt";

	enum class EError
	{
		/** No error */
		OK,

		/** Win32Api, DirectX */
		WINAPIWINDOW_NOT_CREATED,
		DIRECTX_NOT_CREATED,
		TEXTURE_NOT_CREATED,
		VERTEX_BUFFER_NOT_CREATED,
		INDEX_BUFFER_NOT_CREATED,
		VERTEX_BUFFER_NOT_LOCKED,
		INDEX_BUFFER_NOT_LOCKED,

		/** Core creation */
		WINDOW_NOT_CREATED,
		IMAGE_NOT_CREATED,
		INPUT_NOT_CREATED,
		LINE_NOT_CREATED,
		FONT_NOT_CREATED,
		RECTANGLE_NOT_CREATED,

		/** Game creation */
		LIFE_NOT_CREATED,
		MAP_NOT_CREATED,
		SPRITE_NOT_CREATED,
		MONSTERMANAGER_NOT_CREATED,
		EFFECTMANAGER_NOT_CREATED,
		OBJECT_NOT_CREATED,

		/** Map editor creation */
		TILESELECTOR_NOT_CREATED,

		/** GUI creation */
		CONTROL_NOT_CREATED,

		/** Null pointer */
		NULLPTR_DEVICE,
		NULLPTR_WINDOW,
		NULLPTR_MAP,
		NULLPTR_MAP_INFO,
		NULLPTR_IMAGE,
		NULLPTR_FONT,

		/** Null (no data) */
		NULL_VERTEX,
		NULL_INDEX,
		NULL_STRING,

		/** Not enough buffer */
		BUFFER_NOT_ENOUGH,

		/** Invalid type */
		INVALID_CONTROL_TYPE,
	};

	enum class EAnimationDirection
	{
		Left,
		Right,
	};

	enum class EAnimationID
	{
		Idle,
		Walk,
		Jumping,
		Falling,
		Landing,
		Attack1,
		Attack2,
		Attack3,

		Effect,
	};

	struct SMouseData
	{
		POINT MousePosition;
		POINT MouseDownPosition;
		bool bMouseLeftButtonPressed;
		bool bMouseRightButtonPressed;
		bool bOnMouseMove;

		SMouseData() : bMouseLeftButtonPressed(false), bOnMouseMove(false) {};
	};

	struct SAnimationData
	{
		EAnimationID AnimID;
		int FrameS, FrameE;
		bool bForceCycle;
		bool bSetStartFrameEverytime;

		SAnimationData() : FrameS(0), FrameE(0) {};
		SAnimationData(EAnimationID _AnimID, int StartFrame, int EndFrame, bool ForceCycle = false,
			bool SetStartFrameEverytime = false) :
			AnimID(_AnimID), FrameS(StartFrame), FrameE(EndFrame), bForceCycle(ForceCycle),
			bSetStartFrameEverytime(SetStartFrameEverytime) {};
	};

	struct SVertexImage
	{
		FLOAT x, y, z, rhw;
		DWORD color;
		FLOAT u, v;

		SVertexImage() :
			x(0), y(0), z(0), rhw(1), color(0xFFFFFFFF), u(0), v(0) {};
		SVertexImage(float _x, float _y, float _u, float _v) :
			x(_x), y(_y), z(0), rhw(1), color(0xFFFFFFFF), u(_u), v(_v) {};
		SVertexImage(float _x, float _y, DWORD _color) :
			x(_x), y(_y), z(0), rhw(1), color(_color), u(0), v(0) {};
		SVertexImage(float _x, float _y, DWORD _color, float _u, float _v) :
			x(_x), y(_y), z(0), rhw(1), color(_color), u(_u), v(_v) {};
		SVertexImage(float _x, float _y, float _z, float _rhw, DWORD _color, float _u, float _v) :
			x(_x), y(_y), z(_z), rhw(_rhw), color(_color), u(_u), v(_v) {};
	};

	struct SIndex3
	{
		WORD _0, _1, _2;

		SIndex3() :
			_0(0), _1(0), _2(0) {};
		SIndex3(int ID0, int ID1, int ID2) :
			_0(ID0), _1(ID1), _2(ID2) {};
	};

	struct STextureUV
	{
		float u1, u2, v1, v2;

		STextureUV() : u1(0), u2(0), v1(0), v2(0) {};
		STextureUV(float U1, float U2, float V1, float V2) : u1(U1), u2(U2), v1(V1), v2(V2) {};
	};

	struct SLineData
	{
		WSTRING LineText;
		UINT LineSelPosition;
		UINT LineIndex;

		SLineData() : LineSelPosition(0), LineIndex(0) {};
	};

	inline static void ConvertFrameIDIntoUV(int FrameID, POINT SpriteSize, POINT SheetSize, int NumCols, int NumRows, STextureUV* UV)
	{
		int FrameXPos = FrameID % NumCols;
		int FrameYPos = FrameID / NumCols;

		UV->u1 = static_cast<float>(FrameXPos * SpriteSize.x) / static_cast<float>(SheetSize.x);
		UV->u2 = UV->u1 + (static_cast<float>(SpriteSize.x) / static_cast<float>(SheetSize.x));
		UV->v1 = static_cast<float>(FrameYPos * SpriteSize.y) / static_cast<float>(SheetSize.y);
		UV->v2 = UV->v1 + (static_cast<float>(SpriteSize.y) / static_cast<float>(SheetSize.y));
	}

	inline static void ConvertFrameIDIntoUV(int FrameID, D3DXVECTOR2 UnitSize, D3DXVECTOR2 SheetSize, int NumCols, int NumRows,
		STextureUV* UV)
	{
		int FrameXPos = FrameID % NumCols;
		int FrameYPos = FrameID / NumCols;

		UV->u1 = (static_cast<float>(FrameXPos) * UnitSize.x) / SheetSize.x;
		UV->u2 = UV->u1 + (UnitSize.x / SheetSize.x);
		UV->v1 = (static_cast<float>(FrameYPos) * UnitSize.y) / SheetSize.y;
		UV->v2 = UV->v1 + (UnitSize.y / SheetSize.y);
	}

	inline static void GetTileCols(int SheetWidth, int TileWidth, int* TileCols)
	{
		*TileCols = static_cast<int>(SheetWidth / TileWidth);
	}

	inline static void GetTileRows(int SheetHeight, int TileHeight, int* TileRows)
	{
		*TileRows = static_cast<int>(SheetHeight / TileHeight);
	}

	inline void SetColorAlpha(DWORD* Color, BYTE Alpha)
	{
		*Color = (Alpha << 24) | ((*Color << 8) >> 8);
	}

	inline void SetColorXRGB(DWORD* Color, DWORD XRGB)
	{
		*Color = ((*Color >> 24) << 24) | ((XRGB << 8) >> 8);
	}

	inline auto GetColorR(DWORD Color)->BYTE { return ((Color << 8) >> 24); }

	inline auto GetColorG(DWORD Color)->BYTE { return ((Color << 16) >> 24); }

	inline auto GetColorB(DWORD Color)->BYTE { return ((Color << 24) >> 24); }

	inline auto GetColorAlpha(DWORD Color)->BYTE { return (Color >> 24); }

	inline auto GetColorXRGB(DWORD Color)->DWORD { return ((Color << 8) >> 8); }

	inline auto ConvertIntToWSTRING(int In)->WSTRING
	{
		WSTRING Result;
		wchar_t temp[MAX_FILE_LEN]{};
		_itow_s(In, temp, 10);
		Result = temp;
		return Result;
	}

	static auto GetLineDataFromText(WSTRING& Text, UINT SelPosition)->SLineData
	{
		SLineData Result;

		UINT line_count = 0;
		UINT iterator_prev = 0;
		for (UINT iterator = 0; iterator <= Text.length(); iterator++)
		{
			// Check new line('\n') and string end
			if ((Text[iterator] == L'\n') || iterator == Text.length())
			{
				if (SelPosition <= iterator)
				{
					Result.LineText = Text.substr(iterator_prev, iterator - iterator_prev);
					Result.LineSelPosition = SelPosition - iterator_prev;
					Result.LineIndex = line_count;
					return Result;
				}
				iterator_prev = iterator + 1;
				line_count++;
			}
		}
		return Result;
	}
};