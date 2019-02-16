#pragma once

#include "JWControl.h"
#include "../CoreType/TLinkedList.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWTextButton;
	class JWImage;
	class JWListBox;
	// ***

	using MenuItem = JWTextButton;
	using MenuSubItemBox = JWListBox;

	class JWMenuBar final : public JWControl
	{
	public:
		JWMenuBar();
		~JWMenuBar() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		auto AddMenuBarItem(WSTRING Text)->THandleItem override;
		auto AddMenuBarSubItem(THandleItem hItem, WSTRING Text)->THandleItem override;

		void UpdateControlState(JWControl** ppControlWithFocus) override;

		void Draw() override;

		void SetSize(D3DXVECTOR2 Size) override;

		auto OnSubItemClick() const->THandleItem override;

	private:
		auto GetTHandleItemOfMenuBarItem(TIndex ItemIndex)->THandleItem;
		auto GetTIndexOfMenuBarItem(THandleItem hItem)->TIndex;

		void SelectItem(TIndex ItemIndex);
		void UnselectItem();

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND_MENUBAR = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND_MENUBAR = DEFAULT_COLOR_NORMAL;
		static const int DEFAULT_MENUBAR_HEIGHT = 24;
		static const int DEFAULT_MENUBAR_ITEM_PADDING = 10;
		static const D3DXVECTOR2 BLANK_SUBITEMBOX_SIZE;
		static const THandle MENU_ITEM_THANDLE_BASE = 10000;

		// This means that the total count of sub items of each item should be less than 100.
		static const THandle MENU_ITEM_THANDLE_STRIDE = 100;

		// MenuBar's background.
		JWImage* m_pBackground;

		TLinkedList<MenuItem*> m_pItems;
		TLinkedList<MenuSubItemBox*> m_pSubItemBoxes;

		MenuItem* m_pSelectedItem;
		MenuSubItemBox* m_pSelectedSubItemBox;
		TIndex m_SelectedItemIndex;
		THandleItem m_hSelectedSubItem;

		bool m_bMouseReleasedForTheFisrtTime;
		
	};
};