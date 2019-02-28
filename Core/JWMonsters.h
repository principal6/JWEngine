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
		JWMonsterType() {};
		JWMonsterType(WSTRING Name, WSTRING TextureFileName, POINT LifeSize, int TextureNumCols, int TextureNumRows, int HP,
			D3DXVECTOR2 BoundingBoxExtraSize) :
			m_Name(Name), m_TextureFileName(TextureFileName), m_UnitSize(LifeSize), m_TextureNumCols(TextureNumCols),
			m_TextureNumRows(TextureNumRows), m_HPMax(HP), m_BoundingBoxExtraSize(BoundingBoxExtraSize){};
		~JWMonsterType() {};

		auto AddAnimation(const SAnimationData Value)->JWMonsterType*;

	public:
		WSTRING m_Name;
		WSTRING m_TextureFileName;
		POINT m_UnitSize;
		int m_TextureNumCols;
		int m_TextureNumRows;
		int m_HPMax;
		VECTOR<SAnimationData> m_AnimData;
		D3DXVECTOR2 m_BoundingBoxExtraSize;
	};

	/*-----------------------------------------------------------------------------
		JWMonster Class
	-----------------------------------------------------------------------------*/
	class JWMonster final : public JWLife
	{
	public:
		JWMonster();
		~JWMonster() {};

		auto Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const JWMap* pMap)->JWMonster* override;
		void Destroy() noexcept override;

		void SetMonsterType(const JWMonsterType Type);
		auto SetGlobalPosition(const D3DXVECTOR2 Position)->JWMonster* override;
		void Damage(CINT Damage);

		void Draw() noexcept;

	private:
		void SetUIPosition(const D3DXVECTOR2 Position);
		void CalculateHP();
		void UpdateGlobalPosition();

	private:
		static const float OFFSET_Y_HPBAR;

		JWMonsterType m_Type;
		int m_HPCurr;
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

		void Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const JWMap* pMap);
		void Destroy();

		auto AddMonsterType(const JWMonsterType NewType)->JWMonsterType*;
		auto Spawn(const WSTRING MonsterName, const D3DXVECTOR2 GlobalPosition)->JWMonster*;

		void Animate();
		void Gravitate();
		void Draw();
		void DrawBoundingBox();

		auto GetInstancePointer()->VECTOR<JWMonster>*;

	private:
		static LPDIRECT3DDEVICE9 m_pDevice;

		const JWWindow* m_pJWWindow;
		const WSTRING* m_pBaseDir;
		const JWMap* m_pMap;

		VECTOR<JWMonsterType> m_Types;
		VECTOR<JWMonster> m_Instances;
	};
};