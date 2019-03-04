#include "JWImage.h"
#include "JWWindow.h"

using namespace JWENGINE;

// Static member variable declaration
const DWORD JWImage::DEFAULT_BOUNDINGBOX_COLOR = D3DCOLOR_ARGB(255, 0, 150, 50);

void JWImage::Create(const JWWindow& Window, const WSTRING& BaseDir)
{
	m_pJWWindow = &Window;
	m_pDevice = Window.GetDevice();
	m_pBaseDir = &BaseDir;

	ClearVertexAndIndex();

	CreateVertexBuffer();
	CreateIndexBuffer();
	UpdateVertexBuffer();
	UpdateIndexBuffer();

	m_BoundingBoxLine.Create(m_pDevice);
	m_BoundingBoxLine.AddBox(D3DXVECTOR2(0, 0), D3DXVECTOR2(10, 10), m_BoundingBoxColor);
	m_BoundingBoxLine.AddEnd();
}

void JWImage::Destroy() noexcept
{
	m_pJWWindow = nullptr;
	m_pDevice = nullptr; // Just set to nullptr cuz it's newed in <JWWindow> class

	m_BoundingBoxLine.Destroy();

	if (!m_bUseStaticTexture)
	{
		JW_RELEASE(m_pTexture);
	}
	JW_RELEASE(m_pIndexBuffer);
	JW_RELEASE(m_pVertexBuffer);
}

PROTECTED void JWImage::ClearVertexAndIndex() noexcept
{
	m_Vertices.clear();
	m_Indices.clear();
}

PROTECTED void JWImage::CreateVertexBuffer()
{
	if (m_Vertices.size() == 0)
	{
		m_Vertices.push_back(SVertexImage(m_Position.x, m_Position.y, 0.0f, 0.0f));
		m_Vertices.push_back(SVertexImage(m_Position.x + m_Size.x, m_Position.y, 1.0f, 0.0f));
		m_Vertices.push_back(SVertexImage(m_Position.x, m_Position.y + m_Size.y, 0.0f, 1.0f));
		m_Vertices.push_back(SVertexImage(m_Position.x + m_Size.x, m_Position.y + m_Size.y, 1.0f, 1.0f));
	}

	int vertex_size = sizeof(SVertexImage) * static_cast<int>(m_Vertices.size());
	if (FAILED(m_pDevice->CreateVertexBuffer(vertex_size, 0, D3DFVF_TEXTURE, D3DPOOL_MANAGED, &m_pVertexBuffer, nullptr)))
	{
		THROW_CREATION_FAILED;
	}
}

PROTECTED void JWImage::CreateIndexBuffer()
{
	if (m_Indices.size() == 0)
	{
		m_Indices.push_back(SIndex3(0, 1, 3));
		m_Indices.push_back(SIndex3(0, 3, 2));
	}

	int index_size = sizeof(SIndex3) * static_cast<int>(m_Indices.size());
	if (FAILED(m_pDevice->CreateIndexBuffer(index_size, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, nullptr)))
	{
		THROW_CREATION_FAILED;
	}
}

PROTECTED void JWImage::UpdateVertexBuffer()
{
	if (m_Vertices.size())
	{
		int vertex_size = sizeof(SVertexImage) * static_cast<int>(m_Vertices.size());
		void* pVertices = nullptr;
		if (SUCCEEDED(m_pVertexBuffer->Lock(0, vertex_size, (void**)&pVertices, 0)))
		{
			memcpy(pVertices, &m_Vertices[0], vertex_size);
			m_pVertexBuffer->Unlock();
		}
	}
}

PROTECTED void JWImage::UpdateIndexBuffer()
{
	if (m_Indices.size())
	{
		int index_size = sizeof(SIndex3) * static_cast<int>(m_Indices.size());
		void* pIndices = nullptr;
		if (SUCCEEDED(m_pIndexBuffer->Lock(0, index_size, (void **)&pIndices, 0)))
		{
			memcpy(pIndices, &m_Indices[0], index_size);
			m_pIndexBuffer->Unlock();
		}
	}
}

void JWImage::Draw() noexcept
{
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	
	if (m_pTexture)
	{
		m_pDevice->SetTexture(0, m_pTexture);

		// Texture alpha * Diffuse color alpha
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	}
	else
	{
		m_pDevice->SetTexture(0, nullptr);
	}

	m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SVertexImage));
	m_pDevice->SetFVF(D3DFVF_TEXTURE);
	m_pDevice->SetIndices(m_pIndexBuffer);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, static_cast<int>(m_Vertices.size()), 0, static_cast<int>(m_Indices.size()));

	m_pDevice->SetTexture(0, nullptr);
}

void JWImage::DrawBoundingBox() noexcept
{
	m_BoundingBoxLine.SetBox(m_Position + m_BoundingBox.PositionOffset, m_BoundingBox.Size);
	m_BoundingBoxLine.Draw();
}

void JWImage::FlipHorizontal() noexcept
{
	float tempu1 = m_Vertices[0].u;

	m_Vertices[0].u = m_Vertices[1].u;
	m_Vertices[2].u = m_Vertices[3].u;
	m_Vertices[1].u = tempu1;
	m_Vertices[3].u = tempu1;

	UpdateVertexBuffer();
}

void JWImage::FlipVertical() noexcept
{
	float tempv1 = m_Vertices[0].v;

	m_Vertices[0].v = m_Vertices[2].v;
	m_Vertices[1].v = m_Vertices[3].v;
	m_Vertices[2].v = tempv1;
	m_Vertices[3].v = tempv1;

	UpdateVertexBuffer();
}

void JWImage::SetPosition(const D3DXVECTOR2& Position) noexcept
{
	m_Position = Position;

	UpdateVertexData();
}

void JWImage::SetPositionCentered(const D3DXVECTOR2& Position) noexcept
{
	m_Position = D3DXVECTOR2(Position.x - (static_cast<float>(m_ScaledSize.x) / 2.0f),
		Position.y - (static_cast<float>(m_ScaledSize.y) / 2.0f));

	UpdateVertexData();
}

void JWImage::SetSize(const D3DXVECTOR2& Size) noexcept
{
	m_Size = Size;

	m_ScaledSize.x = m_Size.x * m_Scale.x;
	m_ScaledSize.y = m_Size.y * m_Scale.y;

	SetBoundingBox(m_BoundingBoxExtraSize);

	UpdateVertexData();
}

void JWImage::SetTexture(const WSTRING& FileName)
{
	JW_RELEASE(m_pTexture);

	WSTRING NewFileName;
	NewFileName = *m_pBaseDir;
	NewFileName += ASSET_DIR;
	NewFileName += FileName;

	D3DXIMAGE_INFO tImageInfo;
	if (FAILED(D3DXCreateTextureFromFileExW(m_pDevice, NewFileName.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, &tImageInfo, nullptr, &m_pTexture)))
	{
		THROW_CREATION_FAILED;
	}

	m_Size.x = static_cast<float>(tImageInfo.Width);
	m_Size.y = static_cast<float>(tImageInfo.Height);

	// Entire texture size (will never be changed unless the texture itself changes)
	m_AtlasSize = m_Size;

	m_ScaledSize.x = m_Size.x * m_Scale.x;
	m_ScaledSize.y = m_Size.y * m_Scale.y;

	UpdateVertexData();
}

void JWImage::SetTexture(const LPDIRECT3DTEXTURE9 pTexture, const D3DXIMAGE_INFO* pInfo) noexcept
{
	m_bUseStaticTexture = true;

	if (m_pTexture)
	{
		if (!m_bUseStaticTexture)
		{
			m_pTexture->Release();
		}
		m_pTexture = nullptr;
	}

	m_pTexture = pTexture;

	m_Size.x = static_cast<float>(pInfo->Width);
	m_Size.y = static_cast<float>(pInfo->Height);

	// Entire texture size (will never be changed unless the texture itself changes)
	m_AtlasSize = m_Size;

	m_ScaledSize.x = m_Size.x * m_Scale.x;
	m_ScaledSize.y = m_Size.y * m_Scale.y;

	UpdateVertexData();
}

auto JWImage::SetColor(DWORD Color) noexcept->JWImage*
{
	if (m_Vertices.size())
	{
		for (SVertexImage& iterator : m_Vertices)
		{
			iterator.color = Color;
		}
		UpdateVertexBuffer();
	}

	return this;
}

auto JWImage::SetAlpha(BYTE Alpha) noexcept->JWImage*
{
	if (m_Vertices.size())
	{
		for (SVertexImage& iterator : m_Vertices)
		{
			SetColorAlpha(&iterator.color, Alpha);
		}
		UpdateVertexBuffer();
	}

	return this;
}

auto JWImage::SetXRGB(DWORD XRGB) noexcept->JWImage*
{
	if(m_Vertices.size())
	{
		for (SVertexImage& iterator : m_Vertices)
		{
			SetColorXRGB(&iterator.color, XRGB);
		}
		UpdateVertexBuffer();
	}

	return this;
}

auto JWImage::SetScale(const D3DXVECTOR2& Scale) noexcept->JWImage*
{
	m_Scale = Scale;

	m_ScaledSize.x = m_Size.x * m_Scale.x;
	m_ScaledSize.y = m_Size.y * m_Scale.y;

	SetBoundingBox(m_BoundingBoxExtraSize);

	UpdateVertexData();

	return this;
}

auto JWImage::SetVisibleRange(const D3DXVECTOR2& Range) noexcept->JWImage*
{
	m_VisibleRange = Range;

	SetAtlasUV(m_OffsetInAtlas, m_VisibleRange, false);

	return this;
}

auto JWImage::SetAtlasUV(const D3DXVECTOR2& OffsetInAtlas, const D3DXVECTOR2& Size, bool bSetSize) noexcept->JWImage*
{
	if (m_Vertices.size())
	{
		if (bSetSize)
		{
			SetSize(Size);
		}

		m_OffsetInAtlas = OffsetInAtlas;

		STextureUV uv;
		uv.u1 = min(m_OffsetInAtlas.x / m_AtlasSize.x, 1.0f);
		uv.u2 = min((m_OffsetInAtlas.x + Size.x) / m_AtlasSize.x, 1.0f);
		uv.v1 = min(m_OffsetInAtlas.y / m_AtlasSize.y, 1.0f);
		uv.v2 = min((m_OffsetInAtlas.y + Size.y) / m_AtlasSize.y, 1.0f);

		UpdateVertexData(uv);
	}

	return this;
}

auto JWImage::SetUVRange(const STextureUV& UV) noexcept->JWImage*
{
	if (m_Vertices.size())
	{
		UpdateVertexData(UV);
	}

	return this;
}

auto JWImage::SetBoundingBox(const D3DXVECTOR2& ExtraSize) noexcept->JWImage*
{
	m_BoundingBoxExtraSize = ExtraSize;

	m_BoundingBox.PositionOffset.x = -m_BoundingBoxExtraSize.x / 2.0f;
	m_BoundingBox.PositionOffset.y = -m_BoundingBoxExtraSize.y;

	m_BoundingBox.Size.x = m_ScaledSize.x + m_BoundingBoxExtraSize.x;
	m_BoundingBox.Size.y = m_ScaledSize.y + m_BoundingBoxExtraSize.y;

	return this;
}

auto JWImage::SetBoundingBoxAlpha(BYTE Alpha) noexcept->JWImage*
{
	SetColorAlpha(&m_BoundingBoxColor, Alpha);
	m_BoundingBoxLine.SetEntireAlpha(Alpha);

	return this;
}

auto JWImage::SetBoundingBoxXRGB(DWORD XRGB) noexcept->JWImage*
{
	SetColorXRGB(&m_BoundingBoxColor, XRGB);
	m_BoundingBoxLine.SetEntireXRGB(XRGB);

	return this;
}

PROTECTED void JWImage::UpdateVertexData() noexcept
{
	if (m_Vertices.size() < 4)
	{
		return;
	}

	D3DXVECTOR2 tempSize = m_Size;

	if ((m_VisibleRange.x != VISIBLE_RANGE_NOT_SET) && (m_VisibleRange.y != VISIBLE_RANGE_NOT_SET))
	{
		tempSize = m_VisibleRange;
	}

	m_Vertices[0].x = m_Position.x;
	m_Vertices[0].y = m_Position.y;
	m_Vertices[1].x = m_Position.x + tempSize.x * m_Scale.x;
	m_Vertices[1].y = m_Position.y;
	m_Vertices[2].x = m_Position.x;
	m_Vertices[2].y = m_Position.y + tempSize.y * m_Scale.y;
	m_Vertices[3].x = m_Position.x + tempSize.x * m_Scale.x;
	m_Vertices[3].y = m_Position.y + tempSize.y * m_Scale.y;

	UpdateVertexBuffer();
}

PROTECTED void JWImage::UpdateVertexData(const STextureUV& UV) noexcept
{
	if (m_Vertices.size() < 4)
	{
		return;
	}

	m_Vertices[0].u = UV.u1;
	m_Vertices[0].v = UV.v1;
	m_Vertices[1].u = UV.u2;
	m_Vertices[1].v = UV.v1;
	m_Vertices[2].u = UV.u1;
	m_Vertices[2].v = UV.v2;
	m_Vertices[3].u = UV.u2;
	m_Vertices[3].v = UV.v2;

	UpdateVertexBuffer();
}

auto JWImage::GetSize() const noexcept->const D3DXVECTOR2&
{
	return m_Size;
}

auto JWImage::GetScaledSize() const noexcept->const D3DXVECTOR2&
{
	return m_ScaledSize;
}

auto JWImage::GetPosition() const noexcept->const D3DXVECTOR2&
{
	return m_Position;
}

auto JWImage::GetCenterPosition() const noexcept->const D3DXVECTOR2
{
	D3DXVECTOR2 Result = m_Position;
	Result.x += m_ScaledSize.x / 2.0f;
	Result.y += m_ScaledSize.y / 2.0f;

	return Result;
}

auto JWImage::GetBoundingBox() const noexcept->const SBoundingBox
{
	SBoundingBox Result;
	Result.PositionOffset = m_Position + m_BoundingBox.PositionOffset;
	Result.Size = m_BoundingBox.Size;

	return Result;
}

auto JWImage::IsTextureLoaded() const noexcept->bool
{
	if (m_pTexture)
		return true;

	return false;
}