#pragma once

#include "../CoreType/TDynamicArray.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWLine.h"
#include "../CoreBase/JWWindow.h"
#include "../CoreBase/JWText.h"
#include "Controls/JWControl.h"
#include "Controls/JWTextButton.h"
#include "Controls/JWImageButton.h"
#include "Controls/JWLabel.h"
#include "Controls/JWEdit.h"
#include "Controls/JWCheckBox.h"
#include "Controls/JWRadioBox.h"
#include "Controls/JWScrollBar.h"
#include "Controls/JWListBox.h"
#include "Controls/JWMenuBar.h"
#include "Controls/JWImageBox.h"

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
		auto CreateTexture(const WSTRING& Filename, LPDIRECT3DTEXTURE9* ppTexture, D3DXIMAGE_INFO* pInfo)->EError;

		// Add a control instance to this JWGUIWindow.
		auto AddControl(const EControlType Type, const D3DXVECTOR2 Position, const D3DXVECTOR2 Size,
			const WSTRING ControlName = L"")->JWControl*;

		// Get the pointer of the control instance that this JWGUIWindow has.
		auto GetControlPtr(const WSTRING ControlName)->JWControl*;

		void Update(const MSG& Message, const SGUIIMEInputInfo& IMEInfo, const HWND QuitWindowHWND, const HWND ActiveWindowHWND);

		// Must call this function before any draw functions.
		void BeginRender() const;

		// Draw all the controls that this JWGUIWindow has.
		void DrawAllControls();

		// Must call this function after all the draw functions.
		void EndRender();

		// Return true if this JWGUIWindow is destroyed.
		// @warning: when this function returns true, you must call Destroy() method from the outside.
		auto IsDestroyed() const->const bool;

		// Kill all the focus.
		void KillAllFocus();

	private:
		void SetFocusOnControl(JWControl* pFocusedControl);

	private:
		SGUIWindowSharedData m_SharedData;
		MSG m_MSG;

		JWControl* m_pMenuBar;
		bool m_bHasMenuBar;

		std::map<WSTRING, size_t> m_ControlsMap;
		TDynamicArray<JWControl*> m_pControls;
		JWControl* m_pControlWithFocus;

		bool m_bDestroyed;
	};
};