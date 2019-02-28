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

		virtual auto JWLife::Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const JWMap* pMap)->JWLife*;

		virtual auto JWLife::MakeLife(const WSTRING TextureFN, const POINT UnitSize, CINT numCols, CINT numRows, const float Scale = 1.0f)->JWLife*;

		virtual auto JWLife::SetGlobalPosition(const D3DXVECTOR2 Position)->JWLife*;

		virtual auto JWLife::GetGlobalPosition() const->const D3DXVECTOR2;
		virtual auto JWLife::GetGlobalPositionInverse() const->const D3DXVECTOR2;
		virtual auto JWLife::GetVelocity() const->const D3DXVECTOR2;
		virtual auto JWLife::GetOffsetForMapMove() const->const D3DXVECTOR2;
		virtual auto JWLife::GetScaledUnitSize() const->const D3DXVECTOR2;

		// Animation
		virtual auto JWLife::AddAnimation(const SAnimationData Value)->JWLife*;
		virtual void JWLife::SetAnimation(const EAnimationID AnimID, const bool ForceSet = false, const bool bShouldRepeat = false);
		virtual void JWLife::Animate();
		virtual void JWLife::SetDirection(const EAnimationDirection Direction);
		virtual auto JWLife::GetDirection() const->const EAnimationDirection;

		// Move
		virtual void JWLife::Accelerate(const D3DXVECTOR2 Accel);
		virtual void JWLife::AddVelocity(const D3DXVECTOR2 Vel);
		virtual void JWLife::SetVelocity(const D3DXVECTOR2 Vel);
		virtual void JWLife::MoveWithVelocity();
		virtual void JWLife::MoveConst(const D3DXVECTOR2 dXY);
		virtual void JWLife::Walk(const EAnimationDirection Direction);
		virtual void JWLife::Jump();
		virtual void JWLife::Gravitate();

	protected:
		virtual void JWLife::CalculateGlobalPositionInverse();
		virtual void JWLife::CalculateGlobalPosition();

		// Animation
		virtual void JWLife::SetFrame(CINT FrameID);

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
		virtual void JWLife::SetNumRowsAndCols(const POINT UnitSize, CINT numCols, CINT numRows);

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