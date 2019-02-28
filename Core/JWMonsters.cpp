#include "../CoreBase/JWWindow.h"
#include "JWMap.h"
#include "JWMonsters.h"

using namespace JWENGINE;

/*-----------------------------------------------------------------------------
	JWMonsterType Class
-----------------------------------------------------------------------------*/

auto JWMonsterType::AddAnimation(const SAnimationData Value)->JWMonsterType*
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
	m_HPFrame = nullptr;
	m_HPBar = nullptr;
}

auto JWMonster::Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const JWMap* pMap)->JWMonster*
{
	JWLife::Create(pJWWindow, pBaseDir, pMap);

	if (m_HPFrame = new JWImage)
	{
		m_HPFrame->Create(pJWWindow, pBaseDir);
		m_HPFrame->SetTexture(L"gamegui.png");
		m_HPFrame->SetAtlasUV(D3DXVECTOR2(0, 0), D3DXVECTOR2(47, 10));
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}

	if (m_HPBar = new JWImage)
	{
		m_HPBar->Create(pJWWindow, pBaseDir);
		m_HPBar->SetTexture(L"gamegui.png");
		m_HPBar->SetAtlasUV(D3DXVECTOR2(0, 10), D3DXVECTOR2(47, 10));
	}
	else
	{
		throw EError::ALLOCATION_FAILURE;
	}

	return this;
}

void JWMonster::Destroy() noexcept
{
	JW_DESTROY(m_HPFrame);
	JW_DESTROY(m_HPBar);
}

void JWMonster::SetUIPosition(const D3DXVECTOR2 Position)
{
	if ((!m_HPFrame) || (!m_HPBar))
	{
		return;
	}

	D3DXVECTOR2 adjusted_position = GetCenterPosition();
	adjusted_position.y = Position.y;
	adjusted_position.y -= OFFSET_Y_HPBAR + m_BoundingBoxExtraSize.y;
	adjusted_position.x -= m_HPFrame->GetSize().x / 2.0f;

	m_HPFrame->SetPosition(adjusted_position);
	m_HPBar->SetPosition(adjusted_position);
}

void JWMonster::SetMonsterType(const JWMonsterType Type)
{
	m_Type = Type;
	m_HPCurr = Type.m_HPMax;
}

auto JWMonster::SetGlobalPosition(const D3DXVECTOR2 Position)->JWMonster*
{
	m_GlobalPosition = Position;
	JWLife::CalculateGlobalPositionInverse();

	JWLife::SetPosition(m_GlobalPositionInverse);

	SetUIPosition(m_GlobalPositionInverse);

	return this;
}

void JWMonster::UpdateGlobalPosition()
{
	D3DXVECTOR2 MapOffset = m_pMap->GetMapOffset();
	float MapOffsetZeroY = static_cast<float>(m_pMap->GetMapOffsetZeroY());

	D3DXVECTOR2 tGP = m_GlobalPosition;
	m_GlobalPosition.x += MapOffset.x;
	m_GlobalPosition.y -= MapOffset.y - MapOffsetZeroY;

	CalculateGlobalPositionInverse();
	m_GlobalPosition = tGP;

	SetPosition(m_GlobalPositionInverse);

	SetUIPosition(m_GlobalPositionInverse);
}

void JWMonster::CalculateHP()
{
	float fPercent = static_cast<float>(JWMonster::m_HPCurr) / static_cast<float>(m_Type.m_HPMax);
	float tWidth = m_HPBar->GetScaledSize().x;

	m_HPBar->SetVisibleRange(D3DXVECTOR2(tWidth * fPercent, m_HPBar->GetScaledSize().y));
}

void JWMonster::Damage(CINT Damage)
{
	m_HPCurr -= Damage;
	m_HPCurr = max(m_HPCurr, 0);
}

void JWMonster::Draw() noexcept
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

void JWMonsterManager::Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const JWMap* pMap)
{
	if (pJWWindow == nullptr)
	{
		throw EError::NULLPTR_WINDOW;
	}
	
	if (pMap == nullptr)
	{
		throw EError::NULLPTR_MAP;
	}
	
	m_pJWWindow = pJWWindow;
	m_pDevice = pJWWindow->GetDevice();
	m_pBaseDir = pBaseDir;
	m_pMap = pMap;
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

auto JWMonsterManager::AddMonsterType(const JWMonsterType NewType)->JWMonsterType*
{
	m_Types.push_back(NewType);

	return &m_Types[m_Types.size() - 1];
}

auto JWMonsterManager::Spawn(const WSTRING MonsterName, const D3DXVECTOR2 GlobalPosition)->JWMonster*
{
	for (JWMonsterType& TypeIterator : m_Types)
	{
		if (TypeIterator.m_Name == MonsterName)
		{
			JWMonster Temp;
			Temp.Create(m_pJWWindow, m_pBaseDir, m_pMap);
			Temp.SetMonsterType(TypeIterator);
			Temp.MakeLife(TypeIterator.m_TextureFileName, TypeIterator.m_UnitSize, TypeIterator.m_TextureNumCols,
				TypeIterator.m_TextureNumRows, 1.0f);

			for (SAnimationData& AnimIterator : TypeIterator.m_AnimData)
			{
				Temp.AddAnimation(SAnimationData(AnimIterator.AnimID, AnimIterator.FrameS, AnimIterator.FrameE));
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