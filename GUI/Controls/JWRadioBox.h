#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	// ***

	class JWRadioBox final : public JWControl
	{
	public:
		JWRadioBox();
		~JWRadioBox() {};

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void Draw() override;

		auto SetPosition(const D3DXVECTOR2& Position)->JWControl* override;
		auto SetCheckState(const bool Value)->JWControl* override;

		auto GetCheckState() const->const bool override;

	private:
		JWImage* m_pBackground;

		bool m_bChecked;
	};
};