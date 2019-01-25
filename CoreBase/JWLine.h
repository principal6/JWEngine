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
		JWLine() {};
		~JWLine() {};

		auto Create(LPDIRECT3DDEVICE9 pD3DDev)->EError;
		void CreateMax(LPDIRECT3DDEVICE9 pD3DDev);
		void Clear();
		void Destroy();

		void AddLine(D3DXVECTOR2 StartPos, D3DXVECTOR2 Length, DWORD Color);
		void AddBox(D3DXVECTOR2 StartPos, D3DXVECTOR2 Size, DWORD Color);
		void AddEnd();

		void SetBoxPosition(D3DXVECTOR2 StartPos, D3DXVECTOR2 Size);

		void UpdateVB();
		void UpdateIB();
		void Draw() const;

	private:
		void CreateVB();
		void CreateIB();
		void CreateVBMax();
		void CreateIBMax();

	private:
		static LPDIRECT3DDEVICE9 ms_pDevice;
		LPDIRECT3DVERTEXBUFFER9 m_pVB;
		LPDIRECT3DINDEXBUFFER9 m_pIB;

		VECTOR<SVertexLine> m_Vertices;
		VECTOR<SIndex2> m_Indices;
	};
};