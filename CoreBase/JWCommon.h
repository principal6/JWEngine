#pragma once

#include <iostream>
#include <Windows.h>
#include <windowsx.h>
#include <d3dx9.h>
#include <cassert>
#include <vector>
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

	template <typename KeyType, typename ValueType>
	using MAP = std::map<KeyType, ValueType>;

	template <typename T>
	using UNIQUE_PTR = std::unique_ptr<T>;

	template <typename T>
	using SHARED_PTR = std::shared_ptr<T>;

	#define MAKE_UNIQUE(T) std::make_unique<T>
	#define MAKE_SHARED(T) std::make_shared<T>
	#define MAKE_PAIR(Key, Value) std::make_pair(Key, Value)

#endif

// Static function prefix
#define STATIC
// Protected method prefix
#define PROTECTED
// Private method prefix
#define PRIVATE

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWText;
	// ***

	#define JW_SUCCEEDED(func) (func == EError::OK)
	#define JW_FAILED(func) (func != EError::OK)
	#define JW_DELETE_ARRAY(arr) {if(arr) {delete[] arr; arr = nullptr;}}
	#define JW_DESTROY(obj) {if(obj) {obj->Destroy(); delete obj; obj = nullptr;}}
	#define JW_DESTROY_SMART(obj) {if(obj) {obj->Destroy();}}
	#define JW_RELEASE(obj) {if(obj) {obj->Release(); obj = nullptr;}}

	// [DEBUG] abort() and return nullptr
	#define ABORT_RETURN_NULLPTR {abort(); return nullptr;}

	using CINT = const int;

	using THandleItem = unsigned long long;
	static constexpr THandleItem THandleItem_Null = UINT64_MAX;

	using TIndex = size_t;
	static const size_t TIndex_NotSpecified = static_cast<size_t>(-1);
	static const size_t SIZE_T_INVALID = static_cast<size_t>(-1);

	static constexpr int MAX_FILE_LEN = 260;

	// @warning: This value must be 256 for Direct Input
	static constexpr int NUM_KEYS = 256;

	static constexpr int MAX_UNIT_COUNT = 100;

	static const wchar_t GUI_TEXTURE_FILENAME[] = L"jwgui_button.png";
	static const D3DXVECTOR2& GUI_BUTTON_SIZE = D3DXVECTOR2(15.0f, 15.0f);

	const wchar_t ASSET_DIR[] = L"\\Asset\\";
	const wchar_t DEFAULT_FONT[] = L"dotumche12all.fnt";

	static const DWORD D3DFVF_TEXTURE = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	static const DWORD D3DFVF_LINE = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

	static const DWORD DEFAULT_COLOR_BLACK = D3DCOLOR_XRGB(0, 0, 0);
	static const DWORD DEFAULT_COLOR_ALMOST_BLACK = D3DCOLOR_XRGB(35, 35, 35);
	static const DWORD DEFAULT_COLOR_LESS_BLACK = D3DCOLOR_XRGB(70, 70, 70);
	static const DWORD DEFAULT_COLOR_NORMAL = D3DCOLOR_XRGB(105, 105, 105);
	static const DWORD DEFAULT_COLOR_HOVER = D3DCOLOR_XRGB(140, 140, 140);
	static const DWORD DEFAULT_COLOR_PRESSED = D3DCOLOR_XRGB(80, 80, 255);
	static const DWORD DEFAULT_COLOR_DARK_HIGHLIGHT = D3DCOLOR_XRGB(20, 20, 160);
	static const DWORD DEFAULT_COLOR_BORDER = DEFAULT_COLOR_HOVER;
	static const DWORD DEFAULT_COLOR_BORDER_ACTIVE = DEFAULT_COLOR_DARK_HIGHLIGHT;
	static const DWORD DEFAULT_COLOR_LESS_WHITE = D3DCOLOR_XRGB(185, 185, 185);
	static const DWORD DEFAULT_COLOR_ALMOST_WHITE = D3DCOLOR_XRGB(220, 220, 220);
	static const DWORD DEFAULT_COLOR_WHITE = D3DCOLOR_XRGB(255, 255, 255);

	static const DWORD DEFAULT_COLOR_FONT = DEFAULT_COLOR_WHITE;
	static const DWORD DEFAULT_COLOR_WATERMARK = DEFAULT_COLOR_LESS_WHITE;

	static const wchar_t DEFAULT_EDIT_WATERMARK[] = L"JWEdit Control";

	enum class EError
	{
		/** No error */
		OK,

		/** Win32Api, DirectX */
		WINAPIWINDOW_NOT_CREATED,
		DIRECTX_NOT_CREATED,
		DEVICE_NOT_CREATED,
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
		TEXT_NOT_CREATED,
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
		IMAGE_BUTTON_NOT_CREATED,
		IMAGE_BOX_NOT_CREATED,
		SCROLLBAR_NOT_CREATED,

		/** Null pointer */
		NULLPTR_DEVICE,
		NULLPTR_WINDOW,
		NULLPTR_WINDOWDATA,
		NULLPTR_MAP,
		NULLPTR_MAP_INFO,
		NULLPTR_IMAGE,
		NULLPTR_FONT,
		NULLPTR_SCROLLBAR,
		NULLPTR_VERTEX,
		NULLPTR_INDEX,

		/** Not enough buffer */
		BUFFER_NOT_ENOUGH,

		/** Invalid type */
		INVALID_CONTROL_TYPE,

		/** Duplicate creation */
		DUPLICATE_CREATION,

		/** Memory allocation failure */
		ALLOCATION_FAILURE,
	};

	enum class EHorizontalAlignment
	{
		Left,
		Center,
		Right,
	};

	enum class EVerticalAlignment
	{
		Top,
		Middle,
		Bottom,
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

	struct SWindowInputState
	{
		// True while the left button is pressed.
		bool MouseLeftPressed;

		// True only when the left button is pressed for the first time.
		bool MouseLeftFirstPressed;

		bool MouseLeftReleased;
		bool MouseRightPressed;
		bool MouseRightReleased;
		bool ControlPressed;
		bool AltPressed;
		bool ShiftPressed;
		POINT MousePosition;
		POINT MouseDownPosition;
		POINT MouseMovedPosition;
		int MouseWheeled;

		SWindowInputState() : MouseLeftPressed(false), MouseLeftFirstPressed(false), MouseLeftReleased(false),
			MouseRightPressed(false), MouseRightReleased(false),
			ControlPressed(false), AltPressed(false), ShiftPressed(false),
			MousePosition({ 0, 0 }), MouseDownPosition({ 0, 0 }), MouseMovedPosition({ 0, 0 }), MouseWheeled(0) {};
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
		SIndex3(size_t ID0, size_t ID1, size_t ID2)
		{
			_0 = static_cast<WORD>(ID0);
			_1 = static_cast<WORD>(ID1);
			_2 = static_cast<WORD>(ID2);
		}
	};

	struct STextureUV
	{
		float u1, u2, v1, v2;

		STextureUV() : u1(0), u2(0), v1(0), v2(0) {};
		STextureUV(float U1, float U2, float V1, float V2) : u1(U1), u2(U2), v1(V1), v2(V2) {};
	};

	enum class EWindowStyle : DWORD
	{
		Overlapped = WS_OVERLAPPED,
		Popup = WS_POPUP,
		Child = WS_CHILD,
		Minimize = WS_MINIMIZE,
		Visible = WS_VISIBLE,
		Disabled = WS_DISABLED,
		ClipSiblings = WS_CLIPSIBLINGS,
		ClipChildren = WS_CLIPCHILDREN,
		Maximize = WS_MAXIMIZE,
		Caption = WS_CAPTION, // = WS_BORDER | WS_DLGFRAME
		Border = WS_BORDER,
		DlgFrame = WS_DLGFRAME,
		VScroll = WS_VSCROLL,
		HScroll = WS_HSCROLL,
		SysMenu = WS_SYSMENU,
		ThickFrame = WS_THICKFRAME,
		Group = WS_GROUP,
		TabStop = WS_TABSTOP,
		MinimizeBox = WS_MINIMIZEBOX,
		MaximizeBox = WS_MAXIMIZEBOX,
		OverlappedWindow = WS_OVERLAPPEDWINDOW, // = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
		PopupWindow = WS_POPUPWINDOW, // = WS_POPUP | WS_BORDER | WS_SYSMENU
		ChildWindow = WS_CHILDWINDOW,
		Dialogue = WS_DLGFRAME | WS_SYSMENU,
	};

	DEFINE_ENUM_FLAG_OPERATORS(EWindowStyle);

	struct SWindowCreationData
	{
		WSTRING caption;
		int x;
		int y;
		unsigned int width;
		unsigned int height;
		DWORD color_background;
		WNDPROC proc;
		EWindowStyle window_style;
		HWND parent_hwnd;
		
		SWindowCreationData() : x(0), y(0), width(300), height(200), color_background(D3DCOLOR_XRGB(0, 0, 0)),
			proc(nullptr), window_style(EWindowStyle::OverlappedWindow), parent_hwnd(nullptr) {};

		SWindowCreationData(WSTRING _caption, int _x, int _y, unsigned int _width, unsigned int _height, DWORD _color_background,
			EWindowStyle _window_style = EWindowStyle::OverlappedWindow, HWND _parent_hwnd = nullptr) :
			caption(_caption), x(_x), y(_y), width(_width), height(_height), color_background(_color_background),
			proc(nullptr), window_style(_window_style), parent_hwnd(_parent_hwnd) {};
	};

	// This structure contains data that will be shared in a JWGUIWindow.
	struct SGUIWindowSharedData
	{
		JWWindow* pWindow;
		WSTRING BaseDir;
		LPDIRECT3DTEXTURE9 Texture_GUI;
		D3DXIMAGE_INFO Texture_GUI_Info;
		JWText* pText;

		SGUIWindowSharedData() : pWindow(nullptr), Texture_GUI(nullptr), pText(nullptr) {};
	};

	struct SGUIIMEInputInfo
	{
		TCHAR IMEWritingChar[MAX_FILE_LEN];
		TCHAR IMECompletedChar[MAX_FILE_LEN];
		bool bIMEWriting;
		bool bIMECompleted;

		SGUIIMEInputInfo()
		{
			clear();
		};

		void clear()
		{
			memset(IMEWritingChar, 0, sizeof(TCHAR) * MAX_FILE_LEN);
			memset(IMECompletedChar, 0, sizeof(TCHAR) * MAX_FILE_LEN);
			bIMEWriting = false;
			bIMECompleted = false;
		}
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

	inline auto GetMixedColor(BYTE Alpha, DWORD XRGB)->const DWORD
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

	inline auto ConvertIntToWSTRING(int In)->WSTRING
	{
		WSTRING Result;

		wchar_t temp[MAX_FILE_LEN]{};
		_itow_s(In, temp, 10);
		Result = temp;

		return Result;
	}

	// Template function
	template <typename T>
	static void Swap(T& v1, T& v2)
	{
		T temp = v1;
		v1 = v2;
		v2 = temp;
	}

	template <typename T>
	static void SwapPointer(T*& v1, T*& v2)
	{
		T* temp = v1;
		v1 = v2;
		v2 = temp;
	}

	static void SIZE_T_MINUS(size_t& var, const size_t value)
	{
		if (var >= value)
		{
			var -= value;
		}
	}

	static void SIZE_T_PLUS(size_t& var, const size_t value, const size_t& upper_limit)
	{
		if (var + value <= upper_limit)
		{
			var += value;
		}
	}
};