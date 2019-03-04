#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	// ***

	class JWCheckBox final : public JWControl
	{
	public:
		JWCheckBox();
		~JWCheckBox() {};

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData)->JWControl* override;
		void Destroy() noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;
		auto SetCheckState(bool Value) noexcept->JWControl* override;

		auto GetCheckState() const noexcept->bool override;

	private:
		JWImage* m_pBackground = nullptr;
		JWImage* m_pCheckImage = nullptr;

		D3DXVECTOR2 m_ButtonImageOffset{ 0, 0 };
		D3DXVECTOR2 m_OffsetInAtlas{ 0, 0 };

		bool m_bChecked = false;
	};
};