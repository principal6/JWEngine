#include "JWListBox.h"
#include "../../CoreBase/JWImage.h"
#include "../../CoreBase/JWWindow.h"

using namespace JWENGINE;

void JWListBox::Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept
{
	JWControl::Create(Position, Size, SharedData);

	// Set control type
	m_ControlType = EControlType::ListBox;

	// A listbox must have border.
	m_bShouldDrawBorder = true;

	// Create image for background
	m_pBackground = MAKE_UNIQUE(JWImageBox)();
	m_pBackground->Create(m_Position, m_Size, SharedData);
	m_pBackground->SetParentControl(this);
	m_pBackground->SetBackgroundColor(DEFAULT_COLOR_BACKGROUND_LISTBOX);
	m_pBackground->ShouldBeOffsetByMenuBar(false);

	// Create ScrollBar
	m_pScrollBar = MAKE_UNIQUE(JWScrollBar)();
	m_pScrollBar->Create(m_Position, m_Size, SharedData);
	m_pScrollBar->MakeScrollBar(EScrollBarDirection::Vertical);
	m_pScrollBar->SetParentControl(this);
	m_pScrollBar->ShouldBeOffsetByMenuBar(false);

	// Set control's position and size.
	SetPosition(Position);
	SetSize(Size);
}

auto JWListBox::SetMinimumItemHeight(float Value) noexcept->JWControl*
{
	m_MinimumItemHeight = Value;
	m_MinimumItemHeight = max(m_MinimumItemHeight, MINIMUM_ITEM_HEIGHT);

	return this;
}

auto JWListBox::SetImageItemTextureAtlas(const LPDIRECT3DTEXTURE9 pTexture, const D3DXIMAGE_INFO* pInfo) noexcept->JWControl*
{
	m_pTextureForImageItem = pTexture;
	m_pTextureForImageItemInfo = pInfo;

	return this;
}

auto JWListBox::AddListBoxItem(const WSTRING& Text, const D3DXVECTOR2& OffsetInAtlas, const D3DXVECTOR2& SizeInAtlas)->JWControl*
{
	// Get this new item's index.
	size_t item_index = m_pTextItems.size();
	
	// Calculate item's default position.
	D3DXVECTOR2 item_position_offset{ 0, 0 };
	if (item_index)
	{
		// IF,
		// this isn't the first item in the listbox,
		// it needs offsetting.
		item_position_offset.y = m_ItemInfo[item_index - 1].ItemPositionOffset.y + m_ItemInfo[item_index - 1].ItemSize.y;
	}

	// Calculate item's default size.
	D3DXVECTOR2 item_size{ 0, 0 };
	item_size.x = m_Size.x;
	item_size.y = m_MinimumItemHeight;

	/*
	** Add image item, if needed.
	*/
	if (m_bUseImageItems)
	{
		// Calculate image item's size.
		D3DXVECTOR2 image_item_size = item_size;
		image_item_size.y = SizeInAtlas.y;

		// Adjust item's height when image item's height exceeds it.
		item_size.y = max(item_size.y, image_item_size.y);

		m_pImageItems.push_back(MAKE_UNIQUE_AND_MOVE(JWImageBox)());
		auto p_new_image_item = m_pImageItems[m_pImageItems.size() - 1].get();
		
		p_new_image_item->Create(item_position_offset, image_item_size, *m_pSharedData);
		p_new_image_item->SetParentControl(this);
		p_new_image_item->SetBackgroundColor(D3DCOLOR_ARGB(0, 0, 0, 0));
		p_new_image_item->SetTextureAtlas(m_pTextureForImageItem, m_pTextureForImageItemInfo);
		p_new_image_item->SetAtlasUV(OffsetInAtlas, SizeInAtlas);
		p_new_image_item->ShouldBeOffsetByMenuBar(false);
		p_new_image_item->ShouldBeOffsetByParent(false);
	}


	/*
	** Add item's background (always)
	*/
	m_pItemBackground.push_back(MAKE_UNIQUE_AND_MOVE(JWImageBox)());
	auto* p_new_item_background = m_pItemBackground[m_pItemBackground.size() - 1].get();

	p_new_item_background->Create(item_position_offset, item_size, *m_pSharedData);
	p_new_item_background->SetParentControl(this);
	p_new_item_background->ShouldBeOffsetByMenuBar(false);
	if (m_bShouleUseToggleSelection)
	{
		// IF,
		// this ListBox uses toggle selection.
		SetToggleSelectionColor(p_new_item_background);
	}
	else
	{
		// IF,
		// this ListBox doesn't use toggle selection.
		SetNonToggleSelectionColor(p_new_item_background);
	}
	


	/*
	** Add text item (always).
	*/
	// Calculate text item's position.
	D3DXVECTOR2 text_item_position{ item_position_offset.x + SizeInAtlas.x, item_position_offset.y };

	// Calculate text item's size.
	D3DXVECTOR2 text_item_size{ item_size.x - SizeInAtlas.x, item_size.y };

	m_pTextItems.push_back(MAKE_UNIQUE_AND_MOVE(JWLabel)());
	auto* p_new_text_item = m_pTextItems[m_pTextItems.size() - 1].get();

	// Insert space at the head of the text
	// in order to enhance legibility.
	WSTRING adjusted_text = L" " + Text;
	
	p_new_text_item->Create(text_item_position, text_item_size, *m_pSharedData);
	p_new_text_item->SetParentControl(this);
	p_new_text_item->SetText(adjusted_text);
	p_new_text_item->SetTextVerticalAlignment(EVerticalAlignment::Middle);
	p_new_text_item->SetBackgroundColor(D3DCOLOR_ARGB(0, 0, 0, 0));
	p_new_text_item->ShouldBeOffsetByMenuBar(false);


	/*
	** Add item info (always).
	*/
	SListBoxItemInfo new_info;
	new_info.ItemPositionOffset = item_position_offset;
	new_info.ItemSize = item_size;
	new_info.ImageItemSize = SizeInAtlas;

	m_ItemInfo.push_back(new_info);


	// If this listbox uses automatic scrollbar,
	// update it.
	if (m_bShouldUseAutomaticScrollBar)
	{
		UpdateAutomaticScrollBar();
	}

	UpdateViewport();

	return this;
}

PRIVATE void JWListBox::UpdateAutomaticScrollBar() noexcept
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

		UpdateScrollBarPositionAndSize();
	}
}

PRIVATE void JWListBox::UpdateScrollBarPositionAndSize()
{
	if (m_bHasScrollBar)
	{
		D3DXVECTOR2 scrollbar_size{ 0, m_Size.y };

		m_pScrollBar->SetSize(scrollbar_size);

		D3DXVECTOR2 scrollbar_position{ m_Position };
		scrollbar_position.x += m_Size.x;
		scrollbar_position.x -= m_pScrollBar->GetSize().x;

		m_pScrollBar->SetPosition(scrollbar_position);
	}
}

PROTECTED void JWListBox::UpdateViewport() noexcept
{
	JWControl::UpdateViewport();

	m_pBackground->UpdateViewport();

	if (m_ItemInfo.size())
	{
		for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
		{
			m_pItemBackground[iterator]->UpdateViewport();
		}

		if (m_bUseImageItems)
		{
			for (size_t iterator = 0; iterator < m_pImageItems.size(); iterator++)
			{
				m_pImageItems[iterator]->UpdateViewport();
			}
		}

		for (size_t iterator = 0; iterator < m_pTextItems.size(); iterator++)
		{
			m_pTextItems[iterator]->UpdateViewport();
		}
	}

	if (m_bHasScrollBar)
	{
		m_pScrollBar->UpdateViewport();
	}
}

PROTECTED void JWListBox::UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept
{
	JWControl::UpdateControlState(ppControlWithMouse, ppControlWithFocus);

	const SWindowInputState* p_input_state = m_pSharedData->pWindow->GetWindowInputStatePtr();

	bool b_mouse_in_rect = Static_IsMouseInViewPort(p_input_state->MousePosition, m_ControlViewport);

	m_pScrollBar->UpdateControlState(nullptr, nullptr);

	if (!m_bHasScrollBar)
	{
		// IF,
		// this ListBox doesn't have ScrollBar,
		// (although technically it does, the ScrollBar's invisible now.)
		// set the control state of the ScrollBar to Normal.

		m_pScrollBar->SetControlState(EControlState::Normal);
	}
	else
	{
		// IF,
		// this ListBox has ScrollBar,

		if (b_mouse_in_rect)
		{
			// We must use mouse wheel scroll,
			// only when the mouse is over the control.

			if (p_input_state->MouseWheeled)
			{
				long long current_scroll_position = m_pScrollBar->GetScrollPosition();

				current_scroll_position -= (p_input_state->MouseWheeled / WHEEL_DELTA);
				current_scroll_position = max(current_scroll_position, 0);

				m_pScrollBar->SetScrollPosition(current_scroll_position);

				m_pScrollBar->SetControlState(EControlState::Hover);
			}
		}
	}

	if (m_pScrollBar->GetControlState() == EControlState::Normal)
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

			m_SelectedItemIndex = TIndex_Invalid;
		}

		if (m_ItemInfo.size())
		{
			for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
			{
				m_pItemBackground[iterator]->UpdateControlState(nullptr, nullptr);

				if (b_mouse_in_rect)
				{
					if (m_pItemBackground[iterator]->GetControlState() == EControlState::Clicked)
					{
						// Save the selected item's index.
						m_SelectedItemIndex = iterator;
					}
				}
			}
		}
	}
	else
	{
		// IF,
		// the ListBox is being scrolled,
		// scroll items.

		UpdateItemsPosition();
	}

	if (m_bShouleUseToggleSelection)
	{
		// IF,
		// this ListBox uses toggle selection,
		// change the selected and non-selected items' default color.

		for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
		{
			if (iterator == m_SelectedItemIndex)
			{
				m_pItemBackground[iterator]->SetControlStateColor(EControlState::Normal, DEFAULT_COLOR_LESS_BLACK);
				m_pItemBackground[iterator]->SetControlStateColor(EControlState::Hover, DEFAULT_COLOR_LESS_BLACK);
			}
			else
			{
				m_pItemBackground[iterator]->SetControlStateColor(EControlState::Normal, DEFAULT_COLOR_ALMOST_BLACK);
				m_pItemBackground[iterator]->SetControlStateColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_BLACK);
			}
		}
	}
}

PRIVATE void JWListBox::UpdateItemsPosition()
{
	if (m_ItemInfo.size())
	{
		D3DXVECTOR2 item_position{ 0, 0 };

		for (size_t iterator = 0; iterator < m_pItemBackground.size(); iterator++)
		{
			item_position = m_Position + m_ItemInfo[iterator].ItemPositionOffset;
			item_position.y -= m_ItemInfo[m_pScrollBar->GetScrollPosition()].ItemPositionOffset.y;

			m_pItemBackground[iterator]->SetPosition(item_position);

			if (m_bUseImageItems)
			{
				m_pImageItems[iterator]->SetPosition(item_position);
			}

			item_position.x += m_ItemInfo[iterator].ImageItemSize.x;

			m_pTextItems[iterator]->SetPosition(item_position);
		}
	}
}

void JWListBox::Draw() noexcept
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

auto JWListBox::SetBackgroundColor(DWORD Color) noexcept->JWControl*
{
	JWControl::SetBackgroundColor(Color);

	m_pBackground->SetBackgroundColor(Color);

	return this;
}

auto JWListBox::SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl*
{
	JWControl::SetPosition(Position);

	m_pBackground->SetPosition(m_Position);
	
	UpdateScrollBarPositionAndSize();

	UpdateItemsPosition();

	return this;
}

auto JWListBox::SetSize(const D3DXVECTOR2& Size) noexcept->JWControl*
{
	JWControl::SetSize(Size);

	m_pBackground->SetSize(m_Size);
	
	UpdateScrollBarPositionAndSize();

	UpdateItemsPosition();

	return this;
}

auto JWListBox::GetListBoxItemCount() const noexcept->size_t
{
	return m_ItemInfo.size();
}

auto JWListBox::GetListBoxItemHeight() const noexcept->float
{
	//float item_y_size_sum = DEFAULT_ITEM_PADDING_Y;
	float item_y_size_sum = 0;

	if (m_ItemInfo.size())
	{
		for (size_t iterator = 0; iterator < m_ItemInfo.size(); iterator++)
		{
			item_y_size_sum += m_ItemInfo[iterator].ItemSize.y;
		}
	}

	return item_y_size_sum;
}

auto JWListBox::GetSelectedItemIndex() const noexcept->TIndex
{
	return m_SelectedItemIndex;
}

auto JWListBox::ShouldUseAutomaticScrollBar(bool Value) noexcept->JWControl*
{
	m_bShouldUseAutomaticScrollBar = Value;

	return this;
}

auto JWListBox::ShouldUseToggleSelection(bool Value) noexcept->JWControl*
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
				SetToggleSelectionColor(m_pItemBackground[iterator].get());
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
				SetNonToggleSelectionColor(m_pItemBackground[iterator].get());
			}
		}
	}
	
	return this;
}

auto JWListBox::ShouldUseImageItem(bool Value) noexcept->JWControl*
{
	m_bUseImageItems = Value;

	return this;
}

PRIVATE void JWListBox::SetToggleSelectionColor(JWImageBox* pItemBackground) noexcept
{
	// If this ListBox uses toggle selection,
	// the items' background should not be changed when it's hovered.

	pItemBackground->SetControlStateColor(EControlState::Normal, DEFAULT_COLOR_ALMOST_BLACK);
	pItemBackground->SetControlStateColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_BLACK); // See this.
	pItemBackground->SetControlStateColor(EControlState::Pressed, DEFAULT_COLOR_LESS_BLACK);
}

PRIVATE void JWListBox::SetNonToggleSelectionColor(JWImageBox* pItemBackground) noexcept
{
	// If this ListBox doesn't use toggle selection,
	// the default colors of the items' background should be changed when it's hovered.

	pItemBackground->SetControlStateColor(EControlState::Normal, DEFAULT_COLOR_ALMOST_BLACK);
	pItemBackground->SetControlStateColor(EControlState::Hover, DEFAULT_COLOR_LESS_BLACK); // See this.
	pItemBackground->SetControlStateColor(EControlState::Pressed, DEFAULT_COLOR_LESS_BLACK);
}