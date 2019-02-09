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

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUISharedData* pSharedData)->EError override;
		void Destroy() override;

		// A button must override this method.
		// In order to set the button hovered, even when the mouse is not over the control,
		// if the mouse was pressed on the control.
		void UpdateControlState(const SMouseData& MouseData) override;

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;

	private:
		JWImage* m_pImage;
	};
};