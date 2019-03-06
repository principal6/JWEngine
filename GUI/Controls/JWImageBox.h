#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWImageBox final : public JWControl
	{
	friend class JWEdit;
	friend class JWListBox;
	friend class JWMenuBar;

	public:
		JWImageBox() {};
		~JWImageBox() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl* override;
		auto SetTextureAtlas(const LPDIRECT3DTEXTURE9 pTextureAtlas, const D3DXIMAGE_INFO* pTextureAtlasInfo) noexcept->JWControl* override;
		auto SetAtlasUV(const D3DXVECTOR2& OffsetInAtlas, const D3DXVECTOR2& Size) noexcept->JWControl* override;
		auto SetBackgroundColor(DWORD Color) noexcept->JWControl* override;

	private:
		UNIQUE_PTR<JWImage> m_pBackground{};
		UNIQUE_PTR<JWImage> m_pImage{};

		D3DXVECTOR2 m_OffsetInAtlas{ 0, 0 };
	};
};