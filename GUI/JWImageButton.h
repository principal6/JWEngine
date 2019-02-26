#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	// ***

	class JWImageButton final : public JWControl
	{
	friend class JWScrollBar;

	public:
		JWImageButton();
		~JWImageButton() {};

		auto Create(const D3DXVECTOR2 Position, const D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		auto MakeImageButton(const WSTRING TextureAtlasFileName, const D3DXVECTOR2 ButtonSizeInTexture, const D3DXVECTOR2 NormalOffset,
			const D3DXVECTOR2 HoverOffset, const D3DXVECTOR2 PressedOffset)->JWControl* override;
		auto MakeSystemArrowButton(const ESystemArrowDirection Direction)->JWControl* override;

		void Draw() override;

		auto SetPosition(const D3DXVECTOR2 Position)->JWControl* override;
		auto SetSize(const D3DXVECTOR2 Size)->JWControl* override;

	private:
		JWImage* m_pBackground;
		JWImage* m_pButtonImage;

		D3DXVECTOR2 m_ButtonImagePositionOffset;
		bool m_bHorzFlip;
		bool m_bVertFlip;

		D3DXVECTOR2 m_ButtonSizeInTexture;
		D3DXVECTOR2 m_NormalOffset;
		D3DXVECTOR2 m_HoverOffset;
		D3DXVECTOR2 m_PressedOffset;
	};
};