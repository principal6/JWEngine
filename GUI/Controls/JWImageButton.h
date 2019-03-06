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
			const D3DXVECTOR2& HoverOffset, const D3DXVECTOR2& PressedOffset) noexcept->JWControl* override;
		auto MakeSystemArrowButton(ESystemArrowDirection Direction) noexcept->JWControl* override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;

	private:
		UNIQUE_PTR<JWImage> m_pBackground;
		UNIQUE_PTR<JWImage> m_pButtonImage;

		D3DXVECTOR2 m_ButtonImagePositionOffset{ 0, 0 };
		bool m_bHorzFlip{ false };
		bool m_bVertFlip{ false };

		D3DXVECTOR2 m_ButtonSizeInTexture{ 0, 0 };
		D3DXVECTOR2 m_NormalOffset{ 0, 0 };
		D3DXVECTOR2 m_HoverOffset{ 0, 0 };
		D3DXVECTOR2 m_PressedOffset{ 0, 0 };
	};
};