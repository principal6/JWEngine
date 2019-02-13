#pragma once

#include "../CoreType/TDynamicArray.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWLine.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWText.h"
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

namespace JWENGINE
{
	static const wchar_t PROJECT_FOLDER[]{ L"\\GUI" };

	// @warning: JWGUIWindow is created on one and only JWWindow.
	class JWGUIWindow final
	{
	public:
		JWGUIWindow();
		~JWGUIWindow() {};

		auto Create(const SWindowCreationData& WindowCreationData)->EError;
		void Destroy();

		// Create a texture that will be used in this particular JWGUIWindow.
		auto CreateTexture(const WSTRING& Filename, LPDIRECT3DTEXTURE9* pTexture, D3DXIMAGE_INFO* pInfo)->EError;

		// Add a control instance to this JWGUIWindow.
		auto AddControl(EControlType Type, D3DXVECTOR2 Position, D3DXVECTOR2 Size, WSTRING Text = L"")->const THandle;

		// Get the pointer of the control instance that this JWGUIWindow has.
		auto GetControlPtr(const THandle ControlHandle)->JWControl*;

		void Update(MSG& Message, SGUIIMEInputInfo& IMEInfo, HWND QuitWindowHWND);

		// Must call this function before any draw functions.
		void BeginRender();

		// Draw all the controls that this JWGUIWindow has.
		void DrawAllControls();
		
		// Must call this function after all the draw functions.
		void EndRender();

		// Return true if this JWGUIWindow is destroyed.
		// @warning: when this function returns true, you must call Destroy() method from the outside.
		auto IsDestroyed()->bool;

	private:
		SGUIWindowSharedData m_SharedData;
		SMouseData m_MouseData;
		MSG m_MSG;

		JWControl* m_pMenuBar;
		bool m_bHasMenuBar;

		TDynamicArray<JWControl*> m_Controls;
		JWControl* m_pControlWithFocus;

		bool m_bDestroyed;
	};
};