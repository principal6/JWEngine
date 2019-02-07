#pragma once

#include "JWControl.h"
#include "../CoreType/TLinkedList.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWLabel;
	class JWImage;
	class JWImageBox;
	class JWScrollBar;
	// ***

	struct SListBoxItemInfo
	{
		D3DXVECTOR2 ItemPosition;
		D3DXVECTOR2 ItemSize;
		D3DXVECTOR2 ImageItemSize;

		SListBoxItemInfo() : ItemPosition(D3DXVECTOR2(0, 0)), ItemSize(D3DXVECTOR2(0, 0)), ImageItemSize(D3DXVECTOR2(0, 0)) {};
	};

	class JWListBox final : public JWControl
	{
	public:
		JWListBox();
		~JWListBox() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void SetMinimumItemHeight(float Value) override;
		void UseImageItem(LPDIRECT3DTEXTURE9 pTexture, D3DXIMAGE_INFO* pInfo) override;

		void AddListBoxItem(WSTRING Text, D3DXVECTOR2 OffsetInAtlas, D3DXVECTOR2 SizeInAtlas) override;

		void UpdateControlState(const SMouseData& MouseData) override;
		void UpdateScrollBarData();

		void Draw() override;

		void SetBackgroundColor(DWORD Color) override;
		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND_LISTBOX = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND_LISTBOX = DEFAULT_COLOR_NORMAL;
		static const float MINIMUM_ITEM_HEIGHT;
		static const float DEFAULT_ITEM_HEIGHT;
		static const float DEFAULT_ITEM_PADDING_X;
		static const float DEFAULT_ITEM_PADDING_Y;

		LPDIRECT3DTEXTURE9 m_pTextureForImageItem;
		D3DXIMAGE_INFO* m_pTextureForImageItemInfo;

		TLinkedList<JWImageBox*> m_pItemBackground;
		TLinkedList<JWLabel*> m_pTextItems;
		TLinkedList<JWImageBox*> m_pImageItems;
		TLinkedList<SListBoxItemInfo> m_ItemInfo;

		JWImage* m_pBackground;
		JWScrollBar* m_pScrollBar;

		bool m_bShouldHaveScrollBar;
		bool m_bUseImageItems;

		TIndex m_SelectedItemIndex;
		float m_ItemOffsetY;
		float m_MinimumItemHeight;
	};
};