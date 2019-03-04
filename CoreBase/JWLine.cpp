#include "JWLine.h"

using namespace JWENGINE;

void JWLine::Create(const LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice = pDevice;
}

void JWLine::CreateMax(const LPDIRECT3DDEVICE9 pDevice) noexcept
{
	Create(pDevice);

	CreateVertexBufferMax();
	CreateIndexBufferMax();
}

PRIVATE void JWLine::CreateVertexBufferMax()
{
	JW_RELEASE(m_pVB);

	int vertex_size = sizeof(SVertexLine) * MAX_UNIT_COUNT * 8;
	if (FAILED(m_pDevice->CreateVertexBuffer(vertex_size, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVB, nullptr)))
	{
		THROW_CREATION_FAILED;
	}
}

PRIVATE void JWLine::CreateIndexBufferMax()
{
	JW_RELEASE(m_pIB);

	int index_size = sizeof(SIndex2) * MAX_UNIT_COUNT * 4;
	if (FAILED(m_pDevice->CreateIndexBuffer(index_size, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
	{
		THROW_CREATION_FAILED;
	}
}

void JWLine::Destroy() noexcept
{
	m_pDevice = nullptr; // Just set to nullptr, becuase it's newed in <JWWindow> class

	JW_RELEASE(m_pIB);
	JW_RELEASE(m_pVB);
}

void JWLine::AddLine(const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Length, DWORD Color) noexcept
{
	m_Vertices.push_back(SVertexLine(StartPosition.x, StartPosition.y, Color));
	m_Vertices.push_back(SVertexLine(StartPosition.x + Length.x, StartPosition.y + Length.y, Color));

	int tIndicesCount = static_cast<int>(m_Indices.size());
	m_Indices.push_back(SIndex2(tIndicesCount * 2, tIndicesCount * 2 + 1));
}

void JWLine::AddBox(const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Size, DWORD Color) noexcept
{
	AddLine(StartPosition, D3DXVECTOR2(Size.x, 0), Color); // Top
	AddLine(StartPosition, D3DXVECTOR2(0, Size.y), Color); // Left
	AddLine(D3DXVECTOR2(StartPosition.x + Size.x, StartPosition.y), D3DXVECTOR2(0, Size.y), Color); // Right
	AddLine(D3DXVECTOR2(StartPosition.x, StartPosition.y + Size.y), D3DXVECTOR2(Size.x, 0), Color); // Bottom
}

void JWLine::AddEnd() noexcept
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	UpdateVertexBuffer();
	UpdateIndexBuffer();
}

PRIVATE void JWLine::CreateVertexBuffer()
{
	if (!m_pVB)
	{
		int vertex_size = sizeof(SVertexLine) * static_cast<int>(m_Vertices.size());
		if (FAILED(m_pDevice->CreateVertexBuffer(vertex_size, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		{
			THROW_CREATION_FAILED;
		}
	}
}

PRIVATE void JWLine::CreateIndexBuffer()
{
	if (!m_pIB)
	{
		int index_size = sizeof(SIndex2) * static_cast<int>(m_Indices.size());
		if (FAILED(m_pDevice->CreateIndexBuffer(index_size, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		{
			THROW_CREATION_FAILED;
		}
	}
}

void JWLine::ClearVertexAndIndex() noexcept
{
	m_Vertices.clear();
	m_Indices.clear();
}

PRIVATE void JWLine::UpdateVertexBuffer()
{
	if (m_Vertices.size())
	{
		int vertex_size = sizeof(SVertexLine) * static_cast<int>(m_Vertices.size());
		void* pVertices;
		if (SUCCEEDED(m_pVB->Lock(0, vertex_size, (void**)&pVertices, 0)))
		{
			memcpy(pVertices, &m_Vertices[0], vertex_size);
			m_pVB->Unlock();
		}
	}
}

PRIVATE void JWLine::UpdateIndexBuffer()
{
	if (m_Indices.size())
	{
		int index_size = sizeof(SIndex2) * static_cast<int>(m_Indices.size());
		void* pIndices;
		if (SUCCEEDED(m_pIB->Lock(0, index_size, (void **)&pIndices, 0)))
		{
			memcpy(pIndices, &m_Indices[0], index_size);
			m_pIB->Unlock();
		}
	}
}

void JWLine::Draw() const noexcept
{
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pDevice->SetTexture(0, nullptr);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(SVertexLine));
	m_pDevice->SetFVF(D3DFVF_LINE);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, static_cast<int>(m_Vertices.size()), 0, static_cast<int>(m_Indices.size()));
}

void JWLine::SetLine(size_t LineIndex, const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Size) noexcept
{
	if (LineIndex * 2 <= m_Vertices.size())
	{
		m_Vertices[LineIndex * 2].x = StartPosition.x;
		m_Vertices[LineIndex * 2].y = StartPosition.y;
		
		m_Vertices[LineIndex * 2 + 1].x = StartPosition.x + Size.x;
		m_Vertices[LineIndex * 2 + 1].y = StartPosition.y + Size.y;

		UpdateVertexBuffer();
	}
}

void JWLine::SetLineColor(size_t LineIndex, DWORD Color) noexcept
{
	if (LineIndex * 2 <= m_Vertices.size())
	{
		m_Vertices[LineIndex * 2].color = Color;

		m_Vertices[LineIndex * 2 + 1].color = Color;

		UpdateVertexBuffer();
	}
}

void JWLine::SetLineColor(size_t LineIndex, DWORD ColorA, DWORD ColorB) noexcept
{
	if (LineIndex * 2 <= m_Vertices.size())
	{
		m_Vertices[LineIndex * 2].color = ColorA;

		m_Vertices[LineIndex * 2 + 1].color = ColorB;

		UpdateVertexBuffer();
	}
}

void JWLine::SetBox(const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Size) noexcept
{
	if (m_Vertices.size())
	{
		SetLine(0, StartPosition, D3DXVECTOR2(Size.x, 0));
		SetLine(1, StartPosition, D3DXVECTOR2(0, Size.y));
		SetLine(2, D3DXVECTOR2(StartPosition.x + Size.x, StartPosition.y), D3DXVECTOR2(0, Size.y));
		SetLine(3, D3DXVECTOR2(StartPosition.x, StartPosition.y + Size.y), D3DXVECTOR2(Size.x, 0));
	}
}

void JWLine::SetBoxColor(DWORD Color) noexcept
{
	SetLineColor(0, Color);
	SetLineColor(1, Color);
	SetLineColor(2, Color);
	SetLineColor(3, Color);
}

void JWLine::SetBoxColor(DWORD ColorA, DWORD ColorB) noexcept
{
	SetLineColor(0, ColorA, ColorB);
	SetLineColor(1, ColorA, ColorB);
	SetLineColor(2, ColorB, ColorB);
	SetLineColor(3, ColorB, ColorB);
}

void JWLine::SetEntireAlpha(BYTE Alpha) noexcept
{
	if (m_Vertices.size())
	{
		for (SVertexLine& iterator : m_Vertices)
		{
			SetColorAlpha(&iterator.color, Alpha);
		}
		UpdateVertexBuffer();
	}
}

void JWLine::SetEntireXRGB(DWORD Color) noexcept
{
	if (m_Vertices.size())
	{
		for (SVertexLine& iterator : m_Vertices)
		{
			SetColorXRGB(&iterator.color, Color);
		}
		UpdateVertexBuffer();
	}
}