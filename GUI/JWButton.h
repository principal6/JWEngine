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
		JWButton();
		~JWButton() {};

		auto Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;

	private:
		JWImage* m_pImage;
	};
};