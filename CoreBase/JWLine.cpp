#include "JWLine.h"

using namespace JWENGINE;

// Static member variable declaration
LPDIRECT3DDEVICE9 JWLine::ms_pDevice;

auto JWLine::Create(LPDIRECT3DDEVICE9 pD3DDev)->EError
{
	if (nullptr == (ms_pDevice = pD3DDev))
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

void JWLine::CreateMax(LPDIRECT3DDEVICE9 pD3DDev)
{
	Create(pD3DDev);
	CreateVBMax();
	CreateIBMax();
}

void JWLine::Destroy()
{
	ms_pDevice = nullptr; // Just set to nullptr cuz it's newed in <JWWindow> class

	m_Vertices.clear();
	m_Indices.clear();

	DX_RELEASE(m_pIB);
	DX_RELEASE(m_pVB);
}

void JWLine::AddLine(D3DXVECTOR2 StartPos, D3DXVECTOR2 Length, DWORD Color)
{
	m_Vertices.push_back(SVertexLine(StartPos.x, StartPos.y, Color));
	m_Vertices.push_back(SVertexLine(StartPos.x + Length.x, StartPos.y + Length.y, Color));

	int tIndicesCount = static_cast<int>(m_Indices.size());
	m_Indices.push_back(SIndex2(tIndicesCount * 2, tIndicesCount * 2 + 1));
}

void JWLine::AddBox(D3DXVECTOR2 StartPos, D3DXVECTOR2 Size, DWORD Color)
{
	AddLine(StartPos, D3DXVECTOR2(Size.x, 0), Color);
	AddLine(StartPos, D3DXVECTOR2(0, Size.y), Color);
	AddLine(D3DXVECTOR2(StartPos.x + Size.x, StartPos.y), D3DXVECTOR2(0, Size.y), Color);
	AddLine(D3DXVECTOR2(StartPos.x, StartPos.y + Size.y), D3DXVECTOR2(Size.x, 0), Color);
}

void JWLine::AddEnd()
{
	CreateVB();
	CreateIB();
	UpdateVB();
	UpdateIB();
}

void JWLine::SetBoxPosition(D3DXVECTOR2 StartPos, D3DXVECTOR2 Size)
{
	m_Vertices[0].x = StartPos.x;
	m_Vertices[0].y = StartPos.y;
	m_Vertices[1].x = StartPos.x + Size.x;
	m_Vertices[1].y = StartPos.y;

	m_Vertices[2].x = StartPos.x;
	m_Vertices[2].y = StartPos.y;
	m_Vertices[3].x = StartPos.x;
	m_Vertices[3].y = StartPos.y + Size.y;

	m_Vertices[4].x = StartPos.x + Size.x;
	m_Vertices[4].y = StartPos.y;
	m_Vertices[5].x = StartPos.x + Size.x;
	m_Vertices[5].y = StartPos.y + Size.y;

	m_Vertices[6].x = StartPos.x;
	m_Vertices[6].y = StartPos.y + Size.y;
	m_Vertices[7].x = StartPos.x + Size.x;
	m_Vertices[7].y = StartPos.y + Size.y;
	
	UpdateVB();
}

void JWLine::CreateVB()
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

void JWLine::CreateIB()
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

void JWLine::CreateVBMax()
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

void JWLine::CreateIBMax()
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

void JWLine::UpdateVB()
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

void JWLine::UpdateIB()
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