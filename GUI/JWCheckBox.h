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

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUISharedData* pSharedData)->EError override;
		void Destroy() override;

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;

		void SetCheckState(bool Value) override;
		auto GetCheckState() const->bool override;

	private:
		JWImage* m_pBackground;
		JWImage* m_pCheckImage;

		D3DXVECTOR2 m_ButtonImageOffset;
		D3DXVECTOR2 m_OffsetInAtlas;

		bool m_bChecked;
	};
};