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

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->JWControl* override;
		void Destroy() noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;
		auto SetCheckState(const bool Value) noexcept->JWControl* override;

		auto GetCheckState() const noexcept->const bool override;

	private:
		JWImage* m_pBackground;
		JWImage* m_pCheckImage;

		D3DXVECTOR2 m_ButtonImageOffset;
		D3DXVECTOR2 m_OffsetInAtlas;

		bool m_bChecked;
	};
};