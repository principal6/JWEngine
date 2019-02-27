#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWImage;
	//class JWListBox;
	// ***

	class JWImageBox final : public JWControl
	{
	friend class JWEdit;
	friend class JWListBox;
	friend class JWMenuBar;

	public:
		JWImageBox();
		~JWImageBox() {};

		auto Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData* pSharedData)->EError override;
		void Destroy() override;

		void Draw() override;

		auto SetPosition(const D3DXVECTOR2& Position)->JWControl* override;
		auto SetSize(const D3DXVECTOR2& Size)->JWControl* override;
		auto SetTextureAtlas(const LPDIRECT3DTEXTURE9 pTextureAtlas, const D3DXIMAGE_INFO* pTextureAtlasInfo)->JWControl* override;
		auto SetAtlasUV(const D3DXVECTOR2& OffsetInAtlas, const D3DXVECTOR2& Size)->JWControl* override;
		auto SetBackgroundColor(DWORD Color)->JWControl* override;

	private:
		DWORD m_ColorBackground;

		JWImage* m_pBackground;
		JWImage* m_pImage;

		D3DXVECTOR2 m_OffsetInAtlas;
	};
};