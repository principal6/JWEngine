#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	// ***

	class JWTextButton final : public JWControl
	{
	public:
		JWTextButton();
		~JWTextButton() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void UpdateControlState(JWControl** ppControlWithFocus) override;

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;

		void ShouldUseToggleSelection(bool Value) override;

	private:
		JWImage* m_pBackground;

		bool m_bShouleUseToggleSelection;
	};
};