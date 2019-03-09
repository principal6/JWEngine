#pragma once

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
#include "Controls/JWFrameConnector.h"

namespace JWENGINE
{
	static const wchar_t PROJECT_FOLDER[]{ L"\\GUI" };

	// @warning: JWGUIWindow is created on one and only JWWindow.
	class JWGUIWindow final
	{
	friend class JWGUI;

	public:
		JWGUIWindow() {};
		~JWGUIWindow();

		// Create a texture that will be used in this particular JWGUIWindow.
		void CreateTexture(const WSTRING& Filename, LPDIRECT3DTEXTURE9* ppTexture, D3DXIMAGE_INFO* pInfo) noexcept;

		// Add a control instance to this JWGUIWindow.
		auto AddControl(EControlType Type, const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const WSTRING& ControlName = L"") noexcept->JWControl*;

		// Get the reference of the control instance that this JWGUIWindow has.
		auto GetControl(const WSTRING& ControlName)->JWControl&;

		auto GetSharedDataPtr() const noexcept->const SGUIWindowSharedData*;

		// Return true if this JWGUIWindow is destroyed.
		// @warning: when this function returns true, you must call Destroy() method from the outside.
		auto IsDestroyed() const noexcept->bool;

		auto HasMenuBar() const noexcept->bool;

		auto GetMenuBarHeight() const noexcept->float;

		// Kill all the focus.
		void KillAllFocus() noexcept;

	protected:
		// This is called in JWGUI (friend class).
		void Create(const SWindowCreationData& WindowCreationData) noexcept;

		// This is called in JWGUI (friend class).
		void Update(const MSG& Message, const SGUIIMEInputInfo& IMEInfo, VECTOR<HWND>& hWndQuitStack, const HWND ActiveWindowHWND) noexcept;

		// This is called in JWGUI (friend class).
		// Must call this function before any draw functions.
		void BeginRender() const noexcept;

		// This is called in JWGUI (friend class).
		// Draw all the controls that this JWGUIWindow has.
		void DrawAllControls() noexcept;

		// This is called in JWGUI (friend class).
		// Must call this function after all the draw functions.
		void EndRender() noexcept;

	private:
		void SetFocusOnControl(JWControl* pFocusedControl) noexcept;

	private:
		SGUIWindowSharedData m_SharedData;
		MSG m_MSG{};

		JWControl* m_pMenuBar{ nullptr };
		bool m_bHasMenuBar{ false };

		JWControl* m_pControlWithFocus{ nullptr };

		std::map<WSTRING, size_t> m_ControlsMap;
		VECTOR<UNIQUE_PTR<JWControl>> m_pControls;
		
		bool m_bDestroyed{ false };
	};
};