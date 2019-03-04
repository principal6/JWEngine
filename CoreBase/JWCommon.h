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
	class JWGUIWindow;
	class JWWindow;
	class JWText;
	// ***

	#define JW_DELETE_ARRAY(arr) {if(arr) {delete[] arr; arr = nullptr;}}
	#define JW_DESTROY(obj) {if(obj) {obj->Destroy(); delete obj; obj = nullptr;}}
	#define JW_DESTROY_SMART(obj) {if(obj) {obj->Destroy();}}
	#define JW_RELEASE(obj) {if(obj) {obj->Release(); obj = nullptr;}}
	
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
		bool MouseLeftPressed = false;

		// True only when the left button is pressed for the first time.
		bool MouseLeftFirstPressed = false;

		bool MouseLeftReleased = false;
		bool MouseRightPressed = false;
		bool MouseRightReleased = false;
		bool ControlPressed = false;
		bool AltPressed = false;
		bool ShiftPressed = false;
		POINT MousePosition{};
		POINT MouseDownPosition{};
		POINT MouseMovedPosition{};
		int MouseWheeled = 0;

		SWindowInputState() {};
	};

	struct SAnimationData
	{
		EAnimationID AnimID = EAnimationID::Idle;
		int FrameS = 0;
		int FrameE = 0;
		bool bForceCycle = false;
		bool bSetStartFrameEverytime = false;

		SAnimationData() {};
		SAnimationData(EAnimationID _AnimID, int StartFrame, int EndFrame, bool ForceCycle = false,
			bool SetStartFrameEverytime = false) :
			AnimID(_AnimID), FrameS(StartFrame), FrameE(EndFrame), bForceCycle(ForceCycle),
			bSetStartFrameEverytime(SetStartFrameEverytime) {};
	};

	struct SVertexImage
	{
		FLOAT x = 0;
		FLOAT y = 0;
		FLOAT z = 0;
		FLOAT rhw = 0;
		DWORD color = 0xFFFFFFFF;
		FLOAT u = 0;
		FLOAT v = 0;

		SVertexImage() {};
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
		WORD _0 = 0;
		WORD _1 = 0;
		WORD _2 = 0;

		SIndex3() {};
		SIndex3(int ID0, int ID1, int ID2) : _0(ID0), _1(ID1), _2(ID2) {};
		SIndex3(size_t ID0, size_t ID1, size_t ID2) : _0(static_cast<WORD>(ID0)), _1(static_cast<WORD>(ID1)), _2(static_cast<WORD>(ID2)) {};
	};

	struct STextureUV
	{
		float u1 = 0;
		float u2 = 0;
		float v1 = 0;
		float v2 = 0;

		STextureUV() {};
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
		int x = 0;
		int y = 0;
		unsigned int width = 0;
		unsigned int height = 0;
		DWORD color_background = 0xFFFFFFFF;
		WNDPROC proc = nullptr;
		EWindowStyle window_style = EWindowStyle::OverlappedWindow;
		
		SWindowCreationData() : width(300), height(200) {};
		SWindowCreationData(WSTRING _caption, int _x, int _y, unsigned int _width, unsigned int _height, DWORD _color_background,
			EWindowStyle _window_style = EWindowStyle::OverlappedWindow) :
			caption(_caption), x(_x), y(_y), width(_width), height(_height), color_background(_color_background),
			proc(nullptr), window_style(_window_style) {};
	};

	// This structure contains data that will be shared in a JWGUIWindow.
	struct SGUIWindowSharedData
	{
		JWGUIWindow* pGUIWindow = nullptr;
		JWWindow* pWindow = nullptr;
		JWText* pText = nullptr;
		WSTRING BaseDir;
		LPDIRECT3DTEXTURE9 Texture_GUI = nullptr;
		D3DXIMAGE_INFO Texture_GUI_Info{};
	};

	struct SGUIIMEInputInfo
	{
		TCHAR IMEWritingChar[MAX_FILE_LEN]{};
		TCHAR IMECompletedChar[MAX_FILE_LEN]{};
		bool bIMEWriting = false;
		bool bIMECompleted = false;

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

	/** EXCEPTION HANDLERS */
	class exception_base : public std::exception
	{
	public:
		exception_base(const char* function_name, const std::type_info& type_id) noexcept
		{
			m_error_content = "[EXCEPTION] ";
			m_error_content += "this=";
			m_error_content += type_id.name();
			m_error_content += " \'";
			m_error_content += function_name;
			m_error_content += "()\' <";
		}

		auto what() const->const char* override
		{
			return m_error_content.c_str();
		}

	protected:
		std::string m_error_content;
	};

	class creation_failed : public exception_base
	{
	public:
		creation_failed(const char* function_name, const std::type_info& type_id) noexcept : exception_base(function_name, type_id)
		{
			m_error_content += "creation failed>";
		}
	};

	class duplicate_creation : public exception_base
	{
	public:
		duplicate_creation(const char* function_name, const std::type_info& type_id) noexcept : exception_base(function_name, type_id)
		{
			m_error_content += "duplicate creation>";
		}
	};

	class name_collision : public exception_base
	{
	public:
		name_collision(const char* function_name, const std::type_info& type_id, const wchar_t* name) noexcept : exception_base(function_name, type_id)
		{
			m_error_content += "name already exists> ";

			m_name = L"\'";
			m_name += name;
			m_name += L"\'";

			m_error_content += WstringToString(m_name);
		}

	private:
		std::wstring m_name;
	};

	class nullptr_assigned : public exception_base
	{
	public:
		nullptr_assigned(const char* function_name, const std::type_info& type_id) noexcept : exception_base(function_name, type_id)
		{
			m_error_content += "null pointer assigned>";
		}
	};

	class nullptr_accessed : public exception_base
	{
	public:
		nullptr_accessed(const char* function_name, const std::type_info& type_id) noexcept : exception_base(function_name, type_id)
		{
			m_error_content += "null pointer accessed>";
		}
	};

	class not_found : public exception_base
	{
	public:
		not_found(const char* function_name, const std::type_info& type_id) noexcept : exception_base(function_name, type_id)
		{
			m_error_content += "not found>";
		}
	};

	class dxinput_failed : public exception_base
	{
	public:
		dxinput_failed(const char* function_name, const std::type_info& type_id) noexcept : exception_base(function_name, type_id)
		{
			m_error_content += "directx input failed>";
		}
	};

	#define THROW_CREATION_FAILED throw creation_failed(__func__, typeid(this))
	#define THROW_DUPLICATE_CREATION throw duplicate_creation(__func__, typeid(this))
	#define THROW_NULLPTR_ACCESSED throw nullptr_accessed(__func__, typeid(this))
	#define THROW_NOT_FOUND throw not_found(__func__, typeid(this))
	#define THROW_DXINPUT_FAILED throw dxinput_failed(__func__, typeid(this))
	#define THROW_NAME_COLLISION(wchar_name) throw name_collision(__func__, typeid(this), wchar_name)
};