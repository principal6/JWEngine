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
		virtual ~JWRectangle() {};

		virtual auto Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const UINT MaxNumBox = 1)->EError;
		virtual void Destroy();

		virtual void ClearAllRectangles();
		virtual void AddRectangle(const D3DXVECTOR2 Size, const D3DXVECTOR2 Position);

		virtual void Draw();

		virtual void SetRectangleColor(const DWORD Color);
		
	protected:
		virtual void ClearVertexAndIndexData();

		virtual void CreateVertexBuffer();
		virtual void CreateIndexBuffer();
		virtual void UpdateVertexBuffer();
		virtual void UpdateIndexBuffer();

	protected:
		static const DWORD DEFAULT_COLOR_RECTANGLE = D3DCOLOR_ARGB(255, 80, 255, 0);

		const JWWindow* m_pJWWindow;
		const WSTRING* m_pBaseDir;
		
		LPDIRECT3DDEVICE9 m_pDevice;
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
		LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;

		SVertexImage* m_Vertices;
		SIndex3* m_Indices;

		UINT m_MaxNumBox;
		UINT m_BoxCount;

		DWORD m_RectangleColor;
	};
};