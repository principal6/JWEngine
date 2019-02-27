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

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void Draw() override;

		auto SetPosition(const D3DXVECTOR2& Position)->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size)->JWControl* override;
		auto SetCheckState(const bool Value)->JWControl* override;

		auto GetCheckState() const->const bool override;

	private:
		JWImage* m_pBackground;
		JWImage* m_pCheckImage;

		D3DXVECTOR2 m_ButtonImageOffset;
		D3DXVECTOR2 m_OffsetInAtlas;

		bool m_bChecked;
	};
};