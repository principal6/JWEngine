#pragma once

#include "../CoreType/TDynamicArray.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWLine.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWFont.h"
#include "JWControl.h"
#include "JWTextButton.h"
#include "JWImageButton.h"
#include "JWLabel.h"
#include "JWEdit.h"
#include "JWCheckBox.h"
#include "JWRadioBox.h"
#include "JWScrollBar.h"
#include "JWListBox.h"
#include "JWMenuBar.h"
#include "JWImageBox.h"

// @WARNING: ONE GUI PER ONE WINDOW!!

namespace JWENGINE
{
	static const wchar_t PROJECT_FOLDER[]{ L"\\GUI" };

	using PF_MAINLOOP = void(*)();

	class JWGUI final
	{
	public:
		JWGUI();
		~JWGUI() {};

		auto Create(CINT X, CINT Y, CINT Width, CINT Height, DWORD Color)->EError;

		auto CreateOnWindow(JWWindow* pWindow)->EError;
		void Destroy();

		void Run();

		void ShowDialogue();

		auto AddControl(EControlType Type, D3DXVECTOR2 Position, D3DXVECTOR2 Size, WSTRING Text = L"")->const THandle;
		auto GetControlPtr(const THandle ControlHandle)->JWControl*;
		
		void SetMainLoopFunction(PF_MAINLOOP pfMainLoop);

		void DrawAllControls();

		auto CreateTexture(const WSTRING& Filename, LPDIRECT3DTEXTURE9* pTexture, D3DXIMAGE_INFO* pInfo)->EError;

	private:
		friend LRESULT CALLBACK GUIWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

		void MainLoop();
		void HandleMessage();
		
	private:
		static TCHAR ms_IMEWritingChar[MAX_FILE_LEN];
		static TCHAR ms_IMECompletedChar[MAX_FILE_LEN];
		static bool ms_bIMEWriting;
		static bool ms_bIMECompleted;
		bool m_bRunning;

		SGUISharedData m_SharedData;

		TDynamicArray<JWControl*> m_Controls;
		MSG m_MSG;

		SMouseData m_MouseData;

		JWControl* m_pControlWithFocus;
		JWControl* m_pMenuBar;

		PF_MAINLOOP m_pfMainLoop;

		bool m_bHasMenuBar;
	};
};