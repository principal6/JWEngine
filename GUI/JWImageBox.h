#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	// ***

	class JWImageBox final : public JWControl
	{
	public:
		JWImageBox();
		~JWImageBox() {};

		auto Create(D3DXVECTOR2 Position, D3DXVECTOR2 Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void Draw() override;

		void SetPosition(D3DXVECTOR2 Position) override;
		void SetSize(D3DXVECTOR2 Size) override;
		auto SetTextureAtlas(LPDIRECT3DTEXTURE9 pTextureAtlas, D3DXIMAGE_INFO* pTextureAtlasInfo)->JWControl* override;
		auto SetAtlasUV(D3DXVECTOR2 OffsetInAtlas, D3DXVECTOR2 Size)->JWControl* override;
		void SetBackgroundColor(DWORD Color) override;

	private:
		DWORD m_ColorBackground;

		JWImage* m_pBackground;
		JWImage* m_pImage;

		D3DXVECTOR2 m_OffsetInAtlas;
	};
};