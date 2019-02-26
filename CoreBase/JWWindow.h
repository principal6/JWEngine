#pragma once

#include "JWCommon.h"

namespace JWENGINE
{
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

		auto CreateGameWindow(const SWindowCreationData& WindowCreationData)->EError;
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

		auto CreateWINAPIWindow(const wchar_t* Name, const wchar_t* Caption, CINT X, CINT Y, CINT Width, CINT Height,
			EWindowStyle WindowStyle, DWORD BackColor, WNDPROC Proc, LPCWSTR MenuName = nullptr, HWND hWndParent = nullptr)->HWND;
		auto InitializeDirectX()->EError;
		void SetDirect3DParameters();
		void UpdateRenderRect();

		void SetWindowData(CINT Width, CINT Height);

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