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
		D3DXVECTOR2 PositionOffset{ 0, 0 };
		D3DXVECTOR2 Size{ 0, 0 };

		SBoundingBox() {};
		SBoundingBox(D3DXVECTOR2 _PositionOffset, D3DXVECTOR2 _Size) : PositionOffset(_PositionOffset), Size(_Size) {};
	};

	class JWImage
	{
	public:
		JWImage() {};
		virtual ~JWImage() {};

		virtual void Create(const JWWindow& Window, const WSTRING& BaseDir);
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
		virtual auto SetColor(DWORD Color) noexcept->JWImage*;
		virtual auto SetAlpha(BYTE Alpha) noexcept->JWImage*;
		virtual auto SetXRGB(DWORD XRGB) noexcept->JWImage*;
		virtual auto SetScale(const D3DXVECTOR2& Scale) noexcept->JWImage*;
		virtual auto SetVisibleRange(const D3DXVECTOR2& Range) noexcept->JWImage*;
		virtual auto SetAtlasUV(const D3DXVECTOR2& OffsetInAtlas, const D3DXVECTOR2& Size, bool bSetSize = true) noexcept->JWImage*;
		virtual auto SetUVRange(const STextureUV& UV) noexcept->JWImage*;
		virtual auto SetBoundingBox(const D3DXVECTOR2& ExtraSize) noexcept->JWImage*;
		virtual auto SetBoundingBoxAlpha(BYTE Alpha) noexcept->JWImage*;
		virtual auto SetBoundingBoxXRGB(DWORD XRGB) noexcept->JWImage*;

		virtual auto GetSize() const noexcept->const D3DXVECTOR2&;
		virtual auto GetScaledSize() const noexcept->const D3DXVECTOR2&;
		virtual auto GetPosition() const noexcept->const D3DXVECTOR2&;
		virtual auto GetCenterPosition() const noexcept->const D3DXVECTOR2;
		virtual auto GetBoundingBox() const noexcept->const SBoundingBox;

		virtual auto IsTextureLoaded() const noexcept->bool;

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
		static const DWORD DEFAULT_BOUNDINGBOX_COLOR;

		const JWWindow* m_pJWWindow = nullptr;
		const WSTRING* m_pBaseDir = nullptr;

		LPDIRECT3DDEVICE9 m_pDevice = nullptr;

		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer = nullptr;
		LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer = nullptr;
		LPDIRECT3DTEXTURE9 m_pTexture = nullptr;

		VECTOR<SVertexImage> m_Vertices;
		VECTOR<SIndex3> m_Indices;

		D3DXVECTOR2 m_Size{ 0, 0 };
		D3DXVECTOR2 m_ScaledSize{ 0, 0 };
		D3DXVECTOR2 m_VisibleRange{ VISIBLE_RANGE_NOT_SET, VISIBLE_RANGE_NOT_SET };
		D3DXVECTOR2 m_AtlasSize{ 0, 0 };
		D3DXVECTOR2 m_OffsetInAtlas{ 0, 0 };

		D3DXVECTOR2 m_Position{ 0, 0 };
		D3DXVECTOR2 m_Scale{ 1.0f, 1.0f };

		SBoundingBox m_BoundingBox;
		D3DXVECTOR2 m_BoundingBoxExtraSize{ 0, 0 };
		JWLine m_BoundingBoxLine;
		DWORD m_BoundingBoxColor = DEFAULT_BOUNDINGBOX_COLOR;

		bool m_bUseStaticTexture = false;
	};
};