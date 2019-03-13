#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWTextButton final : public JWControl
	{
	friend class JWMenuBar;
	friend class JWScrollBar;

	public:
		JWTextButton() {};
		~JWTextButton() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl& override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl& override;

		auto ShouldUseToggleSelection(bool Value) noexcept->JWControl& override;

		void KillFocus() noexcept override;

	private:
		JWImage m_Background;

		bool m_bShouleUseToggleSelection{ false };
		bool m_bToggleState{ false };
	};
};