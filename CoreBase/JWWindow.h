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

		void CreateGameWindow(const SWindowCreationData& WindowCreationData);
		void CreateGUIWindow(const SWindowCreationData& WindowCreationData);
		void Destroy() noexcept;
		
		void SetWindowCaption(const WSTRING& Caption) noexcept;
		void SetBackgroundColor(D3DCOLOR color) noexcept;
		void Resize(const RECT& Rect) noexcept;

		// 1) Clear()
		// 2) BeginScene()
		void BeginRender() const noexcept;

		// 1) EndScene()
		// 2) Present().
		// 3) Check if the device is lost, and if it is, reset it.
		void EndRender() noexcept;

		// This function does not return nullptr!
		auto GetDevice() const noexcept->const LPDIRECT3DDEVICE9;

		auto GethWnd() const noexcept->const HWND;
		auto GethInstance() const noexcept->const HINSTANCE;
		auto GetWindowData() const noexcept->const SWindowData*;
		auto GetRenderRect() const noexcept->const RECT;

		// InputState
		void UpdateWindowInputState(const MSG& Message) noexcept;
		auto GetWindowInputStatePtr() const noexcept->const SWindowInputState*;

		// Dialog
		void SetDlgBase() noexcept;
		auto OpenFileDlg(LPCWSTR Filter) noexcept->BOOL;
		auto SaveFileDlg(LPCWSTR Filter) noexcept->BOOL;
		auto GetDlgFileName() const noexcept->const WSTRING;
		auto GetDlgFileTitle() const noexcept->const WSTRING;

	private:
		friend LRESULT CALLBACK BaseWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

		auto CreateWINAPIWindow(const wchar_t* Name, const wchar_t* Caption, int X, int Y, int Width, int Height,
			EWindowStyle WindowStyle, DWORD BackColor, WNDPROC Proc, LPCWSTR MenuName = nullptr, HWND hWndParent = nullptr)->HWND;
		void SetWindowData(int Width, int Height) noexcept;
		void InitializeDirectX();
		void SetDirect3DParameters() noexcept;
		void UpdateRenderRect() noexcept;

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

		OPENFILENAMEW m_OFN;
		wchar_t m_FileName[MAX_FILE_LEN];
		wchar_t m_FileTitle[MAX_FILE_LEN];
	};
};