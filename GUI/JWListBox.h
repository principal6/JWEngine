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
	friend class JWMenuBar;

	public:
		JWListBox();
		~JWListBox() {};

		auto Create(const D3DXVECTOR2 Position, const D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		auto SetMinimumItemHeight(const float Value)->JWControl* override;
		auto SetImageItemTextureAtlas(const LPDIRECT3DTEXTURE9 pTexture, const D3DXIMAGE_INFO* pInfo)->JWControl* override;

		auto AddListBoxItem(const WSTRING Text, const D3DXVECTOR2 OffsetInAtlas = D3DXVECTOR2(0, 0),
			const D3DXVECTOR2 SizeInAtlas = D3DXVECTOR2(0, 0))->JWControl* override;

		void Draw() override;

		auto SetPosition(const D3DXVECTOR2 Position)->JWControl* override;
		auto SetSize(const D3DXVECTOR2 Size)->JWControl* override;
		auto SetBackgroundColor(DWORD Color)->JWControl* override;

		auto GetListBoxItemCount() const->const size_t override;

		// Returns the sum of the height of all the items in the ListBox.
		auto GetListBoxItemHeight() const->const float override;

		auto GetSelectedItemIndex() const->const TIndex override;

		auto ShouldUseAutomaticScrollBar(const bool Value)->JWControl* override;
		auto ShouldUseToggleSelection(const bool Value)->JWControl* override;
		auto ShouldUseImageItem(const bool Value)->JWControl* override;

	protected:
		// Must be overridden.
		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) override;

	private:
		void UpdateAutomaticScrollBar();

		void SetToggleSelectionColor(JWImageBox* pItemBackground);
		void SetNonToggleSelectionColor(JWImageBox* pItemBackground);

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND_LISTBOX = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND_LISTBOX = DEFAULT_COLOR_NORMAL;
		static const float MINIMUM_ITEM_HEIGHT;
		static const float DEFAULT_ITEM_HEIGHT;
		static const float DEFAULT_ITEM_PADDING_X;
		static const float DEFAULT_ITEM_PADDING_Y;

		LPDIRECT3DTEXTURE9 m_pTextureForImageItem;
		const D3DXIMAGE_INFO* m_pTextureForImageItemInfo;

		TLinkedList<JWImageBox*> m_pItemBackground;
		TLinkedList<JWLabel*> m_pTextItems;
		TLinkedList<JWImageBox*> m_pImageItems;
		TLinkedList<SListBoxItemInfo> m_ItemInfo;

		JWImage* m_pBackground;
		JWScrollBar* m_pScrollBar;

		// Property settings
		bool m_bHasScrollBar;
		bool m_bShouldUseAutomaticScrollBar;
		bool m_bShouleUseToggleSelection;
		bool m_bUseImageItems;

		TIndex m_SelectedItemIndex;
		float m_ItemOffsetY;
		float m_MinimumItemHeight;
	};
};