#include "JWLine.h"

using namespace JWENGINE;

// Static member variable declaration
LPDIRECT3DDEVICE9 JWLine::ms_pDevice;

auto JWLine::Create(LPDIRECT3DDEVICE9 pDevice)->EError
{
	if (nullptr == (ms_pDevice = pDevice))
		return EError::NULLPTR_DEVICE;

	m_pVB = nullptr;
	m_pIB = nullptr;

	Clear();

	return EError::OK;
}

void JWLine::Clear()
{
	m_Vertices.clear();
	m_Indices.clear();
}

void JWLine::CreateMax(LPDIRECT3DDEVICE9 pDevice)
{
	Create(pDevice);

	CreateVertexBufferMax();
	CreateIndexBufferMax();
}

void JWLine::Destroy()
{
	ms_pDevice = nullptr; // Just set to nullptr, becuase it's newed in <JWWindow> class

	m_Vertices.clear();
	m_Indices.clear();

	JW_RELEASE(m_pIB);
	JW_RELEASE(m_pVB);
}

void JWLine::AddLine(D3DXVECTOR2 StartPosition, D3DXVECTOR2 Length, DWORD Color)
{
	m_Vertices.push_back(SVertexLine(StartPosition.x, StartPosition.y, Color));
	m_Vertices.push_back(SVertexLine(StartPosition.x + Length.x, StartPosition.y + Length.y, Color));

	int tIndicesCount = static_cast<int>(m_Indices.size());
	m_Indices.push_back(SIndex2(tIndicesCount * 2, tIndicesCount * 2 + 1));
}

void JWLine::AddBox(D3DXVECTOR2 StartPosition, D3DXVECTOR2 Size, DWORD Color)
{
	AddLine(StartPosition, D3DXVECTOR2(Size.x, 0), Color);
	AddLine(StartPosition, D3DXVECTOR2(0, Size.y), Color);
	AddLine(D3DXVECTOR2(StartPosition.x + Size.x, StartPosition.y), D3DXVECTOR2(0, Size.y), Color);
	AddLine(D3DXVECTOR2(StartPosition.x, StartPosition.y + Size.y), D3DXVECTOR2(Size.x, 0), Color);
}

void JWLine::AddEnd()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	UpdateVertexBuffer();
	UpdateIndexBuffer();
}

void JWLine::SetLine(UINT LineIndex, D3DXVECTOR2 StartPosition, D3DXVECTOR2 Size)
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

void JWLine::SetLineColor(UINT LineIndex, DWORD Color)
{
	if (LineIndex * 2 <= static_cast<UINT>(m_Vertices.size()))
	{
		m_Vertices[LineIndex * 2].color = Color;

		m_Vertices[LineIndex * 2 + 1].color = Color;

		UpdateVertexBuffer();
	}
}

void JWLine::SetLineColor(UINT LineIndex, DWORD ColorA, DWORD ColorB)
{
	if (LineIndex * 2 <= static_cast<UINT>(m_Vertices.size()))
	{
		m_Vertices[LineIndex * 2].color = ColorA;

		m_Vertices[LineIndex * 2 + 1].color = ColorB;

		UpdateVertexBuffer();
	}
}

void JWLine::SetBox(D3DXVECTOR2 StartPosition, D3DXVECTOR2 Size)
{
	if (m_Vertices.size())
	{
		SetLine(0, StartPosition, D3DXVECTOR2(Size.x, 0));
		SetLine(1, StartPosition, D3DXVECTOR2(0, Size.y));
		SetLine(2, D3DXVECTOR2(StartPosition.x + Size.x, StartPosition.y), D3DXVECTOR2(0, Size.y));
		SetLine(3, D3DXVECTOR2(StartPosition.x, StartPosition.y + Size.y), D3DXVECTOR2(Size.x, 0));
	}
}

void JWLine::SetAlpha(BYTE Alpha)
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

void JWLine::SetXRGB(DWORD Color)
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

void JWLine::CreateVertexBuffer()
{
	if (m_pVB)
	{
		m_pVB->Release();
		m_pVB = nullptr;
	}
	int rVertSize = sizeof(SVertexLine) * static_cast<int>(m_Vertices.size());
	if (FAILED(ms_pDevice->CreateVertexBuffer(rVertSize, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return;
}

void JWLine::CreateIndexBuffer()
{
	if (m_pIB)
	{
		m_pIB->Release();
		m_pIB = nullptr;
	}
	int rIndSize = sizeof(SIndex2) * static_cast<int>(m_Indices.size());
	if (FAILED(ms_pDevice->CreateIndexBuffer(rIndSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return;
}

void JWLine::CreateVertexBufferMax()
{
	if (m_pVB)
	{
		m_pVB->Release();
		m_pVB = nullptr;
	}
	int rVertSize = sizeof(SVertexLine) * MAX_UNIT_COUNT * 8;
	if (FAILED(ms_pDevice->CreateVertexBuffer(rVertSize, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return;
}

void JWLine::CreateIndexBufferMax()
{
	if (m_pIB)
	{
		m_pIB->Release();
		m_pIB = nullptr;
	}
	int rIndSize = sizeof(SIndex2) * MAX_UNIT_COUNT * 4;
	if (FAILED(ms_pDevice->CreateIndexBuffer(rIndSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
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
	ms_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	ms_pDevice->SetTexture(0, nullptr);

	ms_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(SVertexLine));
	ms_pDevice->SetFVF(D3DFVF_LINE);
	ms_pDevice->SetIndices(m_pIB);
	ms_pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, static_cast<int>(m_Vertices.size()), 0, static_cast<int>(m_Indices.size()));
}