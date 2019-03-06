#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWTextButton;
	class JWImageBox;
	class JWListBox;
	// ***

	using MenuItem = JWTextButton;
	using MenuSubItemBox = JWListBox;

	class JWMenuBar final : public JWControl
	{
	public:
		JWMenuBar() {};
		~JWMenuBar() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		auto AddMenuBarItem(const WSTRING& Text)->THandleItem override;
		auto AddMenuBarSubItem(THandleItem hItem, const WSTRING& Text) noexcept->THandleItem override;

		void Draw() noexcept override;

		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;

		auto OnSubItemClick() noexcept->THandleItem override;

		void KillFocus() noexcept override;

	protected:
		// Must be overridden.
		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept override;

	private:
		auto GetTHandleItemOfMenuBarItem(TIndex ItemIndex) noexcept->THandleItem;
		auto GetTIndexOfMenuBarItem(THandleItem hItem) noexcept->TIndex;

		void SelectMenuBarItem(TIndex ItemIndex) noexcept;
		void UnselectMenuBarItem() noexcept;

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND_MENUBAR{ 255 };
		static const DWORD DEFAULT_COLOR_BACKGROUND_MENUBAR{ DEFAULT_COLOR_NORMAL };
		static const int DEFAULT_MENUBAR_HEIGHT{ 24 };
		static const int DEFAULT_MENUBAR_ITEM_PADDING{ 10 };
		static const D3DXVECTOR2& BLANK_SUBITEMBOX_SIZE;
		static const THandleItem MENU_ITEM_THANDLE_BASE{ 10000 };

		// This means that the total count of sub items of each item should be less than 100.
		static const THandleItem MENU_ITEM_THANDLE_STRIDE{ 100 };

		// MenuBar's non-button region.
		UNIQUE_PTR<JWImageBox> m_pNonButtonRegion{};

		VECTOR<UNIQUE_PTR<MenuItem>> m_pItems;
		VECTOR<UNIQUE_PTR<MenuSubItemBox>> m_pSubItemBoxes;

		MenuItem* m_pSelectedItem{ nullptr };
		MenuSubItemBox* m_pSelectedSubItemBox{ nullptr };
		TIndex m_SelectedItemIndex{ TIndex_Invalid };
		THandleItem m_hSelectedSubItem{ THandleItem_Null };
	};
};