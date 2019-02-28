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
#include "Controls/JWFrame.h"

namespace JWENGINE
{
	static const wchar_t PROJECT_FOLDER[]{ L"\\GUI" };

	// @warning: JWGUIWindow is created on one and only JWWindow.
	class JWGUIWindow final
	{
	friend class JWGUI;

	public:
		JWGUIWindow();
		~JWGUIWindow() {};

		// Create a texture that will be used in this particular JWGUIWindow.
		auto CreateTexture(const WSTRING& Filename, LPDIRECT3DTEXTURE9* ppTexture, D3DXIMAGE_INFO* pInfo)->EError;

		// Add a control instance to this JWGUIWindow.
		auto AddControl(const EControlType Type, const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const WSTRING& ControlName = L"")->JWControl*;

		// Get the pointer of the control instance that this JWGUIWindow has.
		auto GetControlPtr(const WSTRING& ControlName)->JWControl*;

		auto GetSharedDataPtr() const->const SGUIWindowSharedData*;

		// Return true if this JWGUIWindow is destroyed.
		// @warning: when this function returns true, you must call Destroy() method from the outside.
		auto IsDestroyed() const->const bool;

		auto HasMenuBar() const->const bool;

		auto GetMenuBarHeight() const->const float;

		// Kill all the focus.
		void KillAllFocus();

	protected:
		// This is called in JWGUI (friend class).
		auto Create(const SWindowCreationData& WindowCreationData)->EError;

		// This is called in JWGUI (friend class).
		void Destroy();

		// This is called in JWGUI (friend class).
		void Update(const MSG& Message, const SGUIIMEInputInfo& IMEInfo, VECTOR<HWND>& hWndQuitStack, const HWND ActiveWindowHWND);

		// This is called in JWGUI (friend class).
		// Must call this function before any draw functions.
		void BeginRender() const;

		// This is called in JWGUI (friend class).
		// Draw all the controls that this JWGUIWindow has.
		void DrawAllControls();

		// This is called in JWGUI (friend class).
		// Must call this function after all the draw functions.
		void EndRender();

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