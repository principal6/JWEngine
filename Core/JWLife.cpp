#include "../CoreBase/JWWindow.h"
#include "JWLife.h"
#include "JWMap.h"

using namespace JWENGINE;

// Static member variables declaration
const D3DXVECTOR2 JWLife::JUMP_POWER = D3DXVECTOR2(0.0f, -14.0f);
const D3DXVECTOR2 JWLife::GRAVITY = D3DXVECTOR2(0.0f, 0.5f);
const float JWLife::STRIDE = 5.0f;

JWLife::JWLife()
{
	m_SheetRows = 0;
	m_SheetCols = 0;

	m_AnimDir = EAnimationDirection::Right;
	m_CurrAnimID = EAnimationID::Idle;
	m_CurrFrameID = 0;
	m_AnimCount = 0;
	m_bBeingAnimated = false;
	m_bRepeating = false;

	m_UnitSize = D3DXVECTOR2(0.0f, 0.0f);

	m_GlobalPos = D3DXVECTOR2(0.0f, 0.0f);
	m_GlobalPosInverse = D3DXVECTOR2(0.0f, 0.0f);
	m_Velocity = D3DXVECTOR2(0.0f, 0.0f);
	m_bHitGround = true;
}

auto JWLife::Create(JWWindow* pJWWindow, WSTRING BaseDir, JWMap* pMap)->EError
{
	if (JW_SUCCEEDED(JWImage::Create(pJWWindow, BaseDir)))
	{
		if (nullptr == (m_pMap = pMap))
			return EError::NULLPTR_MAP;

		SetGlobalPosition(m_GlobalPos);

		return EError::OK;
	}

	return EError::IMAGE_NOT_CREATED;
}

auto JWLife::MakeLife(WSTRING TextureFN, POINT UnitSize, int numCols, int numRows, float Scale)->JWLife*
{
	JWImage::SetTexture(TextureFN);
	JWImage::SetScale(D3DXVECTOR2(Scale, Scale));
	SetNumRowsAndCols(UnitSize, numCols, numRows); // m_UnitSize is set here

	JWImage::SetPosition(D3DXVECTOR2(0.0f, 0.0f));
	JWImage::SetBoundingBox(D3DXVECTOR2(0.0f, 0.0f));

	return this;
}

PRIVATE void JWLife::SetNumRowsAndCols(POINT UnitSize, int numCols, int numRows)
{
	m_SheetCols = numCols;
	m_SheetRows = numRows;

	m_UnitSize.x = static_cast<float>(UnitSize.x);
	m_UnitSize.y = static_cast<float>(UnitSize.y);
	
	// Below we calculate automatically the life's size IAW/ the texture size and cols and rows
	// but this way, we always adjust the unit size proportional to the texture sheet size,
	// which is an unnecessary work if we just set absolute unit size like above

	/*
	m_LifeSize.x = m_Size.x / static_cast<float>(numCols);
	m_LifeSize.y = m_Size.y / static_cast<float>(numRows);
	*/

	SetSize(m_UnitSize);
	SetFrame(0);
}

void JWLife::SetFrame(int FrameID)
{
	if ((m_SheetRows == 0) || (m_SheetCols == 0))
		return;

	STextureUV tUV;
	ConvertFrameIDIntoUV(FrameID, m_UnitSize, m_AtlasSize, m_SheetCols, m_SheetRows, &tUV);

	switch (m_AnimDir)
	{
	case EAnimationDirection::Left:
		// Set the UV with the u value flipped
		UpdateVertexData(tUV.u2, tUV.v1, tUV.u1, tUV.v2);
		break;
	case EAnimationDirection::Right:
		UpdateVertexData(tUV.u1, tUV.v1, tUV.u2, tUV.v2);
		break;
	default:
		break;
	}
}

auto JWLife::AddAnimation(EAnimationID AnimID, int StartFrame, int EndFrame)->JWLife*
{
	m_AnimData.push_back(SAnimationData(AnimID, StartFrame, EndFrame));

	return this;
}

void JWLife::SetAnimation(EAnimationID AnimID, bool bCanInterrupt, bool bForcedSet, bool bRepeating)
{
	int AnimIDInt = static_cast<int>(AnimID);

	if (AnimIDInt > m_AnimData.size())
		return;

	if ((m_CurrAnimID != AnimID) || (bForcedSet))
	{
		m_CurrAnimID = AnimID;
		m_CurrFrameID = m_AnimData[AnimIDInt].FrameS;
		m_bRepeating = bRepeating;
		m_bBeingAnimated = !bCanInterrupt;

		SetFrame(m_CurrFrameID);
	}
}

void JWLife::Animate()
{
	if (m_CurrFrameID < m_AnimData[static_cast<int>(m_CurrAnimID)].FrameE)
	{
		m_CurrFrameID++;
	}
	else
	{
		m_CurrFrameID = m_AnimData[static_cast<int>(m_CurrAnimID)].FrameS;
		if (!m_bRepeating)
			m_bBeingAnimated = false;
	}

	SetFrame(m_CurrFrameID);
}

void JWLife::SetDirection(EAnimationDirection Direction)
{
	m_AnimDir = Direction;
}

auto JWLife::IsBeingAnimated() const->bool
{
	return m_bBeingAnimated;
}

auto JWLife::GetScaledLifeSize() const->D3DXVECTOR2
{
	return m_ScaledSize;
}

auto JWLife::GetDirection() const->EAnimationDirection
{
	return m_AnimDir;
}

void JWLife::CalculateGlobalPositionInverse()
{
	m_GlobalPosInverse = m_GlobalPos;
	m_GlobalPosInverse.y = m_pJWWindow->GetWindowData()->WindowHeight - m_ScaledSize.y - m_GlobalPos.y;
}

void JWLife::CalculateGlobalPosition()
{
	m_GlobalPos = m_GlobalPosInverse;
	m_GlobalPos.y = m_pJWWindow->GetWindowData()->WindowHeight - m_ScaledSize.y - m_GlobalPosInverse.y;
}

auto JWLife::SetGlobalPosition(D3DXVECTOR2 Position)->JWLife*
{
	m_GlobalPos = Position;
	CalculateGlobalPositionInverse();

	if (m_GlobalPosInverse.x > m_pJWWindow->GetWindowData()->WindowHalfWidth)
		m_GlobalPosInverse.x = m_pJWWindow->GetWindowData()->WindowHalfWidth;

	if (m_GlobalPosInverse.y < m_pJWWindow->GetWindowData()->WindowHalfHeight)
		m_GlobalPosInverse.y = m_pJWWindow->GetWindowData()->WindowHalfHeight;

	SetPosition(m_GlobalPosInverse);

	return this;
}

auto JWLife::GetGlobalPosition() const->D3DXVECTOR2
{
	return m_GlobalPos;
}

auto JWLife::GetGlobalPositionInverse() const->D3DXVECTOR2
{
	return m_GlobalPosInverse;
}

auto JWLife::GetVelocity() const->D3DXVECTOR2
{
	return m_Velocity;
}

auto JWLife::GetOffsetForMapMove() const->D3DXVECTOR2
{
	D3DXVECTOR2 Result;
	Result.x = m_GlobalPos.x - m_pJWWindow->GetWindowData()->WindowHalfWidth;
	Result.y = m_GlobalPosInverse.y - m_pJWWindow->GetWindowData()->WindowHalfHeight;

	if (Result.x < 0)
		Result.x = 0;

	if (Result.y > 0)
		Result.y = 0;

	return Result;
}

void JWLife::Accelerate(D3DXVECTOR2 Accel)
{
	m_Velocity += Accel;
}

void JWLife::AddVelocity(D3DXVECTOR2 Vel)
{
	m_Velocity += Vel;
}

void JWLife::SetVelocity(D3DXVECTOR2 Vel)
{
	m_Velocity = Vel;
}

void JWLife::MoveWithVelocity()
{
	m_GlobalPos.x += m_Velocity.x;
	m_GlobalPos.y -= m_Velocity.y; // @warning: Y value of GlobalPos is in the opposite direction!

	CalculateGlobalPositionInverse();

	if (m_GlobalPosInverse.x < m_pJWWindow->GetWindowData()->WindowHalfWidth)
	{
		m_Position.x = m_GlobalPos.x;
	}
	if (m_GlobalPosInverse.y > m_pJWWindow->GetWindowData()->WindowHalfHeight)
	{
		m_Position.y = m_GlobalPosInverse.y;
	}

	SetPosition(m_Position);
}

void JWLife::MoveConst(D3DXVECTOR2 dXY)
{
	m_GlobalPos.x += dXY.x;
	m_GlobalPos.y -= dXY.y; // @warning: Y value of GlobalPos is in the opposite direction!

	CalculateGlobalPositionInverse();

	if (m_GlobalPosInverse.x < m_pJWWindow->GetWindowData()->WindowHalfWidth)
	{
		m_Position.x = m_GlobalPos.x;
	}
	else
	{
		if (m_Position.x != m_pJWWindow->GetWindowData()->WindowHalfWidth)
			m_Position.x = m_pJWWindow->GetWindowData()->WindowHalfWidth;
	}

	if (m_GlobalPosInverse.y > m_pJWWindow->GetWindowData()->WindowHalfHeight)
	{
		m_Position.y = m_GlobalPosInverse.y;
	}
	else
	{
		if (m_Position.y != m_pJWWindow->GetWindowData()->WindowHalfHeight)
			m_Position.y = m_pJWWindow->GetWindowData()->WindowHalfHeight;
	}

	SetPosition(m_Position);
}

void JWLife::Walk(EAnimationDirection Direction)
{
	D3DXVECTOR2 Velocity;
	switch (Direction)
	{
	case EAnimationDirection::Left:
		Velocity = D3DXVECTOR2(-STRIDE, 0);
		break;
	case EAnimationDirection::Right:
		Velocity = D3DXVECTOR2(STRIDE, 0);
		break;
	default:
		break;
	}

	SetAnimation(EAnimationID::Walk, true);
	SetDirection(Direction);

	D3DXVECTOR2 tNewVel = m_pMap->GetVelocityAfterCollision(GetBoundingBox(), Velocity);

	MoveConst(tNewVel);
}

void JWLife::Jump()
{
	if ((m_bHitGround == false) || (m_Velocity.y > 0)) // Currently the sprite is falling down
		return;

	m_bHitGround = false;

	D3DXVECTOR2 tNewVel = m_pMap->GetVelocityAfterCollision(GetBoundingBox(), JUMP_POWER);
	SetVelocity(tNewVel);
}

void JWLife::Gravitate()
{
	Accelerate(GRAVITY);

	D3DXVECTOR2 tNewVel = m_pMap->GetVelocityAfterCollision(GetBoundingBox(), m_Velocity);

	if (tNewVel.y < m_Velocity.y)
	{
		if (m_bHitGround)
		{
			tNewVel.y = 0;
		}
		else
		{
			m_bHitGround = true;
			
			// This is only for main Sprite
			/*
			SetAnimation(AnimationID::Landing, true, true);
			*/

			SetAnimation(EAnimationID::Idle, true, true);
		}
	}
	else
	{
		if (m_Velocity.y < 0)
		{
			if ((m_CurrAnimID == EAnimationID::Idle) || (m_CurrAnimID == EAnimationID::Walk))
			{
				// Idle or Walk is ignored when jumping
				SetAnimation(EAnimationID::Jumping, true, false);
			}
		}
			

		if (m_Velocity.y > 0)
		{
			if ((m_CurrAnimID == EAnimationID::Idle) || (m_CurrAnimID == EAnimationID::Walk))
			{
				// Idle or Walk is ignored when falling
				SetAnimation(EAnimationID::Falling, true, false);
			}
		}
		
		m_bHitGround = false;
	}

	SetVelocity(tNewVel);
	MoveWithVelocity();
}