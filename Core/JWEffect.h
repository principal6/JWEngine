#pragma once

#include "../CoreBase/JWImage.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWMap;
	class JWMonsterManager;
	// ***

	enum class EEffectType
	{
		Still,
		FlyRight,
		FlyDown,
		FlyUp,
	};

	class EffectTypeData
	{
	public:
		EffectTypeData() : m_Delay(0), m_RepeatCount(0) {};
		EffectTypeData(EEffectType Type, SAnimationData Data, D3DXVECTOR2 SpawnOffset, D3DXVECTOR2 BBSize, int Delay,
			int RepeatCount) :
			m_Type(Type), m_AnimData(Data), m_SpawnOffset(SpawnOffset), m_BBSize(BBSize), m_Delay(Delay),
			m_RepeatCount(RepeatCount) {};

		auto GetSpawnOffset() { return m_SpawnOffset; };
		auto GetBoundingBoxSize() { return m_BBSize; };
		auto GetStartFrame() { return m_AnimData.FrameS; };
		auto GetEndFrame() { return m_AnimData.FrameE; };
		auto GetRepeatCount() { return m_RepeatCount; };
		auto GetDelay() { return m_Delay; };

	private:
		EEffectType m_Type;
		SAnimationData m_AnimData;
		D3DXVECTOR2 m_SpawnOffset;
		D3DXVECTOR2 m_BBSize;
		int m_Delay;
		int m_RepeatCount;
	};

	class EffectInstanceData
	{
	public:
		EffectInstanceData() : m_pNext(nullptr), m_Position(D3DXVECTOR2(0, 0)) {};
		EffectInstanceData(int TypeDataID, D3DXVECTOR2 Position, D3DXVECTOR2 Offset, int CurrFrame, SBoundingBox BB,
			int Damage, int RepeatCount) :
			m_pNext(nullptr), m_TypeDataID(TypeDataID), m_Position(Position), m_Offset(Offset), m_CurrFrame(CurrFrame), m_BoundingBox(BB),
			m_Damage(Damage), m_MaxRepeatCount(RepeatCount) {};

		void SetNext(EffectInstanceData* Next) { m_pNext = Next; };
		void SetCurrFrame(CINT FrameID) { m_CurrFrame = FrameID; };
		void SetCurrRepeatCount(CINT Value) { m_CurrRepeatCount = Value; };
		void SetDamage(CINT Damage) { m_Damage = Damage; };
		void SetBoundingBox(const SBoundingBox NewBB) { m_BoundingBox = NewBB; };

		auto GetBoundingBox() const { return m_BoundingBox; };
		auto GetNext() const { return m_pNext; };
		auto GetType() const { return m_Type; };
		auto GetPos() const { return m_Position; };
		auto GetOffset() const { return m_Offset; };
		auto GetCurrRepeatCount() const { return m_CurrRepeatCount; };
		auto GetCurrFrame() const { return m_CurrFrame; };
		auto GetTypeDataID() const { return m_TypeDataID; };
		auto GetDamage() const { return m_Damage; };

	private:
		EffectInstanceData* m_pNext;
		EEffectType m_Type;
		int m_TypeDataID;
		D3DXVECTOR2 m_Position;
		D3DXVECTOR2 m_Offset;
		int m_CurrFrame;
		int m_MaxRepeatCount;
		int m_CurrRepeatCount;
		SBoundingBox m_BoundingBox;
		int m_Damage;
	};

	class JWEffect final : protected JWImage
	{
	public:
		JWEffect();
		~JWEffect() {};

		auto JWEffect::Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const JWMap* pMap)->EError;
		void JWEffect::Destroy() override;

		auto JWEffect::SetTextureAtlas(const WSTRING FileName, CINT numCols, CINT numRows)->JWEffect*;

		auto JWEffect::AddEffectType(const EEffectType Type, const SAnimationData Data, const D3DXVECTOR2 SpawnOffset, const D3DXVECTOR2 BBSize,
			CINT Delay, CINT RepeatCount = 1)->JWEffect*;

		auto JWEffect::Spawn(CINT EffectTypeID, const D3DXVECTOR2 Pos, const EAnimationDirection Dir, CINT Damage)->JWEffect*;

		void JWEffect::Draw() override;
		void JWEffect::DrawBoundingBox() override;

		void JWEffect::CheckCollisionWithMonsters(JWMonsterManager* pMonsters);

	private:
		void JWEffect::CreateVertexBuffer() override;
		void JWEffect::CreateIndexBuffer() override;
		void JWEffect::DeleteInstance(EffectInstanceData* pInstance);
		void JWEffect::Update();

	private:
		const JWMap* m_pMap;
		int m_TextureAtlasCols;
		int m_TextureAtlasRows;

		D3DXVECTOR2 m_UnitSize;

		VECTOR<EffectTypeData> m_TypeData;

		EffectInstanceData* m_pFisrtInstance;
		EffectInstanceData* m_pLastInstance;
		int m_InstanceCount;
		VECTOR<int> m_DelayCounts;
	};
};