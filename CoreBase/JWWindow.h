#pragma once

#include "JWCommon.h"

namespace JWENGINE
{
	enum class EWindowStyle : DWORD
	{
		Overlapped = WS_OVERLAPPED,
		Popup = WS_POPUP,
		Child = WS_CHILD,
		Minimize = WS_MINIMIZE,
		Visible = WS_VISIBLE,
		DIsabled = WS_DISABLED,
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
		ChildWindow = WS_CHILDWINDOW, // = WS_CHILD
		ChildWindow2 = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		ThickPopup = WS_POPUP | WS_THICKFRAME,
		BorderPopup = WS_POPUP | WS_BORDER,
		Dialogue = WS_DLGFRAME | WS_SYSMENU,
	};

	struct SWindowData
	{
		int WindowWidth;
		int WindowHeight;
		float WindowHalfWidth;
		float WindowHalfHeight;
		POINT ScreenSize;

		SWindowData() : WindowWidth(0), WindowHeight(0), WindowHalfWidth(0), WindowHalfHeight(0), ScreenSize{0, 0} {};
	};

	struct SWindowInputState
	{
		bool MouseLeftPressed;
		bool MouseLeftReleased;
		bool MouseRightPressed;
		bool ControlPressed;
		bool AltPressed;
		bool ShiftPressed;
		POINT MousePosition;
		POINT MouseDownPosition;

		SWindowInputState() : MouseLeftPressed(false), MouseLeftReleased(false), MouseRightPressed(false),
			ControlPressed(false), AltPressed(false), ShiftPressed(false),
			MousePosition({ 0, 0 }), MouseDownPosition({ 0, 0 }) {};
	};

	class JWWindow final
	{
	public:
		JWWindow();
		~JWWindow() {};

		auto CreateGameWindow(CINT X, CINT Y, CINT Width, CINT Height)->EError;
		auto CreateGUIWindow(CINT X, CINT Y, CINT Width, CINT Height, DWORD Color, WNDPROC Proc)->EError;
		auto CreateGUIDialogue(CINT X, CINT Y, CINT Width, CINT Height, DWORD Color, WNDPROC Proc)->EError;
		auto CreateParentWindow(CINT X, CINT Y, CINT Width, CINT Height, DWORD Color,
			WNDPROC Proc, LPCWSTR MenuName)->EError;
		auto CreateChildWindow(HWND hWndParent, CINT X, CINT Y, CINT Width, CINT Height,
			DWORD Color, WNDPROC Proc)->EError;
		void Destroy();

		// Scrollbars
		void UseVerticalScrollbar();
		void UseHorizontalScrollbar();
		void SetVerticalScrollbarRange(int Max);
		void SetHorizontalScrollbarRange(int Max);
		auto GetVerticalScrollbarPosition()->int;
		auto GetHorizontalScrollbarPosition()->int;

		void SetWindowCaption(WSTRING Caption);
		void SetBackgroundColor(D3DCOLOR color);
		void Resize(RECT Rect);

		// Render
		void BeginRender() const;
		void EndRender();

		auto GetDevice() const->const LPDIRECT3DDEVICE9;
		auto GethWnd() const->const HWND;
		auto GethInstance() const->const HINSTANCE;
		auto GetWindowData() const->const SWindowData*;
		auto GetMouseData() const->const SMouseData*;
		auto GetRenderRect() const->const RECT;

		// Input
		void UpdateInputState();
		auto GetWindowInputState() const->const SWindowInputState*;

		// Dialog
		void SetDlgBase();
		auto OpenFileDlg(LPCWSTR Filter)->BOOL;
		auto SaveFileDlg(LPCWSTR Filter)->BOOL;
		auto GetDlgFileName()->WSTRING;
		auto GetDlgFileTitle()->WSTRING;

		// Editor message handler
		void EditorChildWindowMessageHandler(UINT Message, WPARAM wParam, LPARAM lParam);

	private:
		friend LRESULT CALLBACK BaseWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
		auto CreateWINAPIWindow(const wchar_t* Name, CINT X, CINT Y, CINT Width, CINT Height,
			EWindowStyle WindowStyle, DWORD BackColor, WNDPROC Proc, LPCWSTR MenuName = nullptr, HWND hWndParent = nullptr)->HWND;
		auto InitializeDirectX()->int;
		void SetDirect3DParameters();
		void UpdateRenderRect();

		void SetWindowData(int Width, int Height);
		void UpdateVerticalScrollbarPosition();
		void UpdateHorizontalScrollbarPosition();

	private:
		static const int VERTICAL_SCROLL_BAR_WIDTH = 20;
		static const int HORIZONTAL_SCROLL_BAR_HEIGHT = 20;
		static int ms_ChildWindowCount;
		
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		RECT m_Rect;
		mutable RECT m_RenderRect;
		SWindowData m_WindowData;
		SMouseData m_MouseData;
		SWindowInputState m_InputState;

		HWND m_hVerticalScrollbar;
		HWND m_hHorizontalScrollbar;

		LPDIRECT3D9 m_pD3D;
		LPDIRECT3DDEVICE9 m_pDevice;
		D3DPRESENT_PARAMETERS m_D3DPP;
		D3DCOLOR m_BGColor;

		OPENFILENAME m_OFN;
		TCHAR m_FileName[MAX_FILE_LEN];
		TCHAR m_FileTitle[MAX_FILE_LEN];
	};
};