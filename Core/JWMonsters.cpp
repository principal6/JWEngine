#include "../CoreBase/JWWindow.h"
#include "JWMap.h"
#include "JWMonsters.h"

using namespace JWENGINE;

/*-----------------------------------------------------------------------------
	JWMonsterType Class
-----------------------------------------------------------------------------*/

auto JWMonsterType::AddAnimation(const SAnimationData& Value) noexcept->JWMonsterType*
{
	m_AnimData.push_back(Value);

	return this;
}

/*-----------------------------------------------------------------------------
	JWMonster Class
-----------------------------------------------------------------------------*/

JWMonster::~JWMonster()
{
	JW_DELETE(m_HPFrame);
	JW_DELETE(m_HPBar);
}

auto JWMonster::Create(const JWWindow& Window, const WSTRING& BaseDir, const JWMap& Map)->JWMonster*
{
	JWLife::Create(Window, BaseDir, Map);

	m_HPFrame = new JWImage;
	m_HPFrame->Create(Window, BaseDir);
	m_HPFrame->SetTexture(L"gamegui.png");
	m_HPFrame->SetAtlasUV(D3DXVECTOR2(0, 0), D3DXVECTOR2(47, 10));

	m_HPBar = new JWImage;
	m_HPBar->Create(Window, BaseDir);
	m_HPBar->SetTexture(L"gamegui.png");
	m_HPBar->SetAtlasUV(D3DXVECTOR2(0, 10), D3DXVECTOR2(47, 10));

	return this;
}

PRIVATE void JWMonster::SetUIPosition(const D3DXVECTOR2& Position) noexcept
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

void JWMonster::SetMonsterType(const JWMonsterType Type) noexcept
{
	m_Type = Type;
	m_HPCurr = Type.m_HPMax;
}

auto JWMonster::SetGlobalPosition(const D3DXVECTOR2& Position) noexcept->JWMonster*
{
	m_GlobalPosition = Position;

	JWLife::CalculateGlobalPositionInverse();

	JWLife::SetPosition(m_GlobalPositionInverse);

	SetUIPosition(m_GlobalPositionInverse);

	return this;
}

PRIVATE void JWMonster::UpdateGlobalPosition() noexcept
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

PRIVATE void JWMonster::CalculateHP() noexcept
{
	float fPercent = static_cast<float>(JWMonster::m_HPCurr) / static_cast<float>(m_Type.m_HPMax);
	float tWidth = m_HPBar->GetScaledSize().x;

	m_HPBar->SetVisibleRange(D3DXVECTOR2(tWidth * fPercent, m_HPBar->GetScaledSize().y));
}

void JWMonster::Damage(int Damage) noexcept
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
LPDIRECT3DDEVICE9 JWMonsterManager::m_pDevice{ nullptr };

JWMonsterManager::~JWMonsterManager()
{
	if (m_pInstances.size())
	{
		for (auto& iterator : m_pInstances)
		{
			JW_DELETE(iterator);
		}
	}

	m_pJWWindow = nullptr;
	m_pDevice = nullptr;
	m_pMap = nullptr;
}

void JWMonsterManager::Create(const JWWindow& Window, const WSTRING& BaseDir, const JWMap& Map)
{
	m_pJWWindow = &Window;
	m_pDevice = Window.GetDevice();
	m_pBaseDir = &BaseDir;
	m_pMap = &Map;
}

auto JWMonsterManager::AddMonsterType(const JWMonsterType& NewType) noexcept->JWMonsterType*
{
	m_Types.push_back(NewType);

	return &m_Types[m_Types.size() - 1];
}

auto JWMonsterManager::Spawn(const WSTRING& MonsterName, const D3DXVECTOR2& GlobalPosition) noexcept->JWMonster*
{
	for (JWMonsterType& TypeIterator : m_Types)
	{
		if (TypeIterator.m_Name == MonsterName)
		{
			JWMonster* temp = new JWMonster();
			temp->Create(*m_pJWWindow, *m_pBaseDir, *m_pMap);
			temp->SetMonsterType(TypeIterator);
			temp->MakeLife(TypeIterator.m_TextureFileName, TypeIterator.m_UnitSize, TypeIterator.m_TextureNumCols,
				TypeIterator.m_TextureNumRows, 1.0f);

			for (SAnimationData& AnimIterator : TypeIterator.m_AnimData)
			{
				temp->AddAnimation(SAnimationData(AnimIterator.AnimID, AnimIterator.FrameS, AnimIterator.FrameE));
			}

			temp->SetGlobalPosition(GlobalPosition);
			temp->SetAnimation(EAnimationID::Idle);
			temp->SetBoundingBox(TypeIterator.m_BoundingBoxExtraSize);

			m_pInstances.push_back(temp);

			return m_pInstances[m_pInstances.size() - 1];
		}
	}
	return nullptr;
}

void JWMonsterManager::Animate() noexcept
{
	for (auto& InstanceIterator : m_pInstances)
	{
		InstanceIterator->Animate();
	}
}

void JWMonsterManager::Gravitate() noexcept
{
	for (auto& InstanceIterator : m_pInstances)
	{
		InstanceIterator->Gravitate();
	}
}

void JWMonsterManager::Draw() noexcept
{
	for (auto& InstanceIterator : m_pInstances)
	{
		InstanceIterator->Draw();
	}
}

void JWMonsterManager::DrawBoundingBox() noexcept
{
	for (auto& InstanceIterator : m_pInstances)
	{
		InstanceIterator->DrawBoundingBox();
	}
}

auto JWMonsterManager::GetInstancePointer() noexcept->VECTOR<JWMonster*>*
{
	return &m_pInstances;
}