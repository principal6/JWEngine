#pragma once

#include <iostream>
#include <Windows.h>
#include <windowsx.h>
#include <d3dx9.h>
#include <cassert>
#include <vector>
#include <list>
#include <memory>
#include <map>
#include <crtdbg.h>
//#include <string>

#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "Imm32.lib")

#ifndef CONVENIENT_STD
#define CONVENIENT_STD
	using STRING = std::string;
	using WSTRING = std::wstring;

	using WIFSTREAM = std::wifstream;
	using WOFSTREAM = std::wofstream;

	template <typename T>
	using VECTOR = std::vector<T>;

	template <typename T>
	using LIST = std::list<T>;

	template <typename KeyType, typename ValueType>
	using MAP = std::map<KeyType, ValueType>;

	template <typename T>
	using UNIQUE_PTR = std::unique_ptr<T>;

	template <typename T>
	using SHARED_PTR = std::shared_ptr<T>;

	#define MAKE_UNIQUE(T) std::make_unique<T>
	#define MAKE_SHARED(T) std::make_shared<T>
	#define MAKE_PAIR(Key, Value) std::make_pair(Key, Value)
	#define MOVE(T) std::move(T)
	#define MAKE_UNIQUE_AND_MOVE(T) MOVE(MAKE_UNIQUE(T))
#endif

// Static function prefix
#define STATIC
// Protected method prefix
#define PROTECTED
// Private method prefix
#define PRIVATE

static constexpr int MAX_FILE_LEN = 260;

struct STextureUV
{
	float u1{};
	float u2{};
	float v1{};
	float v2{};

	STextureUV() {};
	STextureUV(float U1, float U2, float V1, float V2) : u1(U1), u2(U2), v1(V1), v2(V2) {};
};

inline void ConvertFrameIDIntoUV(int FrameID, POINT SpriteSize, POINT SheetSize, int NumCols, int NumRows, STextureUV* UV)
{
	int FrameXPos = FrameID % NumCols;
	int FrameYPos = FrameID / NumCols;

	UV->u1 = static_cast<float>(FrameXPos * SpriteSize.x) / static_cast<float>(SheetSize.x);
	UV->u2 = UV->u1 + (static_cast<float>(SpriteSize.x) / static_cast<float>(SheetSize.x));
	UV->v1 = static_cast<float>(FrameYPos * SpriteSize.y) / static_cast<float>(SheetSize.y);
	UV->v2 = UV->v1 + (static_cast<float>(SpriteSize.y) / static_cast<float>(SheetSize.y));
}

inline void ConvertFrameIDIntoUV(int FrameID, D3DXVECTOR2 UnitSize, D3DXVECTOR2 SheetSize, int NumCols, int NumRows,
	STextureUV* UV)
{
	int FrameXPos = FrameID % NumCols;
	int FrameYPos = FrameID / NumCols;

	UV->u1 = (static_cast<float>(FrameXPos) * UnitSize.x) / SheetSize.x;
	UV->u2 = UV->u1 + (UnitSize.x / SheetSize.x);
	UV->v1 = (static_cast<float>(FrameYPos) * UnitSize.y) / SheetSize.y;
	UV->v2 = UV->v1 + (UnitSize.y / SheetSize.y);
}

inline void GetTileCols(int SheetWidth, int TileWidth, int* TileCols)
{
	*TileCols = static_cast<int>(SheetWidth / TileWidth);
}

inline void GetTileRows(int SheetHeight, int TileHeight, int* TileRows)
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

inline auto GetMixedColor(BYTE Alpha, DWORD XRGB)->DWORD
{
	DWORD new_color;

	SetColorAlpha(&new_color, Alpha);
	SetColorXRGB(&new_color, XRGB);

	return new_color;
}

inline auto GetColorR(DWORD Color)->BYTE { return ((Color << 8) >> 24); }

inline auto GetColorG(DWORD Color)->BYTE { return ((Color << 16) >> 24); }

inline auto GetColorB(DWORD Color)->BYTE { return ((Color << 24) >> 24); }

inline auto GetColorAlpha(DWORD Color)->BYTE { return (Color >> 24); }

inline auto GetColorXRGB(DWORD Color)->DWORD { return ((Color << 8) >> 8); }

inline auto ConvertIntToWSTRING(int value)->WSTRING
{
	wchar_t temp[MAX_FILE_LEN]{};
	swprintf_s(temp, L"%d", value);

	return WSTRING(temp);
}

inline auto ConvertLongLongToWSTRING(long long value)->WSTRING
{
	wchar_t temp[MAX_FILE_LEN]{};
	swprintf_s(temp, L"%lld", value);

	return WSTRING(temp);
}

inline auto ConvertLongLongToSTRING(long long value)->STRING
{
	char temp[MAX_FILE_LEN]{};
	sprintf_s(temp, "%lld", value);

	return STRING(temp);
}

inline auto ConvertFloatToWSTRING(float value)->WSTRING
{
	wchar_t temp[MAX_FILE_LEN]{};
	swprintf_s(temp, L"%f", value);

	return WSTRING(temp);
}

// Template function
template <typename T>
inline void Swap(T& v1, T& v2)
{
	T temp = v1;
	v1 = v2;
	v2 = temp;
}

template <typename T>
inline void SwapPointer(T*& v1, T*& v2)
{
	T* temp = v1;
	v1 = v2;
	v2 = temp;
}

inline void SIZE_T_MINUS(size_t& var, const size_t value)
{
	if (var >= value)
	{
		var -= value;
	}
}

inline void SIZE_T_PLUS(size_t& var, const size_t value, const size_t& upper_limit)
{
	if (var + value <= upper_limit)
	{
		var += value;
	}
}

inline void LOG_DEBUG(WSTRING wide_string)
{
	std::wcout << wide_string.c_str() << std::endl;
}

inline void LOG_DEBUG(STRING string)
{
	std::cout << string.c_str() << std::endl;
}

static auto WstringToString(WSTRING Source)->STRING
{
	STRING Result;

	char* temp = nullptr;
	size_t len = static_cast<size_t>(WideCharToMultiByte(CP_ACP, 0, Source.c_str(), -1, temp, 0, nullptr, nullptr));

	temp = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, Source.c_str(), -1, temp, static_cast<int>(len), nullptr, nullptr);

	Result = temp;

	delete[] temp;
	temp = nullptr;
	return Result;
}

static auto StringToWstring(STRING Source)->WSTRING
{
	WSTRING Result;

	wchar_t* temp = nullptr;
	size_t len = static_cast<size_t>(MultiByteToWideChar(CP_ACP, 0, Source.c_str(), -1, temp, 0));

	temp = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, Source.c_str(), -1, temp, static_cast<int>(len));

	Result = temp;

	delete[] temp;
	temp = nullptr;
	return Result;
}