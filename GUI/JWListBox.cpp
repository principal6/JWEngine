#include "JWListBox.h"
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

	m_bHasScrollBar = false;
	m_bShouldUseAutomaticScrollBar = true;
	m_bShouleUseToggleSelection = true;
	m_bUseImageItems = false;

	m_SelectedItemIndex = TIndex_NotSpecified;

	m_pTextureForImageItem = nullptr;

	m_MinimumItemHeight = DEFAULT_ITEM_HEIGHT;
}

auto JWListBox::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size, pSharedData)))
		return EError::CONTROL_NOT_CREATED;

	// Set control type
	m_ControlType = EControlType::ListBox;

	// Create image for background
	if (m_pBackground = new JWImage)
	{
		if (JW_FAILED(m_pBackground->Create(m_pSharedData->pWindow, &m_pSharedData->BaseDir)))
			return EError::IMAGE_NOT_CREATED;

		m_pBackground->SetPosition(Position);
		m_pBackground->SetSize(Size);
		m_pBackground->SetAlpha(DEFUALT_ALPHA_BACKGROUND_LISTBOX);
		m_pBackground->SetXRGB(DEFAULT_COLOR_BACKGROUND_LISTBOX);
	}
	else
	{
		return EError::ALLOCATION_FAILURE;
	}

	// Create ScrollBar
	if (m_pScrollBar = new JWScrollBar)
	{
		if (JW_FAILED(m_pScrollBar->Create(Position, Size, m_pSharedData)))
			return EError::SCROLLBAR_NOT_CREATED;

		m_pScrollBar->MakeScrollBar(EScrollBarDirection::Vertical);
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

void JWListBox::Destroy()
{
	JWControl::Destroy();

	if (m_pImageItems.size())
	{
		for (size_t iterator = 0; iterator < m_pImageItems.size(); iterator++)
		{
			JW_DESTROY(m_pImageItems[iterator]);
		}
	}

	if (m_ItemInfo.size())
	{
		for (size_t iterator = 0; iterator < m_ItemInfo.size(); iterator++)
		{
			JW_DESTROY(m_pItemBackground[iterator]);
			JW_DESTROY(m_pTextItems[iterator]);
		}
	}

	JW_DESTROY(m_pBackground);
	JW_DESTROY(m_pScrollBar);
}

void JWListBox::SetMinimumItemHeight(float Value)
{
	m_MinimumItemHeight = Value;
	m_MinimumItemHeight = max(m_MinimumItemHeight, MINIMUM_ITEM_HEIGHT);
}

void JWListBox::ShouldUseImageItem(LPDIRECT3DTEXTURE9 pTexture, D3DXIMAGE_INFO* pInfo)
{
	m_bUseImageItems = true;

	m_pTextureForImageItem = pTexture;
	m_pTextureForImageItemInfo = pInfo;
}

void JWListBox::AddListBoxItem(WSTRING Text, D3DXVECTOR2 OffsetInAtlas, D3DXVECTOR2 SizeInAtlas)
{
	// Insert space at the head of the text
	// in order to enhance legibility.
	Text = L" " + Text;

	// Get this new item's index.
	size_t item_index = m_pTextItems.size();
	
	// Calculate item's default position.
	D3DXVECTOR2 item_position = D3DXVECTOR2(0, 0);
	item_position = m_Position;
	item_position.x += DEFAULT_ITEM_PADDING_X;
	item_position.y += DEFAULT_ITEM_PADDING_Y;
	if (item_index)
	{
		// IF,
		// this isn't the first item in the listbox,
		// it needs offsetting.
		item_position.y = m_ItemInfo[item_index - 1].ItemPosition.y + m_ItemInfo[item_index - 1].ItemSize.y;
	}

	// Calculate item's default size.
	D3DXVECTOR2 item_size = D3DXVECTOR2(0, 0);
	item_size.x = m_Size.x - DEFAULT_ITEM_PADDING_Y;
	item_size.y = m_MinimumItemHeight;

	/*
	** Add image item, if needed.
	*/
	if (m_bUseImageItems)
	{
		JWImageBox* new_image_item = new JWImageBox;

		// Calculate image item's size.
		D3DXVECTOR2 image_item_size = item_size;
		image_item_size.y = SizeInAtlas.y;
		if (image_item_size.y > item_size.y)
		{
			// Adjust item's height when image item's height exceeds it.
			item_size.y = image_item_size.y;
		}

		new_image_item->Create(item_position, image_item_size, m_pSharedData);
		new_image_item->SetBackgroundColor(D3DCOLOR_ARGB(0, 0, 0, 0));
		new_image_item->SetTextureAtlas(m_pTextureForImageItem, m_pTextureForImageItemInfo);
		new_image_item->SetAtlasUV(OffsetInAtlas, SizeInAtlas);
		new_image_item->ShouldUseViewport(false);

		m_pImageItems.push_back(new_image_item);
	}


	/*
	** Add item's background (always)
	*/
	JWImageBox* new_item_background = new JWImageBox;
	new_item_background->Create(item_position, item_size, m_pSharedData);
	new_item_background->ShouldUseViewport(false);
	if (m_bShouleUseToggleSelection)
	{
		// IF,
		// this ListBox uses toggle selection.
		SetToggleSelectionColor(new_item_background);
	}
	else
	{
		// IF,
		// this ListBox doesn't use toggle selection.
		SetNonToggleSelectionColor(new_item_background);
	}
	m_pItemBackground.push_back(new_item_background);


	/*
	** Add text item (always).
	*/
	JWLabel* new_text_item = new JWLabel;

	// Calculate text item's position.
	D3DXVECTOR2 text_item_position = D3DXVECTOR2(item_position.x + SizeInAtlas.x, item_position.y);

	// Calculate text item's size.
	D3DXVECTOR2 text_item_size = D3DXVECTOR2(item_size.x - SizeInAtlas.x, item_size.y);

	new_text_item->Create(text_item_position, text_item_size, m_pSharedData);
	new_text_item->SetText(Text);
	new_text_item->SetTextVerticalAlignment(EVerticalAlignment::Middle);
	new_text_item->SetBackgroundColor(D3DCOLOR_ARGB(0, 0, 0, 0));
	new_text_item->ShouldUseViewport(false);

	m_pTextItems.push_back(new_text_item);


	/*
	** Add item info (always).
	*/
	SListBoxItemInfo new_info;
	new_info.ItemPosition = item_position;
	new_info.ItemSize = item_size;
	new_info.ImageItemSize = SizeInAtlas;

	m_ItemInfo.push_back(new_info);


	// If this listbox uses automatic scrollbar,
	// update it.
	if (m_bShouldUseAutomaticScrollBar)
	{
		UpdateAutomaticScrollBar();
	}
}

PRIVATE void JWListBox::UpdateAutomaticScrollBar()
{
	m_bHasScrollBar = false;

	float item_y_size_sum = 0;
	size_t item_count_in_size = 0;

	if (m_ItemInfo.size())
	{
		for (size_t iterator = 0; iterator < m_ItemInfo.size(); iterator++)
		{
			item_y_size_sum += m_ItemInfo[iterator].ItemSize.y;

			if (item_y_size_sum > m_Size.y)
			{
				if (!m_bHasScrollBar)
				{
					m_bHasScrollBar = true;
					item_count_in_size = iterator - 1;
				}
			}
		}
	}

	if (m_bHasScrollBar)
	{
		size_t item_rest = m_ItemInfo.size() - item_count_in_size;
		
		m_pScrollBar->SetScrollRange(item_count_in_size, m_ItemInfo.size());

		D3DXVECTOR2 scrollbar_size = D3DXVECTOR2(0, m_Size.y);
		m_pScrollBar->SetSize(scrollbar_size);

		D3DXVECTOR2 scrollbar_position = m_Position;
		scrollbar_position.x += m_Size.x;
		scrollbar_position.x -= m_pScrollBar->GetSize().x;
		m_pScrollBar->SetPosition(scrollbar_position);
	}
}

void JWListBox::UpdateControlState(const SMouseData& MouseData)
{
	// Static in-function variables
	static long long current_scroll_position = 0;
	static D3DXVECTOR2 item_position = D3DXVECTOR2(0, 0);

	JWControl::UpdateControlState(MouseData);

	m_pScrollBar->UpdateControlState(MouseData);

	if (!m_bHasScrollBar)
	{
		// IF,
		// this ListBox doesn't have ScrollBar,
		// (although technically it does, the ScrollBar's invisible now.)
		// set the control state of the ScrollBar to Normal.
		m_pScrollBar->SetState(EControlState::Normal);
	}
	else
	{
		// IF,
		// this ListBox has ScrollBar,

		if (IsMouseOver(MouseData))
		{
			// We use mouse wheel scroll only when the mouse is over the control.
			if (MouseData.MouseWheeled)
			{
				current_scroll_position = m_pScrollBar->GetScrollPosition();

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
		// IF,
		// ListBox is not being scrolled,
		// update items' control state.

		if (!m_bShouleUseToggleSelection)
		{
			// IF,
			// ListBox uses toggle selection,
			// selected item index doesn't change
			// until another one is selected!
			// but if it doesn't use toggle selction,
			// we must initialize m_SelectedItemIndex every time.
			m_SelectedItemIndex = TIndex_NotSpecified;
		}

		if (m_ItemInfo.size())
		{
			for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
			{
				m_pItemBackground[iterator]->UpdateControlState(MouseData);

				if (m_pItemBackground[iterator]->GetState() == EControlState::Clicked)
				{
					// Save the selected item's index.
					m_SelectedItemIndex = iterator;
				}
			}
		}
	}
	else
	{
		// IF,
		// the ListBox is being scrolled,
		// scroll items.

		m_ItemOffsetY = m_Position.y + DEFAULT_ITEM_PADDING_Y - m_ItemInfo[m_pScrollBar->GetScrollPosition()].ItemPosition.y;

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

	if (m_bShouleUseToggleSelection)
	{
		// If this ListBox uses toggle selection,
		// change the selected and non-selected items' default color.
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
}

void JWListBox::Draw()
{
	JWControl::BeginDrawing();

	// Draw background
	m_pBackground->Draw();

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
	if (m_bHasScrollBar)
	{
		m_pScrollBar->Draw();
	}

	JWControl::EndDrawing();
}

void JWListBox::SetBackgroundColor(DWORD Color)
{
	JWControl::SetBackgroundColor(Color);

	m_pBackground->SetColor(Color);
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

auto JWListBox::GetListBoxItemCount() const->const size_t
{
	return m_ItemInfo.size();
}

auto JWListBox::GetListBoxItemHeight() const->const float
{
	float item_y_size_sum = DEFAULT_ITEM_PADDING_Y;

	if (m_ItemInfo.size())
	{
		for (size_t iterator = 0; iterator < m_ItemInfo.size(); iterator++)
		{
			item_y_size_sum += m_ItemInfo[iterator].ItemSize.y;
		}
	}

	return item_y_size_sum;
}

auto JWListBox::GetSelectedItemIndex() const->const TIndex
{
	return m_SelectedItemIndex;
}

void JWListBox::ShouldUseAutomaticScrollBar(bool Value)
{
	m_bShouldUseAutomaticScrollBar = Value;
}

void JWListBox::ShouldUseToggleSelection(bool Value)
{
	m_bShouleUseToggleSelection = Value;

	if (m_bShouleUseToggleSelection)
	{
		// IF,
		// this ListBox uses toggle selection.
		if (m_pItemBackground.size())
		{
			for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
			{
				SetToggleSelectionColor(m_pItemBackground[iterator]);
			}
		}
	}
	else
	{
		// IF,
		// this ListBox doesn't use toggle selection.
		if (m_pItemBackground.size())
		{
			for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
			{
				SetNonToggleSelectionColor(m_pItemBackground[iterator]);
			}
		}
	}
	
}

PRIVATE void JWListBox::SetToggleSelectionColor(JWImageBox* pItemBackground)
{
	// If this ListBox uses toggle selection,
	// the items' background should not be changed when it's hovered.

	pItemBackground->SetStateColor(EControlState::Normal, DEFAULT_COLOR_ALMOST_BLACK);
	pItemBackground->SetStateColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_BLACK); // See this.
	pItemBackground->SetStateColor(EControlState::Pressed, DEFAULT_COLOR_LESS_BLACK);
}

PRIVATE void JWListBox::SetNonToggleSelectionColor(JWImageBox* pItemBackground)
{
	// If this ListBox doesn't use toggle selection,
	// the default colors of the items' background should be changed when it's hovered.

	pItemBackground->SetStateColor(EControlState::Normal, DEFAULT_COLOR_ALMOST_BLACK);
	pItemBackground->SetStateColor(EControlState::Hover, DEFAULT_COLOR_LESS_BLACK); // See this.
	pItemBackground->SetStateColor(EControlState::Pressed, DEFAULT_COLOR_LESS_BLACK);
}