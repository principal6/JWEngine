#include "JWRectangle.h"
#include "JWWindow.h"

using namespace JWENGINE;

JWRectangle::~JWRectangle()
{
	m_pJWWindow = nullptr;
	m_pDevice = nullptr; // Just set to nullptr cuz it's newed in <JWWindow> class

	JW_RELEASE(m_pIndexBuffer);
	JW_RELEASE(m_pVertexBuffer);
}

void JWRectangle::Create(const JWWindow& Window, const WSTRING& BaseDir, UINT MaxNumBox) noexcept
{
	m_pJWWindow = &Window;
	m_pDevice = Window.GetDevice();
	m_BaseDir = BaseDir;

	// MaxNumBox must be >= 1
	m_MaxNumBox = MaxNumBox;
	m_MaxNumBox = max(m_MaxNumBox, 1);

	CreateVertexBuffer();
	CreateIndexBuffer();
	UpdateVertexBuffer();
	UpdateIndexBuffer();
}

PRIVATE void JWRectangle::CreateVertexBuffer() noexcept
{
	m_Vertices.clear();
	for (size_t i = 0; i < m_MaxNumBox * 4; i++)
	{
		m_Vertices.push_back(SVertexImage(0, 0, m_RectangleColor));
	}

	int vertex_size = sizeof(SVertexImage) * m_MaxNumBox * 4;
	if (FAILED(m_pDevice->CreateVertexBuffer(vertex_size, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVertexBuffer, nullptr)))
	{
		assert(m_pVertexBuffer);
	}
}

PRIVATE void JWRectangle::CreateIndexBuffer() noexcept
{
	m_Indices.clear();
	for (size_t iterator = 0; iterator < m_MaxNumBox; iterator++)
	{
		m_Indices.push_back(SIndex3(iterator * 4, iterator * 4 + 1, iterator * 4 + 3));
		m_Indices.push_back(SIndex3(iterator * 4, iterator * 4 + 3, iterator * 4 + 2));
	}

	int index_size = sizeof(SIndex3) * m_MaxNumBox * 2;
	if (FAILED(m_pDevice->CreateIndexBuffer(index_size, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, nullptr)))
	{
		assert(m_pIndexBuffer);
	}
}

PRIVATE void JWRectangle::UpdateVertexBuffer() noexcept
{
	if (m_Vertices.size())
	{
		int vertex_size = sizeof(SVertexImage) * m_MaxNumBox * 4;
		void* pVertices;
		if (SUCCEEDED(m_pVertexBuffer->Lock(0, vertex_size, (void**)&pVertices, 0)))
		{
			memcpy(pVertices, &m_Vertices[0], vertex_size);
			m_pVertexBuffer->Unlock();
		}
	}
}

PRIVATE void JWRectangle::UpdateIndexBuffer() noexcept
{
	if (m_Indices.size())
	{
		int index_size = sizeof(SIndex3) * m_MaxNumBox * 2;
		void* pIndices;
		if (SUCCEEDED(m_pIndexBuffer->Lock(0, index_size, (void **)&pIndices, 0)))
		{
			memcpy(pIndices, &m_Indices[0], index_size);
			m_pIndexBuffer->Unlock();
		}
	}
}

void JWRectangle::ClearAllRectangles() noexcept
{
	m_BoxCount = 0;

	for (size_t i = 0; i < m_MaxNumBox * 4; i++)
	{
		m_Vertices[i] = SVertexImage(0, 0, m_RectangleColor);
	}

	UpdateVertexBuffer();
}

void JWRectangle::AddRectangle(const D3DXVECTOR2& Size, const D3DXVECTOR2& Position) noexcept
{
	// If box count is max, no adding
	if (m_BoxCount == m_MaxNumBox)
		return;

	m_Vertices[m_BoxCount * 4] = SVertexImage(Position.x, Position.y, m_RectangleColor);
	m_Vertices[m_BoxCount * 4 + 1] = SVertexImage(Position.x + Size.x, Position.y, m_RectangleColor);
	m_Vertices[m_BoxCount * 4 + 2] = SVertexImage(Position.x, Position.y + Size.y, m_RectangleColor);
	m_Vertices[m_BoxCount * 4 + 3] = SVertexImage(Position.x + Size.x, Position.y + Size.y, m_RectangleColor);

	m_BoxCount++;

	UpdateVertexBuffer();
}

void JWRectangle::Draw() const noexcept
{
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pDevice->SetTexture(0, nullptr);

	m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SVertexImage));
	m_pDevice->SetFVF(D3DFVF_TEXTURE);
	m_pDevice->SetIndices(m_pIndexBuffer);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_MaxNumBox * 4, 0, m_MaxNumBox * 2);
}

void JWRectangle::SetRectangleColor(DWORD Color) noexcept
{
	m_RectangleColor = Color;
}