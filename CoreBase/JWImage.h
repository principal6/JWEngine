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

		SBoundingBox() : PositionOffset(0, 0), Size(0, 0) {};
		SBoundingBox(D3DXVECTOR2 _PositionOffset, D3DXVECTOR2 _Size) : PositionOffset(_PositionOffset), Size(_Size) {};
	};

	class JWImage
	{
	public:
		JWImage();
		virtual ~JWImage() {};

		virtual void Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir);
		virtual void Destroy() noexcept;

		virtual void Draw() noexcept;
		virtual void DrawBoundingBox() noexcept;

		virtual void FlipHorizontal() noexcept;
		virtual void FlipVertical() noexcept;

		virtual void SetPosition(const D3DXVECTOR2& Position) noexcept;
		virtual void SetPositionCentered(const D3DXVECTOR2& Position) noexcept;
		virtual void SetSize(const D3DXVECTOR2& Size) noexcept;
		virtual void SetTexture(const WSTRING& FileName);
		virtual void SetTexture(const LPDIRECT3DTEXTURE9 pTexture, const D3DXIMAGE_INFO* pInfo) noexcept;
		virtual auto SetColor(const DWORD Color) noexcept->JWImage*;
		virtual auto SetAlpha(const BYTE Alpha) noexcept->JWImage*;
		virtual auto SetXRGB(const DWORD XRGB) noexcept->JWImage*;
		virtual auto SetScale(const D3DXVECTOR2& Scale) noexcept->JWImage*;
		virtual auto SetVisibleRange(const D3DXVECTOR2& Range) noexcept->JWImage*;
		virtual auto SetAtlasUV(const D3DXVECTOR2& OffsetInAtlas, const D3DXVECTOR2& Size, bool bSetSize = true) noexcept->JWImage*;
		virtual auto SetUVRange(const STextureUV UV) noexcept->JWImage*;
		virtual auto SetBoundingBox(const D3DXVECTOR2& ExtraSize) noexcept->JWImage*;
		virtual auto SetBoundingBoxAlpha(const BYTE Alpha) noexcept->JWImage*;
		virtual auto SetBoundingBoxXRGB(const DWORD XRGB) noexcept->JWImage*;

		virtual auto GetSize() const noexcept->const D3DXVECTOR2&;
		virtual auto GetScaledSize() const noexcept->const D3DXVECTOR2&;
		virtual auto GetPosition() const noexcept->const D3DXVECTOR2&;
		virtual auto GetCenterPosition() const noexcept->const D3DXVECTOR2;
		virtual auto GetBoundingBox() const noexcept->const SBoundingBox;

		virtual auto IsTextureLoaded() const noexcept->const bool;

	protected:
		virtual void ClearVertexAndIndex() noexcept;

		virtual void CreateVertexBuffer();
		virtual void CreateIndexBuffer();
		virtual void UpdateVertexBuffer();
		virtual void UpdateIndexBuffer();

		virtual void UpdateVertexData() noexcept;
		virtual void UpdateVertexData(const STextureUV& UV) noexcept;

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