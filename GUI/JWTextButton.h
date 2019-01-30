#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	// ***

	class JWTextButton final : public JWControl
	{
	public:
		JWTextButton();
		~JWTextButton() {};

		auto Create(JWWindow* pWindow, WSTRING BaseDir, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError override;
		void Destroy() override;

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;
		void SetButtonColor(EControlState State, DWORD Color) override;

	private:
		JWImage* m_pImage;

		DWORD m_Color_Normal;
		DWORD m_Color_Hover;
		DWORD m_Color_Pressed;
	};
};