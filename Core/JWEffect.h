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
		Invalid,

		Still,
		FlyRight,
		FlyDown,
		FlyUp,
	};

	class EffectTypeData
	{
	public:
		EffectTypeData() {};
		EffectTypeData(EEffectType Type, SAnimationData Data, D3DXVECTOR2 SpawnOffset, D3DXVECTOR2 BBSize, int Delay, int RepeatCount) :
			m_Type(Type), m_AnimData(Data), m_SpawnOffset(SpawnOffset), m_BBSize(BBSize), m_Delay(Delay), m_RepeatCount(RepeatCount) {};

		auto GetSpawnOffset() const noexcept { return m_SpawnOffset; };
		auto GetBoundingBoxSize() const noexcept { return m_BBSize; };
		auto GetStartFrame() const noexcept { return m_AnimData.FrameS; };
		auto GetEndFrame() const noexcept { return m_AnimData.FrameE; };
		auto GetRepeatCount() const noexcept { return m_RepeatCount; };
		auto GetDelay() const noexcept { return m_Delay; };

	private:
		EEffectType m_Type{ EEffectType::Invalid };
		SAnimationData m_AnimData{};
		D3DXVECTOR2 m_SpawnOffset{};
		D3DXVECTOR2 m_BBSize{};
		int m_Delay{};
		int m_RepeatCount{};
	};

	class EffectInstanceData
	{
	public:
		EffectInstanceData() {};
		EffectInstanceData(int TypeDataID, D3DXVECTOR2 Position, D3DXVECTOR2 Offset, int CurrFrame, SBoundingBox BB, int Damage, int RepeatCount) :
			m_TypeDataID(TypeDataID), m_Position(Position), m_Offset(Offset), m_CurrFrame(CurrFrame), m_BoundingBox(BB),
			m_Damage(Damage), m_MaxRepeatCount(RepeatCount) {};

		void SetNext(EffectInstanceData* Next) noexcept { m_pNext = Next; };
		void SetCurrFrame(int FrameID) noexcept { m_CurrFrame = FrameID; };
		void SetCurrRepeatCount(int Value) noexcept { m_CurrRepeatCount = Value; };
		void SetDamage(int Damage) noexcept { m_Damage = Damage; };
		void SetBoundingBox(const SBoundingBox NewBB) noexcept { m_BoundingBox = NewBB; };

		auto GetBoundingBox() const noexcept { return m_BoundingBox; };
		auto GetNext() const noexcept { return m_pNext; };
		auto GetType() const noexcept { return m_Type; };
		auto GetPos() const noexcept { return m_Position; };
		auto GetOffset() const noexcept { return m_Offset; };
		auto GetCurrRepeatCount() const noexcept { return m_CurrRepeatCount; };
		auto GetCurrFrame() const noexcept { return m_CurrFrame; };
		auto GetTypeDataID() const noexcept { return m_TypeDataID; };
		auto GetDamage() const noexcept { return m_Damage; };

	private:
		EffectInstanceData* m_pNext{ nullptr };
		EEffectType m_Type{};
		int m_TypeDataID{};
		D3DXVECTOR2 m_Position{};
		D3DXVECTOR2 m_Offset{};
		int m_CurrFrame{};
		int m_MaxRepeatCount{};
		int m_CurrRepeatCount{};
		SBoundingBox m_BoundingBox{};
		int m_Damage{};
	};

	class JWEffect final : protected JWImage
	{
	public:
		JWEffect() {};
		~JWEffect();

		auto Create(const JWWindow& Window, const WSTRING& BaseDir, const JWMap& Map) noexcept->JWEffect*;

		auto SetTextureAtlas(const WSTRING& FileName, int numCols, int numRows)->JWEffect*;

		auto AddEffectType(const EEffectType Type, const SAnimationData& Data, const D3DXVECTOR2& SpawnOffset, const D3DXVECTOR2& BBSize,
			int Delay, int RepeatCount = 1) noexcept->JWEffect*;

		auto Spawn(int EffectTypeID, const D3DXVECTOR2& Pos, const EAnimationDirection Dir, int Damage)->JWEffect*;

		void Draw() noexcept override;
		void DrawBoundingBox() noexcept override;

		void CheckCollisionWithMonsters(JWMonsterManager* pMonsters);

	private:
		void CreateVertexBuffer() noexcept override;
		void CreateIndexBuffer() noexcept override;
		void Unspawn(EffectInstanceData* pInstance);
		void Update();

	private:
		const JWMap* m_pMap{ nullptr };
		int m_TextureAtlasCols{ 0 };
		int m_TextureAtlasRows{ 0 };

		D3DXVECTOR2 m_UnitSize{};

		VECTOR<EffectTypeData> m_TypeData;

		EffectInstanceData* m_pFisrtInstance{ nullptr };
		EffectInstanceData* m_pLastInstance{ nullptr };
		int m_InstanceCount{ 0 };
		VECTOR<int> m_DelayCounts;
	};
};