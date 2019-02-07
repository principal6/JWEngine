#pragma once

#include "JWControl.h"
#include "../CoreType/TLinkedList.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWLabel;
	class JWImage;
	class JWListBox;
	// ***

	using MenuItem = JWLabel;
	using MenuSubItemBox = JWListBox;

	class JWMenuBar final : public JWControl
	{
	public:
		JWMenuBar();
		~JWMenuBar() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		auto AddMenuBarItem(WSTRING Text)->THandleItem override;

		void UpdateControlState(const SMouseData& MouseData) override;

		void Draw() override;

		void SetSize(D3DXVECTOR2 Size) override;

		auto IsMouseOver(const SMouseData& MouseData)->bool override;
	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND_MENUBAR = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND_MENUBAR = DEFAULT_COLOR_NORMAL;
		static const int DEFAULT_MENUBAR_HEIGHT = 24;
		static const D3DXVECTOR2 BLANK_SUBITEMBOX_SIZE;
		static const THandle MENU_ITEM_THANDLE_BASE = 10000;
		static const THandle MENU_SUBITEM_THANDLE_BASE = 1000000;

		JWImage* m_pBackground;

		TLinkedList<MenuItem*> m_pItems;
		MenuItem* m_pFocusedItem;
		MenuSubItemBox* m_pSubItemBox;

		bool m_bMouseReleasedForTheFisrtTime;
		
	};
};