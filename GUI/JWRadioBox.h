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

		auto Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;

		void SetCheckState(bool Value) override;
		auto GetCheckState() const->bool override;

	private:
		JWImage* m_pBackground;

		bool m_bChecked;
	};
};