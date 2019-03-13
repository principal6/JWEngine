#include "JWMenuBar.h"
#include "../../CoreBase/JWText.h"
#include "../../CoreBase/JWWindow.h"
#include "JWImageBox.h"
#include "JWTextButton.h"
#include "JWListBox.h"

using namespace JWENGINE;

// Static const
const D3DXVECTOR2 JWMenuBar::BLANK_SUBITEMBOX_SIZE{ 100.0f, 60.0f };

void JWMenuBar::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type
	m_ControlType = EControlType::MenuBar;

	// A menubar must not have border.
	m_bShouldDrawBorder = false;

	// A menubar must not be offset by menubar.
	m_bShouldBeOffsetByMenuBar = false;

	// MenuBar's position must be fixed!
	m_Position = { 0, 0 };

	// MenuBar's size must be fixed!
	m_Size.x = static_cast<float>(m_pSharedData->pWindow->GetWindowData()->ScreenSize.x);
	m_Size.y = static_cast<float>(DEFAULT_MENUBAR_HEIGHT);

	// Create ImageBox for the non-button region.
	m_pNonButtonRegion = MAKE_UNIQUE(JWImageBox)();
	m_pNonButtonRegion->Create(Position, Size, SharedData);
	m_pNonButtonRegion->ShouldBeOffsetByMenuBar(false);
	m_pNonButtonRegion->SetBackgroundColor(DEFAULT_COLOR_BACKGROUND_MENUBAR);

	// Set control's position and size.
	// @warning: We must use m_Position and m_Size, and not Position nor Size.
	SetPosition(m_Position);
	SetSize(m_Size);
}

auto JWMenuBar::AddMenuBarItem(const WSTRING& Text)->THandleItem
{
	// Calculate item's position.
	D3DXVECTOR2 item_position = m_Position;
	if (m_pItems.size())
	{
		item_position.x = m_pItems[m_pItems.size() - 1]->GetPosition().x;
		item_position.x += m_pItems[m_pItems.size() - 1]->GetSize().x;
	}

	// Calculate item's size.
	D3DXVECTOR2 item_size = m_Size;
	item_size.x = static_cast<float>(m_pSharedData->pText->GetFontData()->Info.Size * Text.length())
		+ static_cast<float>(DEFAULT_MENUBAR_ITEM_PADDING * 2);

	// Create item.
	m_pItems.push_back(MAKE_UNIQUE_AND_MOVE(MenuItem)());
	auto* p_new_item = m_pItems[m_pItems.size() - 1].get();

	p_new_item->Create(item_position, item_size, *m_pSharedData);
	p_new_item->ShouldBeOffsetByMenuBar(false);
	p_new_item->ShouldDrawBorder(false);
	p_new_item->ShouldUseToggleSelection(true);
	p_new_item->SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);
	p_new_item->SetText(Text);
	p_new_item->SetControlStateColor(EControlState::Normal, DEFAULT_COLOR_BACKGROUND_MENUBAR);
	p_new_item->SetControlStateColor(EControlState::Hover, DEFAULT_COLOR_HOVER);
	p_new_item->SetControlStateColor(EControlState::Pressed, DEFAULT_COLOR_ALMOST_BLACK);


	// Calculate sub-item box's position.
	D3DXVECTOR2 subitembox_position = item_position;
	subitembox_position.y += static_cast<float>(DEFAULT_MENUBAR_HEIGHT);

	// Create sub-item box.
	m_pSubItemBoxes.push_back(MAKE_UNIQUE_AND_MOVE(MenuSubItemBox)());
	auto* p_new_subitem_box = m_pSubItemBoxes[m_pSubItemBoxes.size() - 1].get();

	p_new_subitem_box->Create(subitembox_position, BLANK_SUBITEMBOX_SIZE, *m_pSharedData);
	p_new_subitem_box->ShouldBeOffsetByMenuBar(false);
	p_new_subitem_box->ShouldUseAutomaticScrollBar(false);
	p_new_subitem_box->ShouldUseToggleSelection(false);
	p_new_subitem_box->ShouldDrawBorder(false);
	p_new_subitem_box->SetBackgroundColor(DEFAULT_COLOR_LESS_BLACK);


	// Calculate background's position.
	D3DXVECTOR2 background_position = item_position + item_size;
	background_position.y = 0;

	m_pNonButtonRegion->SetPosition(background_position);

	return GetTHandleItemOfMenuBarItem(m_pItems.size() - 1);
}

auto JWMenuBar::AddMenuBarSubItem(THandleItem hItem, const WSTRING& Text) noexcept->THandleItem
{
	TIndex item_index = GetTIndexOfMenuBarItem(hItem);

	m_pSubItemBoxes[item_index]->AddListBoxItem(Text);

	D3DXVECTOR2 new_size = BLANK_SUBITEMBOX_SIZE;
	new_size.y = m_pSubItemBoxes[item_index]->GetListBoxItemHeight();
	m_pSubItemBoxes[item_index]->SetSize(new_size);

	THandleItem Result = hItem + m_pSubItemBoxes[item_index]->GetListBoxItemCount();
	return Result;
}

PRIVATE auto JWMenuBar::GetTHandleItemOfMenuBarItem(TIndex ItemIndex) noexcept->THandleItem
{
	THandleItem Result = MENU_ITEM_THANDLE_BASE;
	Result += (static_cast<THandleItem>(ItemIndex) * MENU_ITEM_THANDLE_STRIDE);

	return Result;
}

PRIVATE auto JWMenuBar::GetTIndexOfMenuBarItem(THandleItem hItem) noexcept->TIndex
{
	TIndex Result = static_cast<TIndex>(hItem - MENU_ITEM_THANDLE_BASE);
	Result = Result / MENU_ITEM_THANDLE_STRIDE;

	return Result;
}

void JWMenuBar::KillFocus() noexcept
{
	JWControl::KillFocus();

	// When menu-bar lose focus, we must unselect the selected item.
	UnselectMenuBarItem();
}

PROTECTED void JWMenuBar::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept
{
	JWControl::UpdateControlState(nullptr, ppControlWithFocus);

	m_pNonButtonRegion->UpdateControlState(nullptr, nullptr);

	if ((m_pNonButtonRegion->GetControlState() == EControlState::Pressed) || (m_pNonButtonRegion->GetControlState() == EControlState::Clicked))
	{
		UnselectMenuBarItem();
	}

	for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
	{
		m_pItems[iterator]->UpdateControlState(nullptr, nullptr);

		// Toggle-able JWTextButton doesn't have Clicked state, so we need to check only the Pressed state.
		if ((m_pItems[iterator]->GetControlState() == EControlState::Pressed))
		{
			SelectMenuBarItem(iterator);
		}
		else if ((m_pItems[iterator]->GetControlState() == EControlState::Hover))
		{
			if (m_pSelectedItem)
			{
				SelectMenuBarItem(iterator);
			}
		}
		else
		{
			if (m_pItems[iterator].get() == m_pSelectedItem)
			{
				UnselectMenuBarItem();
			}
		}
	}

	if (m_pSelectedItem)
	{
		// Sub-item box must have the mouse cursor, when it appears.
		m_pSelectedSubItemBox->UpdateControlState(ppControlWithMouse, nullptr);

		if (m_pSelectedSubItemBox->GetControlState() == EControlState::Clicked)
		{
			m_hSelectedSubItem = GetTHandleItemOfMenuBarItem(m_SelectedItemIndex) + m_pSelectedSubItemBox->GetSelectedItemIndex() + 1;

			UnselectMenuBarItem();
		}
	}
}

void JWMenuBar::Draw() noexcept
{
	JWControl::BeginDrawing();

	// Draw non-button region.
	m_pNonButtonRegion->Draw();

	// Draw items.
	if (m_pItems.size())
	{
		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			m_pItems[iterator]->Draw();
		}
	}

	// Draw the subitem box, if necessary.
	if (m_pSelectedItem)
	{
		m_pSelectedSubItemBox->Draw();
	}

	JWControl::EndDrawing();
}

auto JWMenuBar::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl&
{
	JWControl::SetSize(Size);

	m_pNonButtonRegion->SetSize(m_Size);

	return *this;
}

auto JWMenuBar::OnSubItemClick() noexcept->THandleItem
{
	THandleItem Result = m_hSelectedSubItem;

	m_hSelectedSubItem = THandleItem_Null;

	return Result;
}

PRIVATE void JWMenuBar::SelectMenuBarItem(TIndex ItemIndex) noexcept
{
	if (m_pSelectedItem)
	{
		// IF,
		// there is a previously selected item,
		// unselect it.

		if (m_pSelectedItem != m_pItems[ItemIndex].get())
		{
			m_pSelectedItem->SetControlState(EControlState::Normal);
		}
	}

	m_pSelectedItem = m_pItems[ItemIndex].get();
	m_pSelectedSubItemBox = m_pSubItemBoxes[ItemIndex].get();

	m_SelectedItemIndex = ItemIndex;

	m_pSelectedItem->SetControlState(EControlState::Pressed);

	SetControlState(EControlState::Pressed);
}

PRIVATE void JWMenuBar::UnselectMenuBarItem() noexcept
{
	if (m_pSelectedItem)
	{
		m_pSelectedItem->SetControlState(EControlState::Normal);
	}

	m_pSelectedItem = nullptr;
	m_pSelectedSubItemBox = nullptr;

	m_SelectedItemIndex = TIndex_Invalid;
}