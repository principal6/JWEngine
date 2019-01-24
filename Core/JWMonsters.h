#pragma once

#include "JWLife.h"

namespace JWENGINE
{
	/*-----------------------------------------------------------------------------
		JWMonsterType Class
	-----------------------------------------------------------------------------*/
	class JWMonsterType final
	{
	public:
		WSTRING m_Name;
		WSTRING m_TextureFileName;
		POINT m_UnitSize;
		int m_TextureNumCols;
		int m_TextureNumRows;
		int m_HPMax;
		VECTOR<SAnimationData> m_AnimData;
		D3DXVECTOR2 m_BoundingBoxExtraSize;

	public:
		JWMonsterType() {};
		JWMonsterType(WSTRING Name, WSTRING TextureFileName, POINT LifeSize, int TextureNumCols, int TextureNumRows, int HP,
			D3DXVECTOR2 BoundingBoxExtraSize) :
			m_Name(Name), m_TextureFileName(TextureFileName), m_UnitSize(LifeSize), m_TextureNumCols(TextureNumCols),
			m_TextureNumRows(TextureNumRows), m_HPMax(HP), m_BoundingBoxExtraSize(BoundingBoxExtraSize){};
		~JWMonsterType() {};

		auto JWMonsterType::AddAnimation(SAnimationData Value)->JWMonsterType*;
	};

	/*-----------------------------------------------------------------------------
		JWMonster Class
	-----------------------------------------------------------------------------*/
	class JWMonster final : public JWLife
	{
	public:
		JWMonster();
		~JWMonster() {};

		auto JWMonster::Create(JWWindow* pJWWindow, WSTRING BaseDir, JWMap* pMap)->EError;
		void JWMonster::Destroy() override;

		void JWMonster::SetMonsterType(JWMonsterType Type);
		auto JWMonster::SetGlobalPosition(D3DXVECTOR2 Position)->JWMonster* override;
		void JWMonster::Damage(int Damage);

		void JWMonster::Draw();

	private:
		void JWMonster::SetUIPosition(D3DXVECTOR2 Position);
		void JWMonster::CalculateHP();
		void JWMonster::UpdateGlobalPosition();

	private:
		static const float OFFSET_Y_HPBAR;

		JWMonsterType m_Type;
		int m_HPCurr;
		bool m_bUILoaded;
		JWImage *m_HPFrame;
		JWImage *m_HPBar;
	};

	/*-----------------------------------------------------------------------------
		JWMonsterManager Class
	-----------------------------------------------------------------------------*/
	class JWMonsterManager final
	{
	public:
		JWMonsterManager() {};
		~JWMonsterManager() {};

		auto JWMonsterManager::Create(JWWindow* pJWWindow, WSTRING BaseDir, JWMap* pMap)->EError;
		void JWMonsterManager::Destroy();

		auto JWMonsterManager::AddMonsterType(JWMonsterType Value)->JWMonsterType*;
		auto JWMonsterManager::Spawn(WSTRING MonsterName, D3DXVECTOR2 GlobalPosition)->JWMonster*;

		void JWMonsterManager::Animate();
		void JWMonsterManager::Gravitate();
		void JWMonsterManager::Draw();
		void JWMonsterManager::DrawBoundingBox();

		auto JWMonsterManager::GetInstancePointer()->VECTOR<JWMonster>*;

	private:
		static LPDIRECT3DDEVICE9 m_pDevice;
		JWWindow* m_pJWWindow;
		JWMap* m_pMap;
		WSTRING m_BaseDir;

		VECTOR<JWMonsterType> m_Types;
		VECTOR<JWMonster> m_Instances;
	};
};