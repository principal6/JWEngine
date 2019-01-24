#pragma once

#include "JWCommon.h"
#include "JWLine.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	// ***

	struct SVertexImage
	{
		FLOAT x, y, z, rhw;
		DWORD color;
		FLOAT u, v;

		SVertexImage() :
			x(0), y(0), z(0), rhw(1), color(0xFFFFFFFF), u(0), v(0) {};
		SVertexImage(float _x, float _y, float _u, float _v) :
			x(_x), y(_y), z(0), rhw(1), color(0xFFFFFFFF), u(_u), v(_v) {};
		SVertexImage(float _x, float _y, DWORD _color) :
			x(_x), y(_y), z(0), rhw(1), color(_color), u(0), v(0) {};
		SVertexImage(float _x, float _y, DWORD _color, float _u, float _v) :
			x(_x), y(_y), z(0), rhw(1), color(_color), u(_u), v(_v) {};
		SVertexImage(float _x, float _y, float _z, float _rhw, DWORD _color, float _u, float _v) :
			x(_x), y(_y), z(_z), rhw(_rhw), color(_color), u(_u), v(_v) {};
	};

	struct SIndex3
	{
		WORD _0, _1, _2;

		SIndex3() :
			_0(0), _1(0), _2(0) {};
		SIndex3(int ID0, int ID1, int ID2) :
			_0(ID0), _1(ID1), _2(ID2) {};
	};

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

		virtual auto JWImage::Create(JWWindow* pJWWindow, WSTRING BaseDir)->EError;
		virtual void JWImage::Destroy();

		virtual void JWImage::Draw();
		virtual void JWImage::DrawBoundingBox();

		virtual void JWImage::FlipHorizontal();
		virtual void JWImage::FlipVertical();

		// @warning: SetSize() is used publicly only in Map Editor
		virtual void JWImage::SetSize(D3DXVECTOR2 Size);
		
		virtual void JWImage::SetPosition(D3DXVECTOR2 Position);
		virtual void JWImage::SetPositionCentered(D3DXVECTOR2 Position);
		virtual void JWImage::SetTexture(WSTRING FileName);
		virtual auto JWImage::SetAlpha(BYTE Alpha)->JWImage*;
		virtual auto JWImage::SetXRGB(DWORD Color)->JWImage*;;
		virtual auto JWImage::SetScale(D3DXVECTOR2 Scale)->JWImage*;
		virtual auto JWImage::SetVisibleRange(D3DXVECTOR2 Range)->JWImage*;
		virtual auto JWImage::SetAtlasUV(D3DXVECTOR2 OffsetInAtlas, D3DXVECTOR2 Size,
			bool bSetSize = true)->JWImage*;
		virtual auto JWImage::SetUVRange(float u1, float u2, float v1, float v2)->JWImage*;
		virtual auto JWImage::SetBoundingBox(D3DXVECTOR2 ExtraSize)->JWImage*;
		virtual auto JWImage::SetBoundingBoxColor(DWORD Color)->JWImage*;

		virtual auto JWImage::GetSize() const->D3DXVECTOR2;
		virtual auto JWImage::GetScaledSize() const->D3DXVECTOR2;;
		virtual auto JWImage::GetPosition() const->D3DXVECTOR2;
		virtual auto JWImage::GetCenterPosition() const->D3DXVECTOR2;
		virtual auto JWImage::GetBoundingBox() const->SBoundingBox;

		virtual auto JWImage::IsTextureLoaded() const->bool;

	protected:
		virtual void JWImage::ClearVertexAndIndexData();
		virtual void JWImage::CreateVertexBuffer();
		virtual void JWImage::CreateIndexBuffer();
		virtual void JWImage::UpdateVertexBuffer();
		virtual void JWImage::UpdateIndexBuffer();
		virtual void JWImage::UpdateVertexData();
		virtual void JWImage::UpdateVertexData(float u1, float v1, float u2, float v2);

	protected:
		static const int VISIBLE_RANGE_NOT_SET = -1;
		static const DWORD DEF_BOUNDINGBOX_COLOR;

		JWWindow* m_pJWWindow;
		WSTRING m_BaseDir;

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
	};
};