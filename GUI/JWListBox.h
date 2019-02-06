#pragma once

#include "JWControl.h"
#include "../CoreType/TLinkedList.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWLabel;
	class JWImage;
	class JWScrollBar;
	// ***

	class JWListBox final : public JWControl
	{
	public:
		JWListBox();
		~JWListBox() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void AddTextItem(WSTRING Text) override;

		void UpdateControlState(const SMouseData& MouseData) override;
		void UpdateScrollBarData();

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND_LISTBOX = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND_LISTBOX = DEFAULT_COLOR_NORMAL;
		static const float DEFUALT_ITEM_HEIGHT;

		TLinkedList<JWLabel*> m_pItems;
		JWImage* m_pBackground;
		JWScrollBar* m_pScrollBar;

		bool m_bShouldHaveScrollBar;

		size_t m_SelectedItemIndex;
		float m_ItemOffsetY;
	};
};