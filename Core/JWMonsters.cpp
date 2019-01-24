#include "../CoreBase/JWWindow.h"
#include "JWMap.h"
#include "JWMonsters.h"

using namespace JWENGINE;

/*-----------------------------------------------------------------------------
	JWMonsterType Class
-----------------------------------------------------------------------------*/

auto JWMonsterType::AddAnimation(SAnimationData Value)->JWMonsterType*
{
	m_AnimData.push_back(Value);
	return this;
}

/*-----------------------------------------------------------------------------
	JWMonster Class
-----------------------------------------------------------------------------*/

// Static member variable declaration
const float JWMonster::OFFSET_Y_HPBAR = 16.0f;

JWMonster::JWMonster()
{
	m_HPCurr = 0;
	m_bUILoaded = false;
	m_HPFrame = nullptr;
	m_HPBar = nullptr;
}

auto JWMonster::Create(JWWindow* pJWWindow, WSTRING BaseDir, JWMap* pMap)->EError
{
	if (DX_SUCCEEDED(JWLife::Create(pJWWindow, BaseDir, pMap)))
	{
		m_HPFrame = new JWImage;
		m_HPFrame->Create(pJWWindow, BaseDir);
		m_HPFrame->SetTexture(L"gamegui.png");
		m_HPFrame->SetAtlasUV(D3DXVECTOR2(0, 0), D3DXVECTOR2(47, 10));

		m_HPBar = new JWImage;
		m_HPBar->Create(pJWWindow, BaseDir);
		m_HPBar->SetTexture(L"gamegui.png");
		m_HPBar->SetAtlasUV(D3DXVECTOR2(0, 10), D3DXVECTOR2(47, 10));

		m_bUILoaded = true;

		return EError::OK;
	}

	return EError::LIFE_NOT_CREATED;
}

void JWMonster::Destroy()
{
	DX_DESTROY(m_HPFrame);
	DX_DESTROY(m_HPBar);
}

void JWMonster::SetUIPosition(D3DXVECTOR2 Position)
{
	D3DXVECTOR2 tPos = GetCenterPosition();
	tPos.y = Position.y;
	tPos.y -= OFFSET_Y_HPBAR + m_BoundingBoxExtraSize.y;
	tPos.x -= m_HPFrame->GetSize().x / 2.0f;

	m_HPFrame->SetPosition(tPos);
	m_HPBar->SetPosition(tPos);
}

void JWMonster::SetMonsterType(JWMonsterType Type)
{
	m_Type = Type;
	m_HPCurr = Type.m_HPMax;
}

auto JWMonster::SetGlobalPosition(D3DXVECTOR2 Position)->JWMonster*
{
	m_GlobalPos = Position;
	JWLife::CalculateGlobalPositionInverse();

	JWLife::SetPosition(m_GlobalPosInverse);

	if (m_bUILoaded)
		SetUIPosition(m_GlobalPosInverse);

	return this;
}

void JWMonster::UpdateGlobalPosition()
{
	D3DXVECTOR2 MapOffset = m_pMap->GetMapOffset();
	float MapOffsetZeroY = static_cast<float>(m_pMap->GetMapOffsetZeroY());

	D3DXVECTOR2 tGP = m_GlobalPos;
	m_GlobalPos.x += MapOffset.x;
	m_GlobalPos.y -= MapOffset.y - MapOffsetZeroY;

	CalculateGlobalPositionInverse();
	m_GlobalPos = tGP;

	SetPosition(m_GlobalPosInverse);

	if (m_bUILoaded)
		SetUIPosition(m_GlobalPosInverse);
}

void JWMonster::CalculateHP()
{
	float fPercent = static_cast<float>(JWMonster::m_HPCurr) / static_cast<float>(m_Type.m_HPMax);
	float tWidth = m_HPBar->GetScaledSize().x;

	m_HPBar->SetVisibleRange(D3DXVECTOR2(tWidth * fPercent, m_HPBar->GetScaledSize().y));
}

void JWMonster::Damage(int Damage)
{
	m_HPCurr -= Damage;
	if (m_HPCurr < 0)
		m_HPCurr = 0;
}

void JWMonster::Draw()
{
	CalculateHP();
	UpdateGlobalPosition();

	JWLife::Draw();
	m_HPFrame->Draw();
	m_HPBar->Draw();
}

/*-----------------------------------------------------------------------------
	JWMonsterManager Class
-----------------------------------------------------------------------------*/

// Static member variable declaration
LPDIRECT3DDEVICE9 JWMonsterManager::m_pDevice;

auto JWMonsterManager::Create(JWWindow* pJWWindow, WSTRING BaseDir, JWMap* pMap)->EError
{
	if (pJWWindow == nullptr)
		return EError::NULLPTR_WINDOW;

	if (pMap == nullptr)
		return EError::NULLPTR_MAP;

	m_pJWWindow = pJWWindow;
	m_pDevice = pJWWindow->GetDevice();
	m_BaseDir = BaseDir;
	m_pMap = pMap;

	return EError::OK;
}

void JWMonsterManager::Destroy()
{
	m_pJWWindow = nullptr;
	m_pDevice = nullptr;
	m_pMap = nullptr;

	for (JWMonster& InstanceIterator : m_Instances)
	{
		InstanceIterator.Destroy();
	}
}

auto JWMonsterManager::AddMonsterType(JWMonsterType Value)->JWMonsterType*
{
	m_Types.push_back(Value);
	return &m_Types[m_Types.size() - 1];
}

auto JWMonsterManager::Spawn(WSTRING MonsterName, D3DXVECTOR2 GlobalPosition)->JWMonster*
{
	for (JWMonsterType& TypeIterator : m_Types)
	{
		if (TypeIterator.m_Name == MonsterName)
		{
			JWMonster Temp;
			Temp.Create(m_pJWWindow, m_BaseDir, m_pMap);
			Temp.SetMonsterType(TypeIterator);
			Temp.MakeLife(TypeIterator.m_TextureFileName, TypeIterator.m_UnitSize, TypeIterator.m_TextureNumCols,
				TypeIterator.m_TextureNumRows, 1.0f);

			for (SAnimationData& AnimIterator : TypeIterator.m_AnimData)
			{
				Temp.AddAnimation(AnimIterator.AnimID, AnimIterator.FrameS, AnimIterator.FrameE);
			}

			Temp.SetGlobalPosition(GlobalPosition);
			Temp.SetAnimation(EAnimationID::Idle);
			Temp.SetBoundingBox(TypeIterator.m_BoundingBoxExtraSize);

			m_Instances.push_back(Temp);

			return &m_Instances[m_Instances.size() - 1];
		}
	}
	return nullptr;
}

void JWMonsterManager::Animate()
{
	for (JWMonster& InstanceIterator : m_Instances)
	{
		InstanceIterator.Animate();
	}
}

void JWMonsterManager::Gravitate()
{
	for (JWMonster& InstanceIterator : m_Instances)
	{
		InstanceIterator.Gravitate();
	}
}

void JWMonsterManager::Draw()
{
	for (JWMonster& InstanceIterator : m_Instances)
	{
		InstanceIterator.Draw();
	}
}

void JWMonsterManager::DrawBoundingBox()
{
	for (JWMonster& InstanceIterator : m_Instances)
	{
		InstanceIterator.DrawBoundingBox();
	}
}

auto JWMonsterManager::GetInstancePointer()->VECTOR<JWMonster>*
{
	return &m_Instances;
}