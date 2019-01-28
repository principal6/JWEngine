#include "JWWindow.h"

using namespace JWENGINE;

// Static member variable
int JWWindow::ms_ChildWindowCount = 0;

// Base window procedure for Game/GUI window
LRESULT CALLBACK BaseWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_SYSCOMMAND:
		if (wParam == SC_KEYMENU && (lParam >> 16) <= 0) // Disable Alt key
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

JWWindow::JWWindow()
{
	m_hInstance = nullptr;
	m_hWnd = nullptr;
	m_pD3D = nullptr;
	m_pDevice = nullptr;
	m_BGColor = D3DCOLOR_XRGB(0, 0, 0);

	memset(m_FileName, 0, MAX_FILE_LEN);
	memset(m_FileTitle, 0, MAX_FILE_LEN);

	m_hVerticalScrollbar = nullptr;
	m_hHorizontalScrollbar = nullptr;
}

auto JWWindow::CreateGameWindow(CINT X, CINT Y, CINT Width, CINT Height)->EError
{
	if (CreateWINAPIWindow(L"Game", X, Y, Width, Height, EWindowStyle::OverlappedWindow, m_BGColor, BaseWindowProc)
		== nullptr)
		return EError::WINAPIWINDOW_NOT_CREATED;

	if (InitializeDirectX() == -1)
		return EError::DIRECTX_NOT_CREATED;

	return EError::OK;
}

auto JWWindow::CreateGUIWindow(CINT X, CINT Y, CINT Width, CINT Height, DWORD Color)->EError
{
	// Set DirectX clear color
	m_BGColor = Color;

	if (CreateWINAPIWindow(L"GUI", X, Y, Width, Height, EWindowStyle::OverlappedWindow, Color, BaseWindowProc)
		== nullptr)
		return EError::WINAPIWINDOW_NOT_CREATED;

	if (InitializeDirectX() == -1)
		return EError::DIRECTX_NOT_CREATED;

	return EError::OK;
}

auto JWWindow::CreateParentWindow(CINT X, CINT Y, CINT Width, CINT Height, DWORD Color,
	WNDPROC Proc, LPCWSTR MenuName)->EError
{
	if (CreateWINAPIWindow(L"Editor", X, Y, Width, Height, EWindowStyle::OverlappedWindow, Color, Proc, MenuName)
		== nullptr)
		return EError::WINAPIWINDOW_NOT_CREATED;

	return EError::OK;

}

auto JWWindow::CreateChildWindow(HWND hWndParent, CINT X, CINT Y, CINT Width, CINT Height,
	DWORD Color, WNDPROC Proc)->EError
{
	// Set DirectX clear color
	m_BGColor = Color;

	WSTRING Name = L"EditorChild";
	wchar_t temp[MAX_FILE_LEN] {};
	_itow_s(ms_ChildWindowCount, temp, 10);
	Name += temp;

	ms_ChildWindowCount++;

	if (CreateWINAPIWindow(Name.c_str(), X, Y, Width, Height, EWindowStyle::ChildWindow2, Color, Proc, nullptr, hWndParent)
		== nullptr)
		return EError::WINAPIWINDOW_NOT_CREATED;

	if (InitializeDirectX() == -1)
		return EError::DIRECTX_NOT_CREATED;

	return EError::OK;
}

PRIVATE auto JWWindow::CreateWINAPIWindow(const wchar_t* Name, CINT X, CINT Y, CINT Width, CINT Height,
	EWindowStyle WindowStyle, DWORD BackColor, WNDPROC Proc, LPCWSTR MenuName, HWND hWndParent)->HWND
{
	m_hInstance = GetModuleHandle(nullptr);
	
	// Set window data
	SetWindowData(Width, Height);

	WNDCLASS r_WndClass;
	r_WndClass.cbClsExtra = 0;
	r_WndClass.cbWndExtra = 0;
	r_WndClass.hbrBackground = CreateSolidBrush(RGB(GetColorR(BackColor), GetColorG(BackColor), GetColorB(BackColor)));
	r_WndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	r_WndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	r_WndClass.hInstance = m_hInstance;
	r_WndClass.lpfnWndProc = Proc;
	r_WndClass.lpszClassName = Name;
	r_WndClass.lpszMenuName = MenuName;
	r_WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&r_WndClass);

	m_Rect = { X, Y, X + Width, Y + Height };
	AdjustWindowRect(&m_Rect, (DWORD)WindowStyle, false);

	m_hWnd = CreateWindow(Name, Name, (DWORD)WindowStyle, m_Rect.left, m_Rect.top,
		m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top, hWndParent, (HMENU)nullptr, m_hInstance, nullptr);

	ShowWindow(m_hWnd, SW_SHOW);

	UnregisterClass(Name, m_hInstance);
	
	return m_hWnd;
}

PRIVATE void JWWindow::SetWindowData(int Width, int Height)
{
	m_WindowData.WindowWidth = Width;
	m_WindowData.WindowHeight = Height;
	m_WindowData.WindowHalfWidth = static_cast<float>(Width / 2.0f);
	m_WindowData.WindowHalfHeight = static_cast<float>(Height / 2.0f);
}

PRIVATE auto JWWindow::InitializeDirectX()->int
{
	if (nullptr == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return -1;

	SetDirect3DParameters();

	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_D3DPP, &m_pDevice)))
	{
		return -1;
	}

	return 0;
}

PRIVATE void JWWindow::SetDirect3DParameters()
{
	ZeroMemory(&m_D3DPP, sizeof(m_D3DPP));
	m_D3DPP.Windowed = TRUE;
	m_D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_D3DPP.BackBufferFormat = D3DFMT_UNKNOWN;
	m_D3DPP.hDeviceWindow = m_hWnd;
	
	// Create the back buffer with maximum size(screen resolution).
	// This way you don't need to reset the device when the window is resized
	m_D3DPP.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
	m_D3DPP.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);

	UpdateRenderRect();
}

PRIVATE void JWWindow::UpdateRenderRect()
{
	m_RenderRect = { 0, 0, 0, 0 };
	m_RenderRect.right = m_WindowData.WindowWidth;
	if (m_hVerticalScrollbar)
		m_RenderRect.right -= VERTICAL_SCROLL_BAR_WIDTH;

	m_RenderRect.bottom = m_WindowData.WindowHeight;
	if (m_hHorizontalScrollbar)
		m_RenderRect.bottom -= HORIZONTAL_SCROLL_BAR_HEIGHT;
}

void JWWindow::Destroy()
{
	JW_RELEASE(m_pDevice);
	JW_RELEASE(m_pD3D);
}

void JWWindow::UseVerticalScrollbar()
{
	if (m_hVerticalScrollbar == nullptr)
	{
		m_hVerticalScrollbar = CreateWindow(L"scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_VERT,
			0, 0, 0, 0, m_hWnd, (HMENU)100, m_hInstance, NULL);

		SetVerticalScrollbarRange(0);

		UpdateVerticalScrollbarPosition();

		UpdateRenderRect();
	}
}

void JWWindow::UseHorizontalScrollbar()
{
	if (m_hHorizontalScrollbar == nullptr)
	{
		m_hHorizontalScrollbar = CreateWindow(L"scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ,
			0, 0, 0, 0, m_hWnd, (HMENU)100, m_hInstance, NULL);

		SetHorizontalScrollbarRange(0);

		UpdateHorizontalScrollbarPosition();

		UpdateRenderRect();
	}
}

void JWWindow::SetVerticalScrollbarRange(int Max)
{
	if (m_hVerticalScrollbar)
	{
		SCROLLINFO tInfo;
		tInfo.cbSize = sizeof(tInfo);
		tInfo.fMask = SIF_PAGE | SIF_RANGE;

		tInfo.nPage = 1;

		tInfo.nMin = 0;
		tInfo.nMax = Max;
		SetScrollInfo(m_hVerticalScrollbar, SB_CTL, &tInfo, TRUE);
	}
}

void JWWindow::SetHorizontalScrollbarRange(int Max)
{
	if (m_hHorizontalScrollbar)
	{
		SCROLLINFO tInfo;
		tInfo.cbSize = sizeof(tInfo);
		tInfo.fMask = SIF_PAGE | SIF_RANGE;

		tInfo.nPage = 1;

		tInfo.nMin = 0;
		tInfo.nMax = Max;
		SetScrollInfo(m_hHorizontalScrollbar, SB_CTL, &tInfo, TRUE);
	}
}

auto JWWindow::GetVerticalScrollbarPosition()->int
{
	if (m_hVerticalScrollbar)
	{
		return GetScrollPos(m_hVerticalScrollbar, SB_CTL);
	}
	else
	{
		return 0;
	}
}
auto JWWindow::GetHorizontalScrollbarPosition()->int
{
	if (m_hHorizontalScrollbar)
	{
		return GetScrollPos(m_hHorizontalScrollbar, SB_CTL);
	}
	else
	{
		return 0;
	}
}

PRIVATE void JWWindow::UpdateVerticalScrollbarPosition()
{
	if (m_hVerticalScrollbar)
	{
		int x, y, w, h;
		x = m_WindowData.WindowWidth - VERTICAL_SCROLL_BAR_WIDTH;
		y = 0;
		w = VERTICAL_SCROLL_BAR_WIDTH;
		h = m_WindowData.WindowHeight - HORIZONTAL_SCROLL_BAR_HEIGHT;

		MoveWindow(m_hVerticalScrollbar, x, y, w, h, TRUE);
	}
}

PRIVATE void JWWindow::UpdateHorizontalScrollbarPosition()
{
	if (m_hHorizontalScrollbar)
	{
		int x, y, w, h;
		x = 0;
		y = m_WindowData.WindowHeight - HORIZONTAL_SCROLL_BAR_HEIGHT;
		w = m_WindowData.WindowWidth - VERTICAL_SCROLL_BAR_WIDTH;
		h = HORIZONTAL_SCROLL_BAR_HEIGHT;

		MoveWindow(m_hHorizontalScrollbar, x, y, w, h, TRUE);
	}
}

void JWWindow::SetWindowCaption(WSTRING Caption)
{
	SetWindowTextW(m_hWnd, Caption.c_str());
}

void JWWindow::SetBackgroundColor(D3DCOLOR color)
{
	m_BGColor = color;
}

void JWWindow::Resize(RECT Rect)
{
	SetWindowData(Rect.right, Rect.bottom);
	MoveWindow(m_hWnd, Rect.left, Rect.top, Rect.right, Rect.bottom, false);

	UpdateRenderRect();
}

void JWWindow::BeginRender() const
{
	m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, m_BGColor, 1.0f, 0);
	m_pDevice->BeginScene();
}

void JWWindow::EndRender() const
{
	m_pDevice->EndScene();
	m_pDevice->Present(&m_RenderRect, &m_RenderRect, nullptr, nullptr);
}

auto JWWindow::GetDevice() const->LPDIRECT3DDEVICE9
{
	return m_pDevice;
}

auto JWWindow::GethWnd() const->HWND
{
	return m_hWnd;
}

auto JWWindow::GethInstance() const->HINSTANCE
{
	return m_hInstance;
}

auto JWWindow::GetWindowData()->SWindowData*
{
	return &m_WindowData;
}

auto JWWindow::GetMouseData()->SMouseData*
{
	return &m_MouseData;
}

auto JWWindow::GetRenderRect()->RECT
{
	return m_RenderRect;
}


/** Dialog functions */
void JWWindow::SetDlgBase()
{
	ZeroMemory(&m_OFN, sizeof(m_OFN));
	m_OFN.lStructSize = sizeof(m_OFN);
	m_OFN.hwndOwner = m_hWnd;
	m_OFN.lpstrFile = m_FileName;
	m_OFN.nMaxFile = sizeof(m_FileName);
	m_OFN.lpstrFileTitle = m_FileTitle;
	m_OFN.nMaxFileTitle = sizeof(m_FileTitle);
	m_OFN.nFilterIndex = 1;
	m_OFN.lpstrFileTitle = NULL;
	m_OFN.nMaxFileTitle = 0;
	m_OFN.lpstrInitialDir = NULL;
	m_OFN.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

auto JWWindow::OpenFileDlg(LPCWSTR Filter)->BOOL
{
	SetDlgBase();
	m_OFN.lpstrFilter = Filter;

	return GetOpenFileName(&m_OFN);
}

auto JWWindow::SaveFileDlg(LPCWSTR Filter)->BOOL
{
	SetDlgBase();
	m_OFN.lpstrFilter = Filter;

	return GetSaveFileName(&m_OFN);
}

auto JWWindow::GetDlgFileName()->WSTRING
{
	return m_FileName;
}

auto JWWindow::GetDlgFileTitle()->WSTRING
{
	WSTRING tempString = m_FileName;

	size_t temp = tempString.find_last_of('\\');

	if (temp)
		tempString = tempString.substr(temp + 1);

	return tempString;
}


/** Window procedure for Editor window */
void JWWindow::EditorChildWindowMessageHandler(UINT Message, WPARAM wParam, LPARAM lParam)
{
	int tempScrPos;
	int tempScrMin;
	int tempScrMax;

	switch (Message)
	{
	case WM_MOUSEMOVE:
		m_MouseData.MousePosition.x = GET_X_LPARAM(lParam);
		m_MouseData.MousePosition.y = GET_Y_LPARAM(lParam);
		m_MouseData.bOnMouseMove = true;
		break;

	case WM_LBUTTONDOWN:
		if (!m_MouseData.bMouseLeftButtonPressed)
		{
			m_MouseData.MouseDownPosition.x = GET_X_LPARAM(lParam);
			m_MouseData.MouseDownPosition.y = GET_Y_LPARAM(lParam);

			m_MouseData.bMouseLeftButtonPressed = true;
		}
		break;

	case WM_LBUTTONUP:
		m_MouseData.bMouseLeftButtonPressed = false;
		break;

	case WM_RBUTTONDOWN:
		m_MouseData.bMouseRightButtonPressed = true;
		break;

	case WM_RBUTTONUP:
		m_MouseData.bMouseRightButtonPressed = false;
		break;

	case WM_VSCROLL:
		GetScrollRange((HWND)lParam, SB_CTL, &tempScrMin, &tempScrMax);
		tempScrPos = GetScrollPos((HWND)lParam, SB_CTL);

		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			tempScrPos = max(tempScrMin, tempScrPos - 1);
			break;
		case SB_LINEDOWN:
			tempScrPos = min(tempScrMax, tempScrPos + 1);
			break;
		case SB_PAGEUP:
			tempScrPos = max(tempScrMin, tempScrPos - 10);
			break;
		case SB_PAGEDOWN:
			tempScrPos = min(tempScrMax, tempScrPos + 10);
			break;
		case SB_THUMBTRACK:
			tempScrPos = HIWORD(wParam);
			break;
		}
		SetScrollPos((HWND)lParam, SB_CTL, tempScrPos, TRUE);
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;

	case WM_HSCROLL:
		GetScrollRange((HWND)lParam, SB_CTL, &tempScrMin, &tempScrMax);
		tempScrPos = GetScrollPos((HWND)lParam, SB_CTL);

		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			tempScrPos = max(tempScrMin, tempScrPos - 1);
			break;
		case SB_LINERIGHT:
			tempScrPos = min(tempScrMax, tempScrPos + 1);
			break;
		case SB_PAGELEFT:
			tempScrPos = max(tempScrMin, tempScrPos - 10);
			break;
		case SB_PAGERIGHT:
			tempScrPos = min(tempScrMax, tempScrPos + 10);
			break;
		case SB_THUMBTRACK:
			tempScrPos = HIWORD(wParam);
			break;
		}
		SetScrollPos((HWND)lParam, SB_CTL, tempScrPos, TRUE);
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;

	case WM_SIZE:
		UpdateVerticalScrollbarPosition();
		UpdateHorizontalScrollbarPosition();
		break;
	}
}

auto JWWindow::IsMouseLeftButtonPressed() const->bool
{
	return m_MouseData.bMouseLeftButtonPressed;
}

auto JWWindow::IsMouseRightButtonPressed() const->bool
{
	return m_MouseData.bMouseRightButtonPressed;
}

auto JWWindow::OnMouseMove()->bool
{
	if (m_MouseData.bOnMouseMove)
	{
		m_MouseData.bOnMouseMove = false;
		return true;
	}

	return false;
}