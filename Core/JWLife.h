#pragma once

#include "../CoreBase/JWImage.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	class JWMap;
	// ***

	class JWLife : public JWImage
	{
	public:
		JWLife();
		virtual ~JWLife() {};

		virtual auto JWLife::Create(const JWWindow& Window, const WSTRING& BaseDir, const JWMap& Map)->JWLife*;

		virtual auto JWLife::MakeLife(const WSTRING& TextureFN, const POINT UnitSize, int numCols, int numRows, float Scale = 1.0f) noexcept->JWLife*;

		virtual auto JWLife::SetGlobalPosition(const D3DXVECTOR2& Position) noexcept->JWLife*;

		virtual auto JWLife::GetGlobalPosition() const->const D3DXVECTOR2;
		virtual auto JWLife::GetGlobalPositionInverse() const->const D3DXVECTOR2;
		virtual auto JWLife::GetVelocity() const->const D3DXVECTOR2;
		virtual auto JWLife::GetOffsetForMapMove() const->const D3DXVECTOR2;
		virtual auto JWLife::GetScaledUnitSize() const->const D3DXVECTOR2;

		// Animation
		virtual auto JWLife::AddAnimation(const SAnimationData& Value) noexcept->JWLife*;
		virtual void JWLife::SetAnimation(EAnimationID AnimID, bool ForceSet = false, bool bShouldRepeat = false) noexcept;
		virtual void JWLife::Animate() noexcept;
		virtual void JWLife::SetDirection(EAnimationDirection Direction) noexcept;
		virtual auto JWLife::GetDirection() const noexcept->EAnimationDirection;

		// Move
		virtual void JWLife::Accelerate(const D3DXVECTOR2& Accel) noexcept;
		virtual void JWLife::AddVelocity(const D3DXVECTOR2& Vel) noexcept;
		virtual void JWLife::SetVelocity(const D3DXVECTOR2& Vel) noexcept;
		virtual void JWLife::MoveWithVelocity() noexcept;
		virtual void JWLife::MoveConst(const D3DXVECTOR2& dXY) noexcept;
		virtual void JWLife::Walk(EAnimationDirection Direction) noexcept;
		virtual void JWLife::Jump() noexcept;
		virtual void JWLife::Gravitate() noexcept;

	protected:
		virtual void JWLife::CalculateGlobalPositionInverse() noexcept;
		virtual void JWLife::CalculateGlobalPosition() noexcept;

		// Animation
		virtual void JWLife::SetFrame(int FrameID) noexcept;

	protected:
		static const D3DXVECTOR2 JUMP_POWER;
		static const D3DXVECTOR2 GRAVITY;
		static const float STRIDE;

		const JWMap* m_pMap;
		D3DXVECTOR2 m_GlobalPosition;
		D3DXVECTOR2 m_GlobalPositionInverse;
		D3DXVECTOR2 m_Velocity;
		bool m_bHitGround;

		D3DXVECTOR2 m_UnitSize;

	private:
		virtual void JWLife::SetNumRowsAndCols(const POINT UnitSize, int numCols, int numRows) noexcept;

	private:
		int m_SheetRows;
		int m_SheetCols;

		EAnimationDirection m_AnimDir;
		EAnimationID m_CurrAnimID;
		size_t m_CurrAnimDataIndex;
		int m_CurrFrameID;
		VECTOR<SAnimationData> m_AnimData;
		std::map<int, size_t> m_AnimDataMap;
		bool m_bAnimated;
		bool m_bShouldRepeatAnimation;
	};
};