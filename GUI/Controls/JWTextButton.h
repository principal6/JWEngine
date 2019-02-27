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
	friend class JWMenuBar;
	friend class JWScrollBar;

	public:
		JWTextButton();
		~JWTextButton() {};

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) override;

		void Draw() override;

		auto SetPosition(const D3DXVECTOR2& Position)->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size)->JWControl* override;

		auto ShouldUseToggleSelection(bool Value)->JWControl* override;

		void KillFocus() override;

	private:
		JWImage* m_pBackground;

		bool m_bShouleUseToggleSelection;
		bool m_bToggleState;
	};
};