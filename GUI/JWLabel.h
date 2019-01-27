#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWLabel final : public JWControl
	{
	public:
		JWLabel() {};
		~JWLabel() {};

		auto Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;

		void Draw() override;

		void SetBackgroundAlpha(BYTE Alpha);
		void SetBackgroundXRGB(DWORD XRGB);

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND = D3DCOLOR_XRGB(80, 80, 80);
	};
};