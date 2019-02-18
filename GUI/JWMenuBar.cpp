#include "JWMenuBar.h"
#include "../CoreBase/JWText.h"
#include "../CoreBase/JWWindow.h"
#include "JWImageBox.h"
#include "JWTextButton.h"
#include "JWListBox.h"

using namespace JWENGINE;

// Static const
const D3DXVECTOR2 JWMenuBar::BLANK_SUBITEMBOX_SIZE = D3DXVECTOR2(100.0f, 60.0f);

JWMenuBar::JWMenuBar()
{
	// A menubar must not have border.
	m_bShouldDrawBorder = false;

	m_pNonButtonRegion = nullptr;

	m_pSelectedItem = nullptr;
	m_SelectedItemIndex = TIndex_NotSpecified;
	m_pSelectedSubItemBox = nullptr;
}

auto JWMenuBar::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
		return EError::CONTROL_NOT_CREATED;

	// MenuBar's position must be fixed!
	m_Position.x = 0;
	m_Position.y = 0;

	// MenuBar's size must be fixed!
	m_Size.x = static_cast<float>(m_pSharedData->pWindow->GetWindowData()->ScreenSize.x);
	m_Size.y = static_cast<float>(DEFAULT_MENUBAR_HEIGHT);

	// Set control type
	m_ControlType = EControlType::MenuBar;

	// Create ImageBox for the non-button region.
	if (m_pNonButtonRegion = new JWImageBox)
	{
		if (JW_FAILED(m_pNonButtonRegion->Create(Position, Size, pSharedData)))
			return EError::IMAGE_BOX_NOT_CREATED;

		m_pNonButtonRegion->SetBackgroundColor(DEFAULT_COLOR_BACKGROUND_MENUBAR);
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Set control's size and position.
	SetPosition(m_Position);
	SetSize(m_Size);

	return EError::OK;
}

void JWMenuBar::Destroy()
{
	JWControl::Destroy();

	if (m_pItems.size())
	{
		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			JW_DESTROY(m_pItems[iterator]);
			JW_DESTROY(m_pSubItemBoxes[iterator]);
		}

		m_pItems.clear();
		m_pSubItemBoxes.clear();
	}

	JW_DESTROY(m_pNonButtonRegion);
}

auto JWMenuBar::AddMenuBarItem(WSTRING Text)->THandleItem
{
	//THandleItem Result = MENU_ITEM_THANDLE_BASE;

	MenuItem* new_item = new MenuItem;

	D3DXVECTOR2 item_position = m_Position;
	if (m_pItems.size())
	{
		item_position.x = m_pItems[m_pItems.size() - 1]->GetPosition().x;
		item_position.x += m_pItems[m_pItems.size() - 1]->GetSize().x;
	}

	D3DXVECTOR2 item_size = m_Size;
	item_size.x = static_cast<float>(m_pSharedData->pText->GetFontData()->Info.Size * Text.length())
		+ static_cast<float>(DEFAULT_MENUBAR_ITEM_PADDING * 2);
	
	new_item->Create(item_position, item_size, m_pSharedData);
	new_item->SetText(Text);
	new_item->SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);
	new_item->ShouldDrawBorder(false);
	new_item->ShouldUseToggleSelection(true);
	new_item->SetStateColor(EControlState::Normal, DEFAULT_COLOR_BACKGROUND_MENUBAR);
	new_item->SetStateColor(EControlState::Hover, DEFAULT_COLOR_HOVER);
	new_item->SetStateColor(EControlState::Pressed, DEFAULT_COLOR_ALMOST_BLACK);

	m_pItems.push_back(new_item);

	//Result += (static_cast<THandle>(m_pItems.size()) - 1) * MENU_ITEM_THANDLE_STRIDE;

	// Create sub-item box
	MenuSubItemBox* new_subitem_box = new MenuSubItemBox;
	D3DXVECTOR2 subitembox_position = item_position;
	subitembox_position.y += static_cast<float>(DEFAULT_MENUBAR_HEIGHT);
	new_subitem_box->Create(subitembox_position, BLANK_SUBITEMBOX_SIZE, m_pSharedData);
	new_subitem_box->SetBackgroundColor(DEFAULT_COLOR_LESS_BLACK);
	new_subitem_box->ShouldUseAutomaticScrollBar(false);
	new_subitem_box->ShouldUseToggleSelection(false);

	m_pSubItemBoxes.push_back(new_subitem_box);

	D3DXVECTOR2 background_position = item_position + item_size;
	background_position.y = 0;

	m_pNonButtonRegion->SetPosition(background_position);

	return GetTHandleItemOfMenuBarItem(m_pItems.size() - 1);
	//return Result;
}

auto JWMenuBar::AddMenuBarSubItem(THandleItem hItem, WSTRING Text)->THandleItem
{
	TIndex item_index = GetTIndexOfMenuBarItem(hItem);

	m_pSubItemBoxes[item_index]->AddListBoxItem(Text);

	D3DXVECTOR2 new_size = BLANK_SUBITEMBOX_SIZE;
	new_size.y = m_pSubItemBoxes[item_index]->GetListBoxItemHeight();
	m_pSubItemBoxes[item_index]->SetSize(new_size);

	THandleItem Result = hItem + m_pSubItemBoxes[item_index]->GetListBoxItemCount();
	return Result;
}

PRIVATE auto JWMenuBar::GetTHandleItemOfMenuBarItem(TIndex ItemIndex)->THandleItem
{
	THandleItem Result = MENU_ITEM_THANDLE_BASE;
	Result += (static_cast<THandleItem>(ItemIndex) * MENU_ITEM_THANDLE_STRIDE);
	return Result;
}

PRIVATE auto JWMenuBar::GetTIndexOfMenuBarItem(THandleItem hItem)->TIndex
{
	TIndex Result = static_cast<TIndex>(hItem - MENU_ITEM_THANDLE_BASE);
	Result = Result / MENU_ITEM_THANDLE_STRIDE;
	return Result;
}

void JWMenuBar::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus)
{
	JWControl::UpdateControlState(ppControlWithMouse, ppControlWithFocus);

	m_pNonButtonRegion->UpdateControlState(nullptr, nullptr);

	if ((m_pNonButtonRegion->GetState() == EControlState::Pressed) || (m_pNonButtonRegion->GetState() == EControlState::Clicked))
	{
		UnselectItem();
	}

	for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
	{
		m_pItems[iterator]->UpdateControlState(nullptr, nullptr);

		// Toggle-able JWTextButton doesn't have Clicked state, so we need to check only the Pressed state.
		if ((m_pItems[iterator]->GetState() == EControlState::Pressed))
		{
			SelectItem(iterator);
		}
		else if ((m_pItems[iterator]->GetState() == EControlState::Hover))
		{
			if (m_pSelectedItem)
			{
				SelectItem(iterator);
			}
		}
		else
		{
			if (m_pItems[iterator] == m_pSelectedItem)
			{
				UnselectItem();
			}
		}
	}

	if (m_pSelectedItem)
	{
		m_pSelectedSubItemBox->UpdateControlState(nullptr, nullptr);

		if (m_pSelectedSubItemBox->GetState() == EControlState::Clicked)
		{
			m_hSelectedSubItem = GetTHandleItemOfMenuBarItem(m_SelectedItemIndex) + m_pSelectedSubItemBox->GetSelectedItemIndex() + 1;

			UnselectItem();
		}
	}
}

void JWMenuBar::Draw()
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

void JWMenuBar::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);
	m_pNonButtonRegion->SetSize(Size);
}

auto JWMenuBar::OnSubItemClick()->THandleItem
{
	THandleItem Result = m_hSelectedSubItem;

	m_hSelectedSubItem = THandle_Null;

	return Result;
}

PRIVATE void JWMenuBar::SelectItem(TIndex ItemIndex)
{
	if (m_pSelectedItem)
	{
		// IF,
		// there is a previously selected item,
		// unselect it.

		if (m_pSelectedItem != m_pItems[ItemIndex])
		{
			m_pSelectedItem->SetState(EControlState::Normal);
		}
	}

	m_pSelectedItem = m_pItems[ItemIndex];
	m_pSelectedSubItemBox = m_pSubItemBoxes[ItemIndex];

	m_SelectedItemIndex = ItemIndex;

	m_pSelectedItem->SetState(EControlState::Pressed);

	SetState(EControlState::Pressed);
}

PRIVATE void JWMenuBar::UnselectItem()
{
	if (m_pSelectedItem)
	{
		m_pSelectedItem->SetState(EControlState::Normal);
	}

	m_pSelectedItem = nullptr;
	m_pSelectedSubItemBox = nullptr;

	m_SelectedItemIndex = TIndex_NotSpecified;
}