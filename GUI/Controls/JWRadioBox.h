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

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData)->JWControl* override;
		void Destroy() noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetCheckState(bool Value) noexcept->JWControl* override;

		auto GetCheckState() const noexcept->bool override;

	private:
		JWImage* m_pBackground = nullptr;

		bool m_bChecked = false;
	};
};