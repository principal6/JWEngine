#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	class JWFont;
	// ***

	class JWButton final : public JWControl
	{
	public:
		JWButton() {};
		~JWButton() {};

		auto Create(JWWindow* pWindow, WSTRING BaseDir)->EError override;
		void Destroy() override;

	private:
		JWImage* m_pImage;
		JWFont* m_pFont;
	};
};