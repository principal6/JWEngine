#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWRadioBox final : public JWControl
	{
	public:
		JWRadioBox() {};
		~JWRadioBox() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl& override;
		auto SetCheckState(bool Value) noexcept->JWControl& override;

		auto GetCheckState() const noexcept->bool override;

	private:
		JWImage m_Background;

		bool m_bChecked{ false };
	};
};