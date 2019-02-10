#include "JWMenuBar.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWWindow.h"
#include "JWLabel.h"
#include "JWListBox.h"

using namespace JWENGINE;

// Static const
const D3DXVECTOR2 JWMenuBar::BLANK_SUBITEMBOX_SIZE = D3DXVECTOR2(100.0f, 60.0f);

JWMenuBar::JWMenuBar()
{
	// A menubar must not have border.
	m_bShouldDrawBorder = false;

	m_pBackground = nullptr;

	m_pSelectedItem = nullptr;
	m_SelectedItemIndex = TIndex_NotSpecified;
	m_pSelectedSubItemBox = nullptr;

	m_bMouseReleasedForTheFisrtTime = true;
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

	// Create image for background
	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetPosition(Position);
		m_pBackground->SetSize(Size);
		m_pBackground->SetAlpha(DEFUALT_ALPHA_BACKGROUND_MENUBAR);
		m_pBackground->SetXRGB(DEFAULT_COLOR_BACKGROUND_MENUBAR);
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

	JW_DESTROY(m_pBackground);
}

auto JWMenuBar::AddMenuBarItem(WSTRING Text)->THandleItem
{
	THandleItem Result = MENU_ITEM_THANDLE_BASE;

	MenuItem* new_item = new MenuItem;

	D3DXVECTOR2 item_position = m_Position;
	if (m_pItems.size())
	{
		item_position.x = m_pItems[m_pItems.size() - 1]->GetPosition().x;
		item_position.x += m_pItems[m_pItems.size() - 1]->GetSize().x;
	}

	D3DXVECTOR2 item_size = m_Size;
	item_size.x = static_cast<float>(m_pSharedData->pFont->GetFontData()->Info.Size * (Text.length() + 2));
	
	new_item->Create(item_position, item_size, m_pSharedData);
	new_item->SetText(Text);
	new_item->SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);
	new_item->ShouldDrawBorder(false);
	new_item->SetStateColor(EControlState::Normal, DEFAULT_COLOR_BACKGROUND_MENUBAR);
	new_item->SetStateColor(EControlState::Hover, DEFAULT_COLOR_HOVER);
	new_item->SetStateColor(EControlState::Pressed, DEFAULT_COLOR_ALMOST_BLACK);

	m_pItems.push_back(new_item);

	Result += (static_cast<THandle>(m_pItems.size()) - 1) * MENU_ITEM_THANDLE_STRIDE;

	// Create sub-item box
	MenuSubItemBox* new_subitem_box = new MenuSubItemBox;
	D3DXVECTOR2 subitembox_position = item_position;
	subitembox_position.y += static_cast<float>(DEFAULT_MENUBAR_HEIGHT);
	new_subitem_box->Create(subitembox_position, BLANK_SUBITEMBOX_SIZE, m_pSharedData);
	new_subitem_box->SetBackgroundColor(DEFAULT_COLOR_LESS_BLACK);
	new_subitem_box->ShouldUseAutomaticScrollBar(false);
	new_subitem_box->ShouldUseToggleSelection(false);

	m_pSubItemBoxes.push_back(new_subitem_box);
	
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

void JWMenuBar::UpdateControlState(const SMouseData& MouseData)
{
	JWControl::UpdateControlState(MouseData);

	// Kill focus on menu item when mouse is pressed out of region.
	if (m_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
	{
		bool FocusCheck = false;

		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			if (m_pItems[iterator]->IsMouseOver(MouseData))
			{
				FocusCheck = true;
			}
		}

		if (m_pSelectedItem)
		{
			if (m_pSelectedSubItemBox->IsMouseOver(MouseData))
			{
				FocusCheck = true;
			}
		}

		if (!FocusCheck)
		{
			if (m_pSelectedItem)
			{
				UnselectItem();

				SetState(EControlState::Normal);
			}
		}
	}

	// Set or kill focus on menu item when mouse is pressed on menu items.
	for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
	{
		if (m_pItems[iterator]->IsMouseOver(MouseData))
		{
			if (m_pSelectedItem)
			{
				if (m_pSelectedItem != m_pItems[iterator])
				{
					SelectItem(iterator);
				}
				else
				{
					if (m_pSharedData->pWindow->GetWindowInputState()->MouseLeftReleased)
					{
						if (m_bMouseReleasedForTheFisrtTime)
						{
							m_bMouseReleasedForTheFisrtTime = false;

						}
						else
						{
							UnselectItem();
						}
					}
				}
			}
			else
			{
				if (m_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
				{
					SelectItem(iterator);
				}
				else
				{
					m_pItems[iterator]->SetState(EControlState::Hover);
				}
			}
		}
		else
		{
			if (m_pSelectedItem != m_pItems[iterator])
			{
				// Set not-selected items' control state to Normal.
				m_pItems[iterator]->SetState(EControlState::Normal);
			}
		}
	}

	m_hSelectedSubItem = THandle_Null;
	if (m_pSelectedItem)
	{
		D3DXVECTOR2 subitembox_position = m_pSelectedItem->GetPosition();
		subitembox_position.y += static_cast<float>(DEFAULT_MENUBAR_HEIGHT);

		m_pSelectedSubItemBox->SetPosition(subitembox_position);
		m_pSelectedSubItemBox->UpdateControlState(MouseData);

		if (m_pSelectedSubItemBox->GetSelectedItemIndex() != TIndex_NotSpecified)
		{
			// IF,
			// an item is selected.

			TIndex selected_subitem_index = m_pSelectedSubItemBox->GetSelectedItemIndex();
			m_hSelectedSubItem = GetTHandleItemOfMenuBarItem(m_SelectedItemIndex) + selected_subitem_index + 1;

			UnselectItem();
		}
	}

}

void JWMenuBar::Draw()
{
	JWControl::BeginDrawing();

	// Draw background
	m_pBackground->Draw();

	// Draw items
	if (m_pItems.size())
	{
		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			m_pItems[iterator]->Draw();
		}
	}

	if (m_pSelectedItem)
	{
		m_pSelectedSubItemBox->Draw();
	}

	JWControl::EndDrawing();
}

void JWMenuBar::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);
	m_pBackground->SetSize(Size);
}

auto JWMenuBar::IsMouseOver(const SMouseData& MouseData)->bool
{
	bool Result = false;

	if (JWControl::IsMouseOver(MouseData))
	{
		Result = true;
	}

	if (m_pSelectedItem)
	{
		if (m_pSelectedSubItemBox->IsMouseOver(MouseData))
		{
			Result = true;
		}
	}

	return Result;
}

auto JWMenuBar::OnSubItemClick() const->THandleItem
{
	return m_hSelectedSubItem;
}

PRIVATE void JWMenuBar::SelectItem(TIndex ItemIndex)
{
	if (m_pSelectedItem)
	{
		// IF,
		// there is a previously selected item,
		// unselect it.
		m_pSelectedItem->SetState(EControlState::Normal);
	}

	m_pSelectedItem = m_pItems[ItemIndex];
	m_pSelectedSubItemBox = m_pSubItemBoxes[ItemIndex];
	m_SelectedItemIndex = ItemIndex;

	m_pSelectedItem->SetState(EControlState::Pressed);
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

	m_bMouseReleasedForTheFisrtTime = true;
}