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
	m_pFocusedItem = nullptr;
	m_pSubItemBox = nullptr;

	m_bMouseReleasedForTheFisrtTime = true;
}

auto JWMenuBar::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	// MenuBar's position is fixed!
	Position.x = 0;
	Position.y = 0;

	// MenuBar's size is fixed!
	Size.x = static_cast<float>(ms_pSharedData->pWindow->GetWindowData()->WindowWidth);
	Size.y = static_cast<float>(DEFAULT_MENUBAR_HEIGHT);

	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	// Set control type
	m_Type = EControlType::MenuBar;

	// Set default color
	m_pFont->SetBoxAlpha(0);

	// Create image for background
	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
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

	// Create menu subitem box
	if (m_pSubItemBox = new MenuSubItemBox)
	{
		if (JW_FAILED(m_pSubItemBox->Create(D3DXVECTOR2(0, 0), BLANK_SUBITEMBOX_SIZE)))
			return EError::IMAGE_NOT_CREATED;
		
		m_pSubItemBox->SetBackgroundColor(DEFAULT_COLOR_ALMOST_BLACK);
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

	return EError::OK;
}

void JWMenuBar::Destroy()
{
	if (m_pItems.size())
	{
		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			JW_DESTROY(m_pItems[iterator]);
		}
	}

	JW_DESTROY(m_pBackground);

	JWControl::Destroy();
}

auto JWMenuBar::AddMenuBarItem(WSTRING Text)->THandleItem
{
	THandle Result = MENU_ITEM_THANDLE_BASE;

	MenuItem* new_item = new MenuItem;

	D3DXVECTOR2 item_position = m_PositionClient;
	if (m_pItems.size())
	{
		item_position.x = m_pItems[m_pItems.size() - 1]->GetPosition().x;
		item_position.x += m_pItems[m_pItems.size() - 1]->GetSize().x;
	}

	D3DXVECTOR2 item_size = m_Size;
	item_size.x = static_cast<float>(m_pFont->GetFontData()->Info.Size * (Text.length() + 2));
	
	new_item->Create(item_position, item_size);
	new_item->SetText(Text);
	new_item->SetAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle);
	new_item->ShouldDrawBorder(false);
	new_item->SetStateColor(EControlState::Normal, DEFAULT_COLOR_BACKGROUND_MENUBAR);
	new_item->SetStateColor(EControlState::Hover, DEFAULT_COLOR_HOVER);
	new_item->SetStateColor(EControlState::Pressed, DEFAULT_COLOR_ALMOST_BLACK);

	m_pItems.push_back(new_item);

	Result += static_cast<THandle>(m_pItems.size()) - 1;

	return Result;
}

void JWMenuBar::UpdateControlState(const SMouseData& MouseData)
{
	JWControl::UpdateControlState(MouseData);

	// Kill focus on menu item when mouse is pressed out of region.
	if (ms_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
	{
		bool FocusCheck = false;

		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			if (m_pItems[iterator]->IsMouseOver(MouseData))
			{
				FocusCheck = true;
			}
		}

		if (m_pFocusedItem)
		{
			if (m_pSubItemBox->IsMouseOver(MouseData))
			{
				FocusCheck = true;
			}
		}

		if (!FocusCheck)
		{
			if (m_pFocusedItem)
			{
				m_pFocusedItem->SetState(EControlState::Normal);
				m_pFocusedItem = nullptr;
				m_bMouseReleasedForTheFisrtTime = true;

				SetState(EControlState::Normal);
			}
		}
	}

	// Set or kill focus on menu item when mouse is pressed on menu items.
	for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
	{
		if (m_pItems[iterator]->IsMouseOver(MouseData))
		{
			if (m_pFocusedItem)
			{
				if (m_pFocusedItem != m_pItems[iterator])
				{
					m_pFocusedItem->SetState(EControlState::Normal);
					m_pFocusedItem = m_pItems[iterator];
					m_pFocusedItem->SetState(EControlState::Pressed);
				}
				else
				{
					if (ms_pSharedData->pWindow->GetWindowInputState()->MouseLeftReleased)
					{
						if (m_bMouseReleasedForTheFisrtTime)
						{
							m_bMouseReleasedForTheFisrtTime = false;

						}
						else
						{
							m_pFocusedItem->SetState(EControlState::Normal);
							m_pFocusedItem = nullptr;
							m_bMouseReleasedForTheFisrtTime = true;
						}
					}
				}
			}
			else
			{
				if (ms_pSharedData->pWindow->GetWindowInputState()->MouseLeftPressed)
				{
					m_pFocusedItem = m_pItems[iterator];
					m_pFocusedItem->SetState(EControlState::Pressed);
				}
				else
				{
					m_pItems[iterator]->SetState(EControlState::Hover);
				}
			}
		}
		else
		{
			if (m_pFocusedItem != m_pItems[iterator])
			{
				m_pItems[iterator]->SetState(EControlState::Normal);
			}
		}
	}

	if (m_pFocusedItem)
	{
		D3DXVECTOR2 subitembox_position = m_pFocusedItem->GetPosition();
		subitembox_position.y += static_cast<float>(DEFAULT_MENUBAR_HEIGHT);

		m_pSubItemBox->SetPosition(subitembox_position);
	}

}

void JWMenuBar::Draw()
{
	JWControl::BeginDrawing();

	// Draw background
	m_pBackground->Draw();

	// Draw text
	m_pFont->Draw();

	// Draw items
	if (m_pItems.size())
	{
		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			m_pItems[iterator]->Draw();
		}
	}

	if (m_pFocusedItem)
	{
		m_pSubItemBox->Draw();
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

	if (m_pFocusedItem)
	{
		if (m_pSubItemBox->IsMouseOver(MouseData))
		{
			Result = true;
		}
	}

	return Result;
}