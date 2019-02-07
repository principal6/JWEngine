#include "JWListBox.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWImage.h"
#include "JWLabel.h"
#include "JWScrollBar.h"

using namespace JWENGINE;

// Static const
const float JWListBox::DEFUALT_ITEM_HEIGHT = 20.0f;

JWListBox::JWListBox()
{
	// A listbox must have border.
	m_bShouldDrawBorder = true;

	m_bShouldHaveScrollBar = false;

	m_SelectedItemIndex = JW_NOT_SPECIFIED;
}

auto JWListBox::Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError
{
	if (JW_FAILED(JWControl::Create(Position, Size)))
		return EError::CONTROL_NOT_CREATED;

	// Set control type
	m_Type = EControlType::Label;

	// Set default color
	m_pFont->SetBoxAlpha(0);

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
	if (m_pItems.size())
	{
		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			JW_DESTROY(m_pItems[iterator]);
		}
	}

	JW_DESTROY(m_pBackground);
	JW_DESTROY(m_pScrollBar);

	JWControl::Destroy();
}

void JWListBox::AddTextItem(WSTRING Text)
{
	Text = L" " + Text;
	size_t item_index = m_pItems.size();
	
	JWLabel* new_item = new JWLabel;
	D3DXVECTOR2 item_position = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 item_size = D3DXVECTOR2(0, 0);
	item_position = m_PositionClient;
	item_position.x += 1.0f;
	item_position.y += 1.0f + static_cast<float>(DEFUALT_ITEM_HEIGHT * item_index);
	item_size.x = m_Size.x - 1.0f;
	item_size.y = DEFUALT_ITEM_HEIGHT;

	new_item->Create(item_position, item_size);
	new_item->SetText(Text);
	new_item->SetVerticalAlignment(EVerticalAlignment::Middle);
	new_item->SetStateColor(EControlState::Normal, DEFAULT_COLOR_ALMOST_BLACK);
	new_item->SetStateColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_BLACK);
	new_item->SetStateColor(EControlState::Pressed, DEFAULT_COLOR_LESS_BLACK);
	new_item->ShouldUseViewport(false);

	m_pItems.push_back(new_item);

	UpdateScrollBarData();
}

void JWListBox::UpdateScrollBarData()
{
	if (static_cast<float>(1.0f + DEFUALT_ITEM_HEIGHT * m_pItems.size()) >= m_Size.y)
	{
		m_bShouldHaveScrollBar = true;

		size_t item_count_in_size = static_cast<size_t>(m_Size.y / DEFUALT_ITEM_HEIGHT);
		size_t item_rest = m_pItems.size() - item_count_in_size;
		
		m_pScrollBar->SetScrollRange(item_count_in_size, m_pItems.size());

		D3DXVECTOR2 scrollbar_size = m_Size;
		scrollbar_size.x = 10.0f;
		m_pScrollBar->SetSize(scrollbar_size);

		D3DXVECTOR2 scrollbar_position = m_PositionClient;
		scrollbar_position.x += m_Size.x;
		scrollbar_position.x -= m_pScrollBar->GetSize().x;
		m_pScrollBar->SetPosition(scrollbar_position);
	}
	else
	{
		m_bShouldHaveScrollBar = false;
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

	if (m_pScrollBar->GetState() == EControlState::Normal)
	{
		// Update items' control state
		if (m_pItems.size())
		{
			for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
			{
				m_pItems[iterator]->UpdateControlState(MouseData);

				if (m_pItems[iterator]->GetState() == EControlState::Clicked)
				{
					m_SelectedItemIndex = iterator;
				}
			}
		}
	}
	else
	{
		// Scroll items
		m_ItemOffsetY = -static_cast<float>(m_pScrollBar->GetScrollPosition()) * DEFUALT_ITEM_HEIGHT;

		D3DXVECTOR2 item_position = D3DXVECTOR2(0, 0);
		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			item_position = m_PositionClient;
			item_position.x += 1.0f;
			item_position.y += 1.0f + static_cast<float>(DEFUALT_ITEM_HEIGHT * iterator) + m_ItemOffsetY;

			m_pItems[iterator]->SetPosition(item_position);
		}
	}

	// Set selected item's color
	for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
	{
		if (iterator == m_SelectedItemIndex)
		{
			m_pItems[iterator]->SetStateColor(EControlState::Normal, DEFAULT_COLOR_LESS_BLACK);
			m_pItems[iterator]->SetStateColor(EControlState::Hover, DEFAULT_COLOR_LESS_BLACK);
		}
		else
		{
			m_pItems[iterator]->SetStateColor(EControlState::Normal, DEFAULT_COLOR_ALMOST_BLACK);
			m_pItems[iterator]->SetStateColor(EControlState::Hover, DEFAULT_COLOR_ALMOST_BLACK);
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
	if (m_pItems.size())
	{
		for (size_t iterator = 0; iterator < m_pItems.size(); iterator++)
		{
			m_pItems[iterator]->Draw();
		}
	}

	// Draw ScrollBar
	if (m_bShouldHaveScrollBar)
	{
		m_pScrollBar->Draw();
	}

	JWControl::EndDrawing();
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