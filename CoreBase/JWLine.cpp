#include "JWLine.h"

using namespace JWENGINE;

auto JWLine::Create(const LPDIRECT3DDEVICE9 pDevice)->EError
{
	if (nullptr == (m_pDevice = pDevice))
		return EError::NULLPTR_DEVICE;

	m_pVB = nullptr;
	m_pIB = nullptr;

	return EError::OK;
}

auto JWLine::CreateMax(const LPDIRECT3DDEVICE9 pDevice)->EError
{
	Create(pDevice);

	CreateVertexBufferMax();
	CreateIndexBufferMax();

	return EError::OK;
}

PRIVATE void JWLine::CreateVertexBufferMax()
{
	if (m_pVB)
	{
		m_pVB->Release();
		m_pVB = nullptr;
	}

	int rVertSize = sizeof(SVertexLine) * MAX_UNIT_COUNT * 8;
	if (FAILED(m_pDevice->CreateVertexBuffer(rVertSize, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return;
}

PRIVATE void JWLine::CreateIndexBufferMax()
{
	if (m_pIB)
	{
		m_pIB->Release();
		m_pIB = nullptr;
	}

	int rIndSize = sizeof(SIndex2) * MAX_UNIT_COUNT * 4;
	if (FAILED(m_pDevice->CreateIndexBuffer(rIndSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return;
}

void JWLine::ClearBuffers()
{
	m_Vertices.clear();
	m_Indices.clear();
}

void JWLine::Destroy()
{
	m_pDevice = nullptr; // Just set to nullptr, becuase it's newed in <JWWindow> class

	m_Vertices.clear();
	m_Indices.clear();

	JW_RELEASE(m_pIB);
	JW_RELEASE(m_pVB);
}

void JWLine::AddLine(const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Length, const DWORD Color)
{
	m_Vertices.push_back(SVertexLine(StartPosition.x, StartPosition.y, Color));
	m_Vertices.push_back(SVertexLine(StartPosition.x + Length.x, StartPosition.y + Length.y, Color));

	int tIndicesCount = static_cast<int>(m_Indices.size());
	m_Indices.push_back(SIndex2(tIndicesCount * 2, tIndicesCount * 2 + 1));
}

void JWLine::AddBox(const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Size, const DWORD Color)
{
	AddLine(StartPosition, D3DXVECTOR2(Size.x, 0), Color); // Top
	AddLine(StartPosition, D3DXVECTOR2(0, Size.y), Color); // Left
	AddLine(D3DXVECTOR2(StartPosition.x + Size.x, StartPosition.y), D3DXVECTOR2(0, Size.y), Color); // Right
	AddLine(D3DXVECTOR2(StartPosition.x, StartPosition.y + Size.y), D3DXVECTOR2(Size.x, 0), Color); // Bottom
}

void JWLine::AddEnd()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	UpdateVertexBuffer();
	UpdateIndexBuffer();
}

PRIVATE void JWLine::CreateVertexBuffer()
{
	if (m_pVB)
	{
		m_pVB->Release();
		m_pVB = nullptr;
	}

	int rVertSize = sizeof(SVertexLine) * static_cast<int>(m_Vertices.size());
	if (FAILED(m_pDevice->CreateVertexBuffer(rVertSize, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return;
}

PRIVATE void JWLine::CreateIndexBuffer()
{
	if (m_pIB)
	{
		m_pIB->Release();
		m_pIB = nullptr;
	}

	int rIndSize = sizeof(SIndex2) * static_cast<int>(m_Indices.size());
	if (FAILED(m_pDevice->CreateIndexBuffer(rIndSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return;
}

void JWLine::UpdateVertexBuffer()
{
	if (m_Vertices.size() > 0)
	{
		int rVertSize = sizeof(SVertexLine) * static_cast<int>(m_Vertices.size());
		VOID* pVertices;
		if (FAILED(m_pVB->Lock(0, rVertSize, (void**)&pVertices, 0)))
			return;
		memcpy(pVertices, &m_Vertices[0], rVertSize);
		m_pVB->Unlock();
	}
}

void JWLine::UpdateIndexBuffer()
{
	if (m_Indices.size() > 0)
	{
		int rIndSize = sizeof(SIndex2) * static_cast<int>(m_Indices.size());
		VOID* pIndices;
		if (FAILED(m_pIB->Lock(0, rIndSize, (void **)&pIndices, 0)))
			return;
		memcpy(pIndices, &m_Indices[0], rIndSize);
		m_pIB->Unlock();
	}
}

void JWLine::Draw() const
{
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pDevice->SetTexture(0, nullptr);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(SVertexLine));
	m_pDevice->SetFVF(D3DFVF_LINE);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, static_cast<int>(m_Vertices.size()), 0, static_cast<int>(m_Indices.size()));
}

void JWLine::SetLine(const UINT LineIndex, const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Size)
{
	if (LineIndex * 2 <= static_cast<UINT>(m_Vertices.size()))
	{
		m_Vertices[LineIndex * 2].x = StartPosition.x;
		m_Vertices[LineIndex * 2].y = StartPosition.y;
		
		m_Vertices[LineIndex * 2 + 1].x = StartPosition.x + Size.x;
		m_Vertices[LineIndex * 2 + 1].y = StartPosition.y + Size.y;

		UpdateVertexBuffer();
	}
}

void JWLine::SetLineColor(const UINT LineIndex, const DWORD Color)
{
	if (LineIndex * 2 <= static_cast<UINT>(m_Vertices.size()))
	{
		m_Vertices[LineIndex * 2].color = Color;

		m_Vertices[LineIndex * 2 + 1].color = Color;

		UpdateVertexBuffer();
	}
}

void JWLine::SetLineColor(const UINT LineIndex, const DWORD ColorA, const DWORD ColorB)
{
	if (LineIndex * 2 <= static_cast<UINT>(m_Vertices.size()))
	{
		m_Vertices[LineIndex * 2].color = ColorA;

		m_Vertices[LineIndex * 2 + 1].color = ColorB;

		UpdateVertexBuffer();
	}
}

void JWLine::SetBox(const D3DXVECTOR2& StartPosition, const D3DXVECTOR2& Size)
{
	if (m_Vertices.size())
	{
		SetLine(0, StartPosition, D3DXVECTOR2(Size.x, 0));
		SetLine(1, StartPosition, D3DXVECTOR2(0, Size.y));
		SetLine(2, D3DXVECTOR2(StartPosition.x + Size.x, StartPosition.y), D3DXVECTOR2(0, Size.y));
		SetLine(3, D3DXVECTOR2(StartPosition.x, StartPosition.y + Size.y), D3DXVECTOR2(Size.x, 0));
	}
}

void JWLine::SetBoxColor(const DWORD Color)
{
	SetLineColor(0, Color);
	SetLineColor(1, Color);
	SetLineColor(2, Color);
	SetLineColor(3, Color);
}

void JWLine::SetBoxColor(const DWORD ColorA, const DWORD ColorB)
{
	SetLineColor(0, ColorA, ColorB);
	SetLineColor(1, ColorA, ColorB);
	SetLineColor(2, ColorB, ColorB);
	SetLineColor(3, ColorB, ColorB);
}

void JWLine::SetEntireAlpha(const BYTE Alpha)
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

void JWLine::SetEntireXRGB(const DWORD Color)
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