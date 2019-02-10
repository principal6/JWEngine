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
	public:
		JWImageButton();
		~JWImageButton() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void MakeImageButton(WSTRING TextureAtlasFileName, D3DXVECTOR2 ButtonSizeInTexture, D3DXVECTOR2 NormalOffset,
			D3DXVECTOR2 HoverOffset, D3DXVECTOR2 PressedOffset) override;
		void MakeSystemArrowButton(ESystemArrowDirection Direction) override;

		// A button must override this method.
		// In order to set the button hovered, even when the mouse is not over the control,
		// if the mouse was pressed on the control.
		void UpdateControlState(const SMouseData& MouseData) override;

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;

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