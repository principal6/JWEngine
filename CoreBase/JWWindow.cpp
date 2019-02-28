#include "JWWindow.h"

using namespace JWENGINE;

// Base window procedure for Game window
LRESULT CALLBACK JWENGINE::BaseWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
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
}

void JWWindow::CreateGameWindow(const SWindowCreationData& WindowCreationData)
{
	// Set DirectX clear color
	m_BGColor = WindowCreationData.color_background;

	if (CreateWINAPIWindow(L"Game", WindowCreationData.caption.c_str(),
		WindowCreationData.x, WindowCreationData.y,
		WindowCreationData.width, WindowCreationData.height,
		EWindowStyle::OverlappedWindow,
		m_BGColor,
		BaseWindowProc) == nullptr)
	{
		throw EError::WINAPIWINDOW_NOT_CREATED;
	}

	InitializeDirectX();
}

void JWWindow::CreateGUIWindow(const SWindowCreationData& WindowCreationData)
{
	// Set DirectX clear color
	m_BGColor = WindowCreationData.color_background;

	if (CreateWINAPIWindow(L"GUI", WindowCreationData.caption.c_str(),
		WindowCreationData.x, WindowCreationData.y,
		WindowCreationData.width, WindowCreationData.height,
		WindowCreationData.window_style,
		WindowCreationData.color_background,
		WindowCreationData.proc, nullptr, WindowCreationData.parent_hwnd) == nullptr)
	{
		throw EError::WINAPIWINDOW_NOT_CREATED;
	}

	InitializeDirectX();
}

PRIVATE auto JWWindow::CreateWINAPIWindow(const wchar_t* Name, const wchar_t* Caption, CINT X, CINT Y, CINT Width, CINT Height,
	EWindowStyle WindowStyle, DWORD BackColor, WNDPROC Proc, LPCWSTR MenuName, HWND hWndParent)->HWND
{
	m_hInstance = GetModuleHandle(nullptr);
	
	// Set window data
	SetWindowData(Width, Height);

	WNDCLASS r_WndClass;
	r_WndClass.cbClsExtra = 0;
	r_WndClass.cbWndExtra = 0;
	r_WndClass.hbrBackground = 0; //CreateSolidBrush(RGB(GetColorR(BackColor), GetColorG(BackColor), GetColorB(BackColor)));
	r_WndClass.hCursor = LoadCursor(nullptr, IDC_ARROW); // could be nullptr
	r_WndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	r_WndClass.hInstance = m_hInstance;
	r_WndClass.lpfnWndProc = Proc;
	r_WndClass.lpszClassName = Name;
	r_WndClass.lpszMenuName = MenuName;
	r_WndClass.style = CS_HREDRAW | CS_VREDRAW; //CS_DBLCLKS
	RegisterClass(&r_WndClass);

	m_Rect = { X, Y, X + Width, Y + Height };
	AdjustWindowRect(&m_Rect, (DWORD)WindowStyle, false);

	if ((m_hWnd = CreateWindow(Name, Caption, (DWORD)WindowStyle, m_Rect.left, m_Rect.top,
		m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top, hWndParent, (HMENU)nullptr, m_hInstance, nullptr)) == nullptr)
	{
		throw EError::NULLPTR_HWND;
	}

	ShowWindow(m_hWnd, SW_SHOW);

	UnregisterClass(Name, m_hInstance);
	
	return m_hWnd;
}

PRIVATE void JWWindow::SetWindowData(CINT Width, CINT Height) noexcept
{
	m_WindowData.WindowWidth = Width;
	m_WindowData.WindowHeight = Height;
	m_WindowData.WindowHalfWidth = static_cast<float>(Width / 2.0f);
	m_WindowData.WindowHalfHeight = static_cast<float>(Height / 2.0f);
}

PRIVATE void JWWindow::InitializeDirectX()
{
	if (nullptr == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		throw EError::DIRECT3D_NOT_CREATED;
	}

	SetDirect3DParameters();

	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_D3DPP, &m_pDevice)))
	{
		throw EError::DEVICE_NOT_CREATED;
	}
}

PRIVATE void JWWindow::SetDirect3DParameters() noexcept
{
	ZeroMemory(&m_D3DPP, sizeof(m_D3DPP));
	m_D3DPP.Windowed = TRUE;
	m_D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_D3DPP.BackBufferFormat = D3DFMT_UNKNOWN;
	m_D3DPP.hDeviceWindow = m_hWnd;
	
	// Create the back buffer with maximum size(screen resolution).
	// This way you don't need to reset the device when the window is resized
	m_WindowData.ScreenSize.x = GetSystemMetrics(SM_CXSCREEN);
	m_WindowData.ScreenSize.y = GetSystemMetrics(SM_CYSCREEN);

	m_D3DPP.BackBufferWidth = m_WindowData.ScreenSize.x;
	m_D3DPP.BackBufferHeight = m_WindowData.ScreenSize.y;

	UpdateRenderRect();
}

PRIVATE void JWWindow::UpdateRenderRect() noexcept
{
	m_RenderRect = { 0, 0, 0, 0 };

	//m_RenderRect.right = m_WindowData.WindowWidth;
	m_RenderRect.right = m_WindowData.ScreenSize.x;

	//m_RenderRect.bottom = m_WindowData.WindowHeight;
	m_RenderRect.bottom = m_WindowData.ScreenSize.y;
}

void JWWindow::Destroy() noexcept
{
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}

	JW_RELEASE(m_pDevice);
	JW_RELEASE(m_pD3D);
}

void JWWindow::SetWindowCaption(const WSTRING& Caption) noexcept
{
	SetWindowTextW(m_hWnd, Caption.c_str());
}

void JWWindow::SetBackgroundColor(const D3DCOLOR color) noexcept
{
	m_BGColor = color;
}

void JWWindow::Resize(const RECT Rect) noexcept
{
	SetWindowData(Rect.right, Rect.bottom);
	//MoveWindow(m_hWnd, Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, false);

	UpdateRenderRect();
}

void JWWindow::BeginRender() const noexcept
{
	m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, m_BGColor, 1.0f, 0);
	m_pDevice->BeginScene();
}

void JWWindow::EndRender() noexcept
{
	m_pDevice->EndScene();

	if (FAILED(m_pDevice->Present(&m_RenderRect, &m_RenderRect, nullptr, nullptr)))
	{
		m_pDevice->Reset(&m_D3DPP);
	}

}

auto JWWindow::GetDevice() const noexcept->const LPDIRECT3DDEVICE9
{
	return m_pDevice;
}

auto JWWindow::GethWnd() const noexcept->const HWND
{
	return m_hWnd;
}

auto JWWindow::GethInstance() const noexcept->const HINSTANCE
{
	return m_hInstance;
}

auto JWWindow::GetWindowData() const noexcept->const SWindowData*
{
	return &m_WindowData;
}

auto JWWindow::GetRenderRect() const noexcept->const RECT
{
	return m_RenderRect;
}

void JWWindow::UpdateWindowInputState(const MSG& Message) noexcept
{
	// Window message input
	// Initialize mouse wheel stride.
	m_WindowInputState.MouseWheeled = 0;

	m_WindowInputState.MouseLeftFirstPressed = false;

	if (Message.hwnd == m_hWnd)
	{
		m_WindowInputState.MouseLeftReleased = false;
		m_WindowInputState.MouseRightReleased = false;

		switch (Message.message)
		{
		case WM_MOUSEMOVE:
			m_WindowInputState.MousePosition.x = GET_X_LPARAM(Message.lParam);
			m_WindowInputState.MousePosition.y = GET_Y_LPARAM(Message.lParam);
			break;
		case WM_LBUTTONDOWN:
			if (!m_WindowInputState.MouseLeftPressed)
			{
				m_WindowInputState.MouseLeftFirstPressed = true;
			}

			m_WindowInputState.MouseLeftPressed = true;
			
			m_WindowInputState.MouseDownPosition.x = GET_X_LPARAM(Message.lParam);
			m_WindowInputState.MouseDownPosition.y = GET_Y_LPARAM(Message.lParam);
			break;
		case WM_LBUTTONUP:
			m_WindowInputState.MouseLeftPressed = false;
			m_WindowInputState.MouseLeftReleased = true;
			break;
		case WM_RBUTTONDOWN:
			m_WindowInputState.MouseRightPressed = true;
			break;
		case WM_RBUTTONUP:
			m_WindowInputState.MouseRightPressed = false;
			m_WindowInputState.MouseRightReleased = true;
			break;
		case WM_MOUSEWHEEL:
			m_WindowInputState.MouseWheeled = static_cast<__int16>(HIWORD(Message.wParam));
			break;
		default:
			break;
		}

		m_WindowInputState.MouseMovedPosition.x = m_WindowInputState.MousePosition.x - m_WindowInputState.MouseDownPosition.x;
		m_WindowInputState.MouseMovedPosition.y = m_WindowInputState.MousePosition.y - m_WindowInputState.MouseDownPosition.y;
	}

	// Keyboard
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		m_WindowInputState.ControlPressed = true;
	}
	else
	{
		m_WindowInputState.ControlPressed = false;
	}

	if (GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		m_WindowInputState.AltPressed = true;
	}
	else
	{
		m_WindowInputState.AltPressed = false;
	}

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		m_WindowInputState.ShiftPressed = true;
	}
	else
	{
		m_WindowInputState.ShiftPressed = false;
	}
}

auto JWWindow::GetWindowInputStatePtr() const noexcept->const SWindowInputState*
{
	return &m_WindowInputState;
}

/** Dialog functions */
void JWWindow::SetDlgBase() noexcept
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

auto JWWindow::OpenFileDlg(LPCWSTR Filter) noexcept->BOOL
{
	SetDlgBase();

	m_OFN.lpstrFilter = Filter;

	return GetOpenFileName(&m_OFN);
}

auto JWWindow::SaveFileDlg(LPCWSTR Filter) noexcept->BOOL
{
	SetDlgBase();

	m_OFN.lpstrFilter = Filter;

	return GetSaveFileName(&m_OFN);
}

auto JWWindow::GetDlgFileName() const noexcept->const WSTRING
{
	return m_FileName;
}

auto JWWindow::GetDlgFileTitle() const noexcept->const WSTRING
{
	WSTRING tempString = m_FileName;

	size_t temp = tempString.find_last_of('\\');

	if (temp)
	{
		tempString = tempString.substr(temp + 1);
	}

	return tempString;
}