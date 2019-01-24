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
	};

	struct SWindowData
	{
		int WindowWidth;
		int WindowHeight;
		float WindowHalfWidth;
		float WindowHalfHeight;

		SWindowData() : WindowWidth(0), WindowHeight(0), WindowHalfWidth(0), WindowHalfHeight(0) {};
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

	class JWWindow final
	{
	public:
		JWWindow();
		~JWWindow() {};

		auto JWWindow::CreateGameWindow(CINT X, CINT Y, CINT Width, CINT Height)->EError;
		auto JWWindow::CreateParentWindow(CINT X, CINT Y, CINT Width, CINT Height, DWORD Color,
			WNDPROC Proc, LPCWSTR MenuName)->EError;
		auto JWWindow::CreateChildWindow(HWND hWndParent, CINT X, CINT Y, CINT Width, CINT Height,
			DWORD Color, WNDPROC Proc)->EError;
		void JWWindow::Destroy();
		void JWWindow::GameWindowHandleMessage();

		// Scrollbars
		void JWWindow::UseVerticalScrollbar();
		void JWWindow::UseHorizontalScrollbar();
		void JWWindow::SetVerticalScrollbarRange(int Max);
		void JWWindow::SetHorizontalScrollbarRange(int Max);
		auto JWWindow::GetVerticalScrollbarPosition()->int;
		auto JWWindow::GetHorizontalScrollbarPosition()->int;

		void JWWindow::SetWindowCaption(WSTRING Caption);
		void JWWindow::SetBackgroundColor(D3DCOLOR color);
		void JWWindow::Resize(RECT Rect);

		void JWWindow::BeginRender() const;
		void JWWindow::EndRender() const;

		auto JWWindow::GetDevice() const->LPDIRECT3DDEVICE9;
		auto JWWindow::GethWnd() const->HWND;
		auto JWWindow::GethInstance() const->HINSTANCE;
		auto JWWindow::GetWindowData()->SWindowData*;
		auto JWWindow::GetMouseData()->SMouseData*;
		auto JWWindow::GetRenderRect()->RECT;

		// Dialog
		void JWWindow::SetDlgBase();
		auto JWWindow::OpenFileDlg(LPCWSTR Filter)->BOOL;
		auto JWWindow::SaveFileDlg(LPCWSTR Filter)->BOOL;
		auto JWWindow::GetDlgFileName()->WSTRING;
		auto JWWindow::GetDlgFileTitle()->WSTRING;

		// Editor message handler
		void JWWindow::EditorChildWindowMessageHandler(UINT Message, WPARAM wParam, LPARAM lParam);
		auto JWWindow::IsMouseLeftButtonPressed() const->bool;
		auto JWWindow::IsMouseRightButtonPressed() const->bool;
		auto JWWindow::OnMouseMove()->bool;

	private:
		friend LRESULT CALLBACK GameWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

		auto JWWindow::CreateWINAPIWindow(const wchar_t* Name, CINT X, CINT Y, CINT Width, CINT Height,
			EWindowStyle WindowStyle, DWORD BackColor, WNDPROC Proc, LPCWSTR MenuName = nullptr, HWND hWndParent = nullptr)->HWND;
		auto JWWindow::InitializeDirectX()->int;
		void JWWindow::SetDirect3DParameters();
		void JWWindow::UpdateRenderRect();

		void JWWindow::SetWindowData(int Width, int Height);
		void JWWindow::UpdateVerticalScrollbarPosition();
		void JWWindow::UpdateHorizontalScrollbarPosition();

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