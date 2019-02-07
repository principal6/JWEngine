#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWLabel final : public JWControl
	{
	public:
		JWLabel();
		~JWLabel() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;

		void Draw() override;

		void SetBackgroundColor(DWORD Color);

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;
		void SetUseMultiline(bool Value) override;

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND_LABEL = 255;
		static const DWORD DEFAULT_COLOR_BACKGROUND_LABEL = DEFAULT_COLOR_ALMOST_BLACK;
	};
};