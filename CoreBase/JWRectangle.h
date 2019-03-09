#pragma once

#include "JWCommon.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	// ***

	class JWRectangle
	{
	public:
		JWRectangle();
		virtual ~JWRectangle();

		virtual void Create(const JWWindow& Window, const WSTRING& BaseDir, UINT MaxNumBox = 1) noexcept;

		virtual void ClearAllRectangles() noexcept;

		virtual void AddRectangle(const D3DXVECTOR2& Size, const D3DXVECTOR2& Position) noexcept;

		virtual void Draw() const noexcept;

		virtual void SetRectangleColor(DWORD Color) noexcept;
		
	protected:
		virtual void DeleteVertexAndIndex() noexcept;

		virtual void CreateVertexBuffer() noexcept;
		virtual void CreateIndexBuffer() noexcept;
		virtual void UpdateVertexBuffer() noexcept;
		virtual void UpdateIndexBuffer() noexcept;

	protected:
		static constexpr DWORD DEFAULT_COLOR_RECTANGLE{ D3DCOLOR_ARGB(255, 80, 255, 0) };

		const JWWindow* m_pJWWindow{ nullptr };
		WSTRING m_BaseDir;
		
		LPDIRECT3DDEVICE9 m_pDevice{ nullptr };
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer{ nullptr };
		LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer{ nullptr };

		SVertexImage* m_Vertices{ nullptr };
		SIndex3* m_Indices{ nullptr };

		UINT m_MaxNumBox{};
		UINT m_BoxCount{};

		DWORD m_RectangleColor{ DEFAULT_COLOR_RECTANGLE };
	};
};