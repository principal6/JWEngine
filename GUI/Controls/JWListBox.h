#pragma once

#include "JWControl.h"
#include "JWImageBox.h"
#include "JWLabel.h"
#include "JWScrollBar.h"

namespace JWENGINE
{
	struct SListBoxItemInfo
	{
		D3DXVECTOR2 ItemPosition{};
		D3DXVECTOR2 ItemSize{};
		D3DXVECTOR2 ImageItemSize{};
	};

	class JWListBox final : public JWControl
	{
	friend class JWMenuBar;

	public:
		JWListBox() {};
		~JWListBox() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		auto SetMinimumItemHeight(float Value) noexcept->JWControl* override;
		auto SetImageItemTextureAtlas(const LPDIRECT3DTEXTURE9 pTexture, const D3DXIMAGE_INFO* pInfo) noexcept->JWControl* override;

		auto AddListBoxItem(const WSTRING& Text, const D3DXVECTOR2& OffsetInAtlas = D3DXVECTOR2(0, 0),
			const D3DXVECTOR2& SizeInAtlas = D3DXVECTOR2(0, 0))->JWControl* override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;
		auto SetBackgroundColor(DWORD Color) noexcept->JWControl* override;

		auto GetListBoxItemCount() const noexcept->size_t override;

		// Returns the sum of the height of all the items in the ListBox.
		auto GetListBoxItemHeight() const noexcept->float override;

		auto GetSelectedItemIndex() const noexcept->TIndex override;

		auto ShouldUseAutomaticScrollBar(bool Value) noexcept->JWControl* override;
		auto ShouldUseToggleSelection(bool Value) noexcept->JWControl* override;
		auto ShouldUseImageItem(bool Value) noexcept->JWControl* override;

	protected:
		// Must be overridden.
		void UpdateViewport() noexcept;

		// Must be overridden.
		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept override;

	private:
		void UpdateAutomaticScrollBar() noexcept;

		void SetToggleSelectionColor(JWImageBox* pItemBackground) noexcept;
		void SetNonToggleSelectionColor(JWImageBox* pItemBackground) noexcept;

	private:
		static const DWORD DEFAULT_COLOR_BACKGROUND_LISTBOX{ DEFAULT_COLOR_NORMAL };
		static const float MINIMUM_ITEM_HEIGHT;
		static const float DEFAULT_ITEM_HEIGHT;
		static const float DEFAULT_ITEM_PADDING_X;
		static const float DEFAULT_ITEM_PADDING_Y;

		LPDIRECT3DTEXTURE9 m_pTextureForImageItem{ nullptr };
		const D3DXIMAGE_INFO* m_pTextureForImageItemInfo{ nullptr };

		VECTOR<UNIQUE_PTR<JWImageBox>> m_pItemBackground;
		VECTOR<UNIQUE_PTR<JWImageBox>> m_pImageItems;
		VECTOR<UNIQUE_PTR<JWLabel>> m_pTextItems;
		VECTOR<SListBoxItemInfo> m_ItemInfo;

		UNIQUE_PTR<JWImageBox> m_pBackground;
		UNIQUE_PTR<JWScrollBar> m_pScrollBar;

		// Property settings
		bool m_bHasScrollBar{ false };
		bool m_bShouldUseAutomaticScrollBar{ true };
		bool m_bShouleUseToggleSelection{ true };
		bool m_bUseImageItems{ false };

		TIndex m_SelectedItemIndex{ TIndex_Invalid };
		float m_ItemOffsetY{ 0 };
		float m_MinimumItemHeight{ DEFAULT_ITEM_HEIGHT };
	};
};