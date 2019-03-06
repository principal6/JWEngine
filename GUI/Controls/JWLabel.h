#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWLabel final : public JWControl
	{
	friend class JWListBox;

	public:
		JWLabel() {};
		~JWLabel() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;

	private:
		static const BYTE DEFUALT_ALPHA_BACKGROUND_LABEL{ 255 };
		static const DWORD DEFAULT_COLOR_BACKGROUND_LABEL{ DEFAULT_COLOR_ALMOST_BLACK };

		UNIQUE_PTR<JWImage> m_pBackground{};
	};
};