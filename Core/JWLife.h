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

		virtual auto JWLife::Create(JWWindow* pJWWindow, WSTRING BaseDir, JWMap* pMap)->EError;

		virtual auto JWLife::MakeLife(WSTRING TextureFN, POINT UnitSize, int numCols, int numRows, float Scale = 1.0f)->JWLife*;

		virtual auto JWLife::SetGlobalPosition(D3DXVECTOR2 Position)->JWLife*;

		virtual auto JWLife::GetGlobalPosition() const->D3DXVECTOR2;
		virtual auto JWLife::GetGlobalPositionInverse() const->D3DXVECTOR2;
		virtual auto JWLife::GetVelocity() const->D3DXVECTOR2;
		virtual auto JWLife::GetOffsetForMapMove() const->D3DXVECTOR2;

		// Animation
		virtual auto JWLife::AddAnimation(EAnimationID AnimID, int StartFrame, int EndFrame)->JWLife*;
		virtual void JWLife::Animate();
		virtual void JWLife::SetFrame(int FrameID);
		virtual void JWLife::SetAnimation(EAnimationID AnimID, bool CanInterrupt = false,
			bool ForcedSet = false, bool Repeating = false);
		virtual void JWLife::SetDirection(EAnimationDirection Direction);

		virtual auto JWLife::IsBeingAnimated() const->bool;
		virtual auto JWLife::GetScaledLifeSize() const->D3DXVECTOR2;
		virtual auto JWLife::GetDirection() const->EAnimationDirection;

		// Move
		virtual void JWLife::Accelerate(D3DXVECTOR2 Accel);
		virtual void JWLife::AddVelocity(D3DXVECTOR2 Vel);
		virtual void JWLife::SetVelocity(D3DXVECTOR2 Vel);
		virtual void JWLife::MoveWithVelocity();
		virtual void JWLife::MoveConst(D3DXVECTOR2 dXY);
		virtual void JWLife::Walk(EAnimationDirection Direction);
		virtual void JWLife::Jump();
		virtual void JWLife::Gravitate();

	protected:
		void JWLife::CalculateGlobalPositionInverse();
		void JWLife::CalculateGlobalPosition();

	protected:
		static const D3DXVECTOR2 JUMP_POWER;
		static const D3DXVECTOR2 GRAVITY;
		static const float STRIDE;

		const JWMap* m_pMap;
		D3DXVECTOR2 m_GlobalPos;
		D3DXVECTOR2 m_GlobalPosInverse;
		D3DXVECTOR2 m_Velocity;
		bool m_bHitGround;

		D3DXVECTOR2 m_UnitSize;

	private:
		virtual void JWLife::SetNumRowsAndCols(POINT UnitSize, int numCols, int numRows);

	private:
		int m_SheetRows;
		int m_SheetCols;

		EAnimationDirection m_AnimDir;
		EAnimationID m_CurrAnimID;
		int m_CurrFrameID;
		VECTOR<SAnimationData> m_AnimData;
		int m_AnimCount;
		bool m_bBeingAnimated;
		bool m_bRepeating;
	};
};