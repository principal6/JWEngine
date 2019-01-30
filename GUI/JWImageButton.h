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

		auto Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;

		void SetImageButtonDireciton(EImageButtonDirection Direction) override;

	private:
		JWImage* m_pBackground;
		JWImage* m_pButtonImage;

		D3DXVECTOR2 m_ButtonImageOffset;
		D3DXVECTOR2 m_OffsetInAtlas;
		bool m_bHorzFlip;
		bool m_bVertFlip;
	};
};