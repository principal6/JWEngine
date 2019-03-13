#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWImageButton final : public JWControl
	{
		friend class JWScrollBar;

	public:
		JWImageButton() {};
		~JWImageButton() = default;

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		auto MakeImageButton(const WSTRING& TextureAtlasFileName, const D3DXVECTOR2& ButtonSizeInTexture, const D3DXVECTOR2& NormalOffset,
			const D3DXVECTOR2& HoverOffset, const D3DXVECTOR2& PressedOffset)->JWControl& override;
		auto MakeSystemArrowButton(ESystemArrowDirection Direction) noexcept->JWControl& override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl& override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl& override;

	private:
		JWImage m_Background;
		JWImage m_ButtonImage;

		D3DXVECTOR2 m_ButtonImagePositionOffset{};
		bool m_bHorzFlip{ false };
		bool m_bVertFlip{ false };

		D3DXVECTOR2 m_ButtonSizeInTexture{};
		D3DXVECTOR2 m_NormalOffset{};
		D3DXVECTOR2 m_HoverOffset{};
		D3DXVECTOR2 m_PressedOffset{};
	};
};