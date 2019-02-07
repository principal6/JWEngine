#include "JWListBox.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"
#include "JWLabel.h"
#include "JWScrollBar.h"
#include "JWImageBox.h"

using namespace JWENGINE;

// Static const
const float JWListBox::MINIMUM_ITEM_HEIGHT = 16.0f;
const float JWListBox::DEFAULT_ITEM_HEIGHT = 22.0f;
const float JWListBox::DEFAULT_ITEM_PADDING_X = 1.0f;
const float JWListBox::DEFAULT_ITEM_PADDING_Y = 1.0f;

JWListBox::JWListBox()
{
	// A listbox must have border.
	m_bShouldDrawBorder = true;

	m_bShouldHaveScrollBar = false;
	m_bUseImageItems = false;

	m_SelectedItemIndex = TIndex_NotSpecified;

	m_pTextureForImageItem = nullptr;

	m_MinimumItemHeight = DEFAULT_ITEM_HEIGHT;
}

auto JWListBox::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	// Set control type
	m_Type = EControlType::ListBox;

	// Set default color
	m_pFont->SetBoxColor(GetMixedColor(0, DEFAULT_COLOR_NORMAL));

	// Create image for background
	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(ms_pSharedData->pWindow, &ms_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetPosition(Position);
		m_pBackground->SetSize(Size);
		m_pBackground->SetAlpha(DEFUALT_ALPHA_BACKGROUND_LISTBOX);
		m_pBackground->SetXRGB(DEFAULT_COLOR_BACKGROUND_LISTBOX);
	}
	else
	{
		return EError::NULLPTR_IMAGE;
	}

	// Create ScrollBar
	if (m_pScrollBar = new JWScrollBar)
	{
		if (JW_FAILED(m_pScrollBar->Create(Position, Size)))
			return EError::SCROLLBAR_NOT_CREATED;

		m_pScrollBar->MakeScrollBar(EScrollBarDirection::Vertical);
	}
	else
	{
		return EError::NULLPTR_SCROLLBAR;
	}

	// Set control's size and position.
	SetSize(Size);
	SetPosition(Position);

	return EError::OK;
}

void JWListBox::Destroy()
{
	if (m_pTextItems.size())
	{
		for (size_t iterator = 0; iterator < m_pTextItems.size(); iterator++)
		{
			JW_DESTROY(m_pTextItems[iterator]);
		}
	}

	JW_DESTROY(m_pBackground);
	JW_DESTROY(m_pScrollBar);

	JWControl::Destroy();
}

void JWListBox::SetMinimumItemHeight(float Value)
{
	m_MinimumItemHeight = Value;
	m_MinimumItemHeight = max(m_MinimumItemHeight, MINIMUM_ITEM_HEIGHT);
}

void JWListBox::UseImageItem(LPDIRECT3DTEXTURE9 pTexture, D3DXIMAGE_INFO* pInfo)
{
	m_bUseImageItems = true;

	m_pTextureForImageItem = pTexture;
	m_pTextureForImageItemInfo = pInfo;
}

void JWListBox::AddListBoxItem(WSTRING Text, D3DXVECTOR2 OffsetInAtlas, D3DXVECTOR2 SizeInAtlas)
{
	Text = L" " + Text;
	size_t item_index = m_pTextItems.size();
	
	D3DXVECTOR2 item_position = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 item_size = D3DXVECTOR2(0, 0);
	item_position = m_PositionClient;
	item_position.x += DEFAULT_ITEM_PADDING_X;
	item_position.y += DEFAULT_ITEM_PADDING_Y;
	if (item_index)
	{
		item_position.y = m_ItemInfo[item_index - 1].ItemPosition.y + m_ItemInfo[item_index - 1].ItemSize.y;
	}
	item_size.x = m_Size.x - DEFAULT_ITEM_PADDING_Y;
	item_size.y = m_MinimumItemHeight;

	// Add image item
	if (m_bUseImageItems)
	{
		JWImageBox* new_image_item = new JWImageBox;

		D3DXVECTOR2 image_item_size = item_size;
		image_item_size.y = SizeInAtlas.y;

		if (image_item_size.y > item_size.y)
		{
			item_size.y = image_item_size.y;
		}

		new_image_item->Create(item_position, image_item_size);
		new_image_item->SetBackgroundColor(D3DCOLOR_ARGB(0, 0, 0, 0));
		new_image_item->SetTextureAtlas(m_pTextureForImageItem, m_pTextureForImageItemInfo);
		new_image_item->SetAtlasUV(OffsetInAtlas, SizeInAtlas);
		new_image_item->ShouldUseViewport(false);

		m_pImageItems.push_back(new_image_item);
	}

	// Add text item
	JWLabel* new_text_item = new JWLabel;

	D3DXVECTOR2 text_item_position = item_position;
	text_item_position.x += SizeInAtlas.x;

	D3DXVECTOR2 text_item_size = item_size;
	text_item_size.x -= SizeInAtlas.x;

	new_text_item->Create(text_item_position, text_item_size);
	new_text_item->SetText(Text);
	new_text_item->SetVerticalAlignment(EVerticalAlignment::Middle);
	new_text_item->SetBackgroundColor(D3DCOLOR_ARGB(0, 0, 0, 0));
	new_text_item->ShouldUseViewport(false);

	m_pTextItems.push_back(new_text_item);


	// Add item's background
	JWImageBox* new_item_background = new JWImageBox;
	new_item_background->Create(item_position, item_size);
	new_item_background->ShouldUseViewport(false);
	new_item_background->SetStateColor(EControlState::Normal, DEFAULT_COLOR_ALMOST_BLACK);
	new_item_background->SetStateColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_BLACK);
	new_item_background->SetStateColor(EControlState::Pressed, DEFAULT_COLOR_LESS_BLACK);

	m_pItemBackground.push_back(new_item_background);


	// Add item info
	SListBoxItemInfo new_info;
	new_info.ItemPosition = item_position;
	new_info.ItemSize = item_size;
	new_info.ImageItemSize = SizeInAtlas;

	m_ItemInfo.push_back(new_info);

	UpdateScrollBarData();
}

void JWListBox::UpdateScrollBarData()
{
	m_bShouldHaveScrollBar = false;

	float item_y_size_sum = 0;
	size_t item_count_in_size = 0;

	if (m_ItemInfo.size())
	{
		for (size_t iterator = 0; iterator < m_ItemInfo.size(); iterator++)
		{
			item_y_size_sum += m_ItemInfo[iterator].ItemSize.y;

			if (item_y_size_sum >= m_Size.y)
			{
				if (!m_bShouldHaveScrollBar)
				{
					m_bShouldHaveScrollBar = true;
					item_count_in_size = iterator - 1;
				}
			}
		}
	}

	if (m_bShouldHaveScrollBar)
	{
		size_t item_rest = m_ItemInfo.size() - item_count_in_size;
		
		m_pScrollBar->SetScrollRange(item_count_in_size, m_ItemInfo.size());

		D3DXVECTOR2 scrollbar_size = m_Size;
		scrollbar_size.x = 10.0f;
		m_pScrollBar->SetSize(scrollbar_size);

		D3DXVECTOR2 scrollbar_position = m_PositionClient;
		scrollbar_position.x += m_Size.x;
		scrollbar_position.x -= m_pScrollBar->GetSize().x;
		m_pScrollBar->SetPosition(scrollbar_position);
	}
}

void JWListBox::UpdateControlState(const SMouseData& MouseData)
{
	JWControl::UpdateControlState(MouseData);

	m_pScrollBar->UpdateControlState(MouseData);

	if (!m_bShouldHaveScrollBar)
	{
		m_pScrollBar->SetState(EControlState::Normal);
	}
	else
	{
		if (IsMouseOver(MouseData))
		{
			// We use mouse wheel scroll only when the mouse is over the control.
			if (MouseData.MouseWheeled)
			{
				long long current_scroll_position = m_pScrollBar->GetScrollPosition();

				if (MouseData.MouseWheeled > 0)
				{
					current_scroll_position--;
					current_scroll_position = max(current_scroll_position, 0);

					m_pScrollBar->SetScrollPosition(current_scroll_position);
				}
				else
				{
					current_scroll_position++;

					m_pScrollBar->SetScrollPosition(current_scroll_position);
				}

				m_pScrollBar->SetState(EControlState::Hover);
			}
		}
	}

	if (m_pScrollBar->GetState() == EControlState::Normal)
	{
		// Update items' control state
		if (m_ItemInfo.size())
		{
			for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
			{
				m_pItemBackground[iterator]->UpdateControlState(MouseData);

				if (m_pItemBackground[iterator]->GetState() == EControlState::Clicked)
				{
					m_SelectedItemIndex = iterator;
				}
			}
		}
	}
	else
	{
		// Scroll items
		m_ItemOffsetY = m_PositionClient.y + DEFAULT_ITEM_PADDING_Y - m_ItemInfo[m_pScrollBar->GetScrollPosition()].ItemPosition.y;

		D3DXVECTOR2 item_position = D3DXVECTOR2(0, 0);
		for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
		{
			item_position = m_ItemInfo[iterator].ItemPosition;
			item_position.y += m_ItemOffsetY;

			m_pItemBackground[iterator]->SetPosition(item_position);
			
			if (m_bUseImageItems)
			{
				m_pImageItems[iterator]->SetPosition(item_position);
			}

			item_position.x += m_ItemInfo[iterator].ImageItemSize.x;

			m_pTextItems[iterator]->SetPosition(item_position);
		}
	}

	// Set selected item's color
	for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
	{
		if (iterator == m_SelectedItemIndex)
		{
			m_pItemBackground[iterator]->SetStateColor(EControlState::Normal, DEFAULT_COLOR_LESS_BLACK);
			m_pItemBackground[iterator]->SetStateColor(EControlState::Hover, DEFAULT_COLOR_LESS_BLACK);
		}
		else
		{
			m_pItemBackground[iterator]->SetStateColor(EControlState::Normal, DEFAULT_COLOR_ALMOST_BLACK);
			m_pItemBackground[iterator]->SetStateColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_BLACK);
		}
	}
}

void JWListBox::Draw()
{
	JWControl::BeginDrawing();

	// Draw background
	m_pBackground->Draw();

	// Draw text
	m_pFont->Draw();

	// Draw items
	if (m_ItemInfo.size())
	{
		for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
		{
			m_pItemBackground[iterator]->Draw();
		}

		if (m_bUseImageItems)
		{
			for (size_t iterator = 0; iterator < m_pImageItems.size(); iterator++)
			{
				m_pImageItems[iterator]->Draw();
			}
		}

		for (size_t iterator = 0; iterator < m_pTextItems.size(); iterator++)
		{
			m_pTextItems[iterator]->Draw();
		}
	}

	// Draw ScrollBar
	if (m_bShouldHaveScrollBar)
	{
		m_pScrollBar->Draw();
	}

	JWControl::EndDrawing();
}

void JWListBox::SetBackgroundColor(DWORD Color)
{
	JWControl::SetBackgroundColor(Color);

	m_pBackground->SetAlpha(GetColorAlpha(Color));
	m_pBackground->SetXRGB(GetColorXRGB(Color));
}

void JWListBox::SetPosition(D3DXVECTOR2 Position)
{
	JWControl::SetPosition(Position);
	m_pBackground->SetPosition(Position);
}

void JWListBox::SetSize(D3DXVECTOR2 Size)
{
	JWControl::SetSize(Size);
	m_pBackground->SetSize(Size);
}