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
		JWFrame();
		~JWFrame() {};

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		auto AddChildControl(JWControl* pChildControl)->JWControl* override;

		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) override;

		void Draw() override;

		auto SetPosition(const D3DXVECTOR2& Position)->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size)->JWControl* override;

	protected:

	private:
		JWImageBox* m_pBackground;

		VECTOR<JWControl*> m_pChildControls;
	};
};