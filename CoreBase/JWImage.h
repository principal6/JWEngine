#pragma once

#include "JWCommon.h"
#include "JWLine.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	// ***

	struct SBoundingBox
	{
		D3DXVECTOR2 PositionOffset;
		D3DXVECTOR2 Size;

		SBoundingBox() : PositionOffset(D3DXVECTOR2(0, 0)), Size(D3DXVECTOR2(0, 0)) {};
		SBoundingBox(D3DXVECTOR2 _POSOFFSET, D3DXVECTOR2 _SIZE) : PositionOffset(_POSOFFSET), Size(_SIZE) {};
	};

	class JWImage
	{
	public:
		JWImage();
		virtual ~JWImage() {};

		virtual auto Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir)->EError;
		virtual void Destroy();

		virtual void Draw();
		virtual void DrawBoundingBox();

		virtual void FlipHorizontal();
		virtual void FlipVertical();

		virtual void SetPosition(const D3DXVECTOR2 Position);
		virtual void SetPositionCentered(const D3DXVECTOR2 Position);
		virtual void SetSize(const D3DXVECTOR2 Size);
		virtual void SetTexture(const WSTRING FileName);
		virtual void SetTexture(const LPDIRECT3DTEXTURE9 pTexture, const D3DXIMAGE_INFO* pInfo);
		virtual auto SetColor(const DWORD Color)->JWImage*;
		virtual auto SetAlpha(const BYTE Alpha)->JWImage*;
		virtual auto SetXRGB(const DWORD XRGB)->JWImage*;
		virtual auto SetScale(const D3DXVECTOR2 Scale)->JWImage*;
		virtual auto SetVisibleRange(const D3DXVECTOR2 Range)->JWImage*;
		virtual auto SetAtlasUV(const D3DXVECTOR2 OffsetInAtlas, const D3DXVECTOR2 Size, bool bSetSize = true)->JWImage*;
		virtual auto SetUVRange(const STextureUV UV)->JWImage*;
		virtual auto SetBoundingBox(const D3DXVECTOR2 ExtraSize)->JWImage*;
		virtual auto SetBoundingBoxAlpha(const BYTE Alpha)->JWImage*;
		virtual auto SetBoundingBoxXRGB(const DWORD XRGB)->JWImage*;

		virtual auto GetSize() const->const D3DXVECTOR2;
		virtual auto GetScaledSize() const->const D3DXVECTOR2;
		virtual auto GetPosition() const->const D3DXVECTOR2;
		virtual auto GetCenterPosition() const->const D3DXVECTOR2;
		virtual auto GetBoundingBox() const->const SBoundingBox;

		virtual auto IsTextureLoaded() const->const bool;

	protected:
		virtual void ClearVertexAndIndexData();

		virtual void CreateVertexBuffer();
		virtual void CreateIndexBuffer();
		virtual void UpdateVertexBuffer();
		virtual void UpdateIndexBuffer();

		virtual void UpdateVertexData();
		virtual void UpdateVertexData(const STextureUV& UV);

	protected:
		static const int VISIBLE_RANGE_NOT_SET = -1;
		static const DWORD DEF_BOUNDINGBOX_COLOR;

		const JWWindow* m_pJWWindow;
		const WSTRING* m_pBaseDir;

		LPDIRECT3DDEVICE9 m_pDevice;

		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
		LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
		LPDIRECT3DTEXTURE9 m_pTexture;

		VECTOR<SVertexImage> m_Vertices;
		VECTOR<SIndex3> m_Indices;

		D3DXVECTOR2 m_Size;
		D3DXVECTOR2 m_ScaledSize;
		D3DXVECTOR2 m_VisibleRange;
		D3DXVECTOR2 m_AtlasSize;
		D3DXVECTOR2 m_OffsetInAtlas;

		D3DXVECTOR2 m_Position;
		D3DXVECTOR2 m_Scale;

		SBoundingBox m_BoundingBox;
		D3DXVECTOR2 m_BoundingBoxExtraSize;
		JWLine m_BoundingBoxLine;
		DWORD m_BoundingBoxColor;

		bool m_bUseStaticTexture;
	};
};