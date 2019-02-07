#pragma once

#include "../CoreType/TDynamicArray.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWLine.h"
#include "../CoreBase/JWWindow.h"
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

		auto Create(JWWindow* pWindow)->EError;
		void Destroy();

		void Run();

		auto AddControl(EControlType Type, D3DXVECTOR2 Position, D3DXVECTOR2 Size, WSTRING Text = L"")->const THandle;
		auto GetControlPtr(const THandle ControlHandle)->JWControl*;
		
		void SetMainLoopFunction(PF_MAINLOOP pfMainLoop);

		void DrawAllControls();

		auto CreateTexture(const WSTRING& Filename, LPDIRECT3DTEXTURE9* pTexture, D3DXIMAGE_INFO* pInfo)->EError;

	private:
		void MainLoop();
		void HandleMessage();
		
	private:
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