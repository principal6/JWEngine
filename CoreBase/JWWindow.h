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

	class JWWindow final
	{
	public:
		JWWindow();
		~JWWindow() {};

		auto CreateGameWindow(CInt X, CInt Y, CInt Width, CInt Height)->EError;
		auto CreateGUIWindow(const SWindowCreationData& WindowCreationData)->EError;
		void Destroy();

		void SetWindowCaption(const WSTRING Caption);
		void SetBackgroundColor(const D3DCOLOR color);
		void Resize(const RECT Rect);

		// 1) Clear()
		// 2) BeginScene()
		void BeginRender() const;

		// 1) EndScene()
		// 2) Present().
		// 3) Check if the device is lost, and if it is, reset it.
		void EndRender();

		auto GetDevice() const->const LPDIRECT3DDEVICE9;
		auto GethWnd() const->const HWND;
		auto GethInstance() const->const HINSTANCE;
		auto GetWindowData() const->const SWindowData*;
		auto GetRenderRect() const->const RECT;

		// Input
		void UpdateWindowInputState(const MSG& Message);
		auto GetWindowInputStatePtr() const->const SWindowInputState*;

		// Dialog
		void SetDlgBase();
		auto OpenFileDlg(LPCWSTR Filter)->BOOL;
		auto SaveFileDlg(LPCWSTR Filter)->BOOL;
		auto GetDlgFileName() const->const WSTRING;
		auto GetDlgFileTitle() const->const WSTRING;

	private:
		friend LRESULT CALLBACK BaseWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

		auto CreateWINAPIWindow(const wchar_t* Name, CInt X, CInt Y, CInt Width, CInt Height,
			EWindowStyle WindowStyle, DWORD BackColor, WNDPROC Proc, LPCWSTR MenuName = nullptr, HWND hWndParent = nullptr)->HWND;
		auto InitializeDirectX()->EError;
		void SetDirect3DParameters();
		void UpdateRenderRect();

		void SetWindowData(CInt Width, CInt Height);

	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		RECT m_Rect;
		mutable RECT m_RenderRect;
		SWindowData m_WindowData;
		SWindowInputState m_WindowInputState;

		LPDIRECT3D9 m_pD3D;
		LPDIRECT3DDEVICE9 m_pDevice;
		D3DPRESENT_PARAMETERS m_D3DPP;
		D3DCOLOR m_BGColor;

		OPENFILENAME m_OFN;
		TCHAR m_FileName[MAX_FILE_LEN];
		TCHAR m_FileTitle[MAX_FILE_LEN];
	};
};