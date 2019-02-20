#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWText;
	class JWImageBox;
	class JWRectangle;
	// ***

	class JWEdit final : public JWControl
	{
	public:
		JWEdit();
		~JWEdit() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void Draw() override;

	protected:
		// Events called in JWGUIWindow (friend class).

	private:
		static const DWORD DEFAULT_COLOR_BACKGROUND_EDIT = DEFAULT_COLOR_ALMOST_BLACK;

		JWImageBox* m_pBackground;

		JWText* m_pEditText;
	};
};