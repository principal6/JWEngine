#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImageBox;
	// ***

	class JWFrame final : public JWControl
	{
	public:
		JWFrame() {};
		~JWFrame() {};

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData)->JWControl* override;
		void Destroy() noexcept override;

		auto AddChildControl(JWControl& ChildControl) noexcept->JWControl* override;

		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;

	protected:

	private:
		JWImageBox* m_pBackground = nullptr;

		VECTOR<JWControl*> m_pChildControls;
	};
};