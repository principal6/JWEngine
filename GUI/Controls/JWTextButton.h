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

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData)->JWControl* override;
		void Destroy() noexcept override;

		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;

		auto ShouldUseToggleSelection(bool Value) noexcept->JWControl* override;

		void KillFocus() noexcept override;

	private:
		JWImage* m_pBackground = nullptr;

		bool m_bShouleUseToggleSelection = false;
		bool m_bToggleState = false;
	};
};