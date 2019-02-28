#pragma once

#include "JWCommon.h"

namespace JWENGINE
{
	struct SVertexLine
	{
		FLOAT x, y, z, rhw;
		DWORD color;

		SVertexLine() : x(0), y(0), z(0), rhw(1), color(0xffffffff) {};
		SVertexLine(float _x, float _y, DWORD _color) : x(_x), y(_y), z(0), rhw(1), color(_color) {};
		SVertexLine(float _x, float _y, float _z, float _rhw, DWORD _color) :
			x(_x), y(_y), z(_z), rhw(_rhw), color(_color) {};
	};

	struct SIndex2
	{
		WORD _0, _1;

		SIndex2() : _0(0), _1(0) {};
		SIndex2(int ID0, int ID1) : _0(ID0), _1(ID1) {};
	};

	class JWLine final
	{
	public:
		JWLine();
		~JWLine() {};

		void Create(const LPDIRECT3DDEVICE9 pDevice);
		void CreateMax(const LPDIRECT3DDEVICE9 pDevice) noexcept;
		void Destroy() noexcept;

		// @warning: We must call AddEnd() after finishing adding lines.
		void AddLine(const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Length, DWORD Color) noexcept;

		// @warning: We must call AddEnd() after finishing adding boxes.
		void AddBox(const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Size, DWORD Color) noexcept;

		// This function must be called after adding all the lines and boxes.
		void AddEnd() noexcept;

		void ClearVertexAndIndex() noexcept;

		void Draw() const noexcept;

		void SetLine(UINT LineIndex, const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Size) noexcept;
		void SetLineColor(UINT LineIndex, DWORD Color) noexcept;
		void SetLineColor(UINT LineIndex, DWORD ColorA, DWORD ColorB) noexcept;
		void SetBox(const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Size) noexcept;
		void SetBoxColor(DWORD Color) noexcept;
		void SetBoxColor(DWORD ColorA, DWORD ColorB) noexcept;
		void SetEntireAlpha(BYTE Alpha) noexcept;
		void SetEntireXRGB(DWORD Color) noexcept;

	private:
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void UpdateVertexBuffer();
		void UpdateIndexBuffer();

		void CreateVertexBufferMax();
		void CreateIndexBufferMax();

	private:
		LPDIRECT3DDEVICE9 m_pDevice;

		LPDIRECT3DVERTEXBUFFER9 m_pVB;
		LPDIRECT3DINDEXBUFFER9 m_pIB;

		VECTOR<SVertexLine> m_Vertices;
		VECTOR<SIndex2> m_Indices;
	};
};