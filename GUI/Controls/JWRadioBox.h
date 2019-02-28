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

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->JWControl* override;
		void Destroy() noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetCheckState(const bool Value) noexcept->JWControl* override;

		auto GetCheckState() const noexcept->const bool override;

	private:
		JWImage* m_pBackground;

		bool m_bChecked;
	};
};