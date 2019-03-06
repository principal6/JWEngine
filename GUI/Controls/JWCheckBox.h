#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWCheckBox final : public JWControl
	{
	public:
		JWCheckBox() {};
		~JWCheckBox() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;
		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;
		auto SetCheckState(bool Value) noexcept->JWControl* override;

		auto GetCheckState() const noexcept->bool override;

	private:
		UNIQUE_PTR<JWImage> m_pBackground{};
		UNIQUE_PTR<JWImage> m_pCheckImage{};

		D3DXVECTOR2 m_ButtonImageOffset{ 0, 0 };
		D3DXVECTOR2 m_OffsetInAtlas{ 0, 0 };

		bool m_bChecked{ false };
	};
};