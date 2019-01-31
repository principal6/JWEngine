#include "JWRectangle.h"
#include "JWWindow.h"

using namespace JWENGINE;

JWRectangle::JWRectangle()
{
	m_pJWWindow = nullptr;
	m_pDevice = nullptr;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;

	m_Vertices = nullptr;
	m_Indices = nullptr;

	m_MaxNumBox = 0;
	m_BoxCount = 0;
	m_RectangleColor = DEFAULT_COLOR_RECTANGLE;

	ClearVertexAndIndexData();
}

auto JWRectangle::Create(JWWindow* pJWWindow, WSTRING BaseDir, UINT MaxNumBox)->EError
{
	if (pJWWindow == nullptr)
		return EError::NULLPTR_WINDOW;

	m_pJWWindow = pJWWindow;
	m_pDevice = pJWWindow->GetDevice();
	m_BaseDir = BaseDir;

	// MaxNumBox >= 1
	MaxNumBox = max(MaxNumBox, 1);
	m_MaxNumBox = MaxNumBox;

	ClearVertexAndIndexData();

	CreateVertexBuffer();
	CreateIndexBuffer();
	UpdateVertexBuffer();
	UpdateIndexBuffer();

	return EError::OK;
}

void JWRectangle::Destroy()
{
	m_pJWWindow = nullptr;
	m_pDevice = nullptr; // Just set to nullptr cuz it's newed in <JWWindow> class

	ClearVertexAndIndexData();

	JW_RELEASE(m_pIndexBuffer);
	JW_RELEASE(m_pVertexBuffer);
}

PRIVATE void JWRectangle::ClearVertexAndIndexData()
{
	if (m_Vertices)
	{
		delete[] m_Vertices;
		m_Vertices = nullptr;
	}

	if (m_Indices)
	{
		delete[] m_Indices;
		m_Indices = nullptr;
	}
}

PRIVATE void JWRectangle::CreateVertexBuffer()
{
	if (!m_Vertices)
	{
		m_Vertices = new SVertexImage[m_MaxNumBox * 4];

		for (UINT i = 0; i < m_MaxNumBox * 4; i++)
		{
			m_Vertices[i] = SVertexImage(0, 0, m_RectangleColor);
		}
	}

	int rVertSize = sizeof(SVertexImage) * m_MaxNumBox * 4;
	if (FAILED(m_pDevice->CreateVertexBuffer(rVertSize, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVertexBuffer, nullptr)))
	{
		return;
	}
}

PRIVATE void JWRectangle::CreateIndexBuffer()
{
	if (!m_Indices)
	{
		m_Indices = new SIndex3[m_MaxNumBox * 2];

		for (size_t iterator = 0; iterator < m_MaxNumBox; iterator++)
		{
			m_Indices[iterator * 2] = SIndex3(iterator * 4, iterator * 4 + 1, iterator * 4 + 3);
			m_Indices[iterator * 2 + 1] = SIndex3(iterator * 4, iterator * 4 + 3, iterator * 4 + 2);
		}
	}

	int rIndSize = sizeof(SIndex3) * m_MaxNumBox * 2;
	if (FAILED(m_pDevice->CreateIndexBuffer(rIndSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, nullptr)))
	{
		return;
	}
}

PRIVATE void JWRectangle::UpdateVertexBuffer()
{
	if (m_Vertices)
	{
		int rVertSize = sizeof(SVertexImage) * m_MaxNumBox * 4;
		VOID* pVertices;
		if (FAILED(m_pVertexBuffer->Lock(0, rVertSize, (void**)&pVertices, 0)))
		{
			return;
		}
		memcpy(pVertices, &m_Vertices[0], rVertSize);
		m_pVertexBuffer->Unlock();
	}
}

PRIVATE void JWRectangle::UpdateIndexBuffer()
{
	if (m_Indices)
	{
		int rIndSize = sizeof(SIndex3) * m_MaxNumBox * 2;
		VOID* pIndices;
		if (FAILED(m_pIndexBuffer->Lock(0, rIndSize, (void **)&pIndices, 0)))
		{
			return;
		}
		memcpy(pIndices, &m_Indices[0], rIndSize);
		m_pIndexBuffer->Unlock();
	}
}

void JWRectangle::ClearAllRectangles()
{
	m_BoxCount = 0;

	for (UINT i = 0; i < m_MaxNumBox * 4; i++)
	{
		m_Vertices[i] = SVertexImage(0, 0, m_RectangleColor);
	}

	UpdateVertexBuffer();
}

void JWRectangle::AddRectangle(D3DXVECTOR2 Size, D3DXVECTOR2 Position)
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

void JWRectangle::Draw()
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

void JWRectangle::SetRectangleAlpha(BYTE Alpha)
{
	SetColorAlpha(&m_RectangleColor, Alpha);
}

void JWRectangle::SetRectangleXRGB(DWORD Color)
{
	SetColorXRGB(&m_RectangleColor, Color);
}