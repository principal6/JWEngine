#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWFrame final : public JWControl
	{
	public:
		JWFrame() {};
		~JWFrame() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		auto AddChildControl(JWControl& ChildControl) noexcept->JWControl* override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;

	protected:
		// Must be overridden.
		void UpdateViewport() noexcept;

		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept override;

	private:
		UNIQUE_PTR<JWImage> m_pBackground{};

		VECTOR<JWControl*> m_pChildControls;
	};
};