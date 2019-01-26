#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	// ***

	class JWButton final : public JWControl
	{
	public:
		JWButton() {};
		~JWButton() {};

		auto Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void Update(const SMouseData& MouseData) override;
		void Draw() override;

	private:
		JWImage* m_pImage;
	};
};