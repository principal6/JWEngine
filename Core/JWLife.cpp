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
	m_bAnimated = false;
	m_bShouldRepeatAnimation = false;

	m_UnitSize = D3DXVECTOR2(0.0f, 0.0f);

	m_GlobalPosition = D3DXVECTOR2(0.0f, 0.0f);
	m_GlobalPositionInverse = D3DXVECTOR2(0.0f, 0.0f);
	m_Velocity = D3DXVECTOR2(0.0f, 0.0f);
	m_bHitGround = true;
}

auto JWLife::Create(const JWWindow* pJWWindow, const WSTRING* pBaseDir, const JWMap* pMap)->JWLife*
{
	JWImage::Create(pJWWindow, pBaseDir);
	
	if ((m_pMap = pMap) == nullptr)
	{
		throw EError::NULLPTR_MAP;
	}

	SetGlobalPosition(m_GlobalPosition);

	return this;
}

auto JWLife::MakeLife(const WSTRING TextureFN, const POINT UnitSize, CINT numCols, CINT numRows, const float Scale)->JWLife*
{
	JWImage::SetTexture(TextureFN);
	JWImage::SetScale(D3DXVECTOR2(Scale, Scale));
	SetNumRowsAndCols(UnitSize, numCols, numRows); // m_UnitSize is set here

	JWImage::SetPosition(D3DXVECTOR2(0.0f, 0.0f));
	JWImage::SetBoundingBox(D3DXVECTOR2(0.0f, 0.0f));

	return this;
}

PRIVATE void JWLife::SetNumRowsAndCols(const POINT UnitSize, CINT numCols, CINT numRows)
{
	m_SheetCols = numCols;
	m_SheetRows = numRows;

	m_UnitSize.x = static_cast<float>(UnitSize.x);
	m_UnitSize.y = static_cast<float>(UnitSize.y);
	
	// Below we calculate automatically the unit's size IAW/ the texture size and cols and rows
	// but this way, we always adjust the unit size proportional to the texture sheet size,
	// which is an unnecessary work if we just set absolute unit size like above

	/*
	m_LifeSize.x = m_Size.x / static_cast<float>(numCols);
	m_LifeSize.y = m_Size.y / static_cast<float>(numRows);
	*/

	SetSize(m_UnitSize);
	SetFrame(0);
}

auto JWLife::AddAnimation(const SAnimationData Value)->JWLife*
{
	int AnimIDInt = static_cast<int>(Value.AnimID);

	m_AnimData.push_back(Value);

	// Pair AnimID with m_AnimData's index
	m_AnimDataMap.insert(std::make_pair(AnimIDInt, m_AnimData.size() - 1));

	return this;
}

void JWLife::SetAnimation(const EAnimationID AnimID, const bool bForceSet, const bool bShouldRepeat)
{
	// If AnimID is not in the m_AnimData, then exit the function
	auto iterator_AnimDataMap = m_AnimDataMap.find(static_cast<int>(AnimID));
	if (iterator_AnimDataMap == m_AnimDataMap.end())
		return;

	size_t AnimDataIndex = iterator_AnimDataMap->second;
	bool bShouldSetAnimation = false;
	bool bShouldSetStartFrame = false;

	if (m_bAnimated)
	{
		// It was animated before
		if (m_CurrAnimID != AnimID)
		{
			// we're setting a new animation
			// then, we should set animation!
			bShouldSetAnimation = true;
			bShouldSetStartFrame = true;
		}
		else
		{
			// we're setting the same animation as the previous one
			bShouldSetAnimation = true;
			bShouldSetStartFrame = m_AnimData[m_CurrAnimDataIndex].bSetStartFrameEverytime;
		}
	}
	else
	{
		// It wasn't animated before
		if (m_CurrAnimID != AnimID)
		{
			// we're setting a new animation
			if (m_AnimData[m_CurrAnimDataIndex].bForceCycle)
			{
				// but the previous animation has a forced cycle
				// so, ignore this SetAnimation()
				bShouldSetAnimation = false;

				if (bForceSet)
				{
					// But this SetAnimation() is forced
					// then should set animation!
					bShouldSetAnimation = true;
					bShouldSetStartFrame = true;
				}
			}
			else
			{
				// the previous animation doesn't have its cycle forced
				// then should set animation!
				bShouldSetAnimation = true;
				bShouldSetStartFrame = true;
			}
		}
	}

	if (bShouldSetAnimation)
	{
		m_CurrAnimID = AnimID;
		m_CurrAnimDataIndex = AnimDataIndex;

		if (bShouldSetStartFrame)
		{
			m_CurrFrameID = m_AnimData[AnimDataIndex].FrameS;
		}

		m_bShouldRepeatAnimation = bShouldRepeat;
		m_bAnimated = false;

		SetFrame(m_CurrFrameID);
	}
}

PRIVATE void JWLife::SetFrame(CINT FrameID)
{
	if ((m_SheetRows == 0) || (m_SheetCols == 0))
		return;

	STextureUV tUV;
	ConvertFrameIDIntoUV(FrameID, m_UnitSize, m_AtlasSize, m_SheetCols, m_SheetRows, &tUV);

	switch (m_AnimDir)
	{
	case EAnimationDirection::Left:
		// Set the UV with the u value flipped
		Swap(tUV.u1, tUV.u2);
		UpdateVertexData(tUV);
		break;
	case EAnimationDirection::Right:
		UpdateVertexData(tUV);
		break;
	default:
		break;
	}
}

void JWLife::Animate()
{
	if (m_bAnimated)
	{
		// The previous animation is ended
		// and no animation is set, then set it to Idle
		SetAnimation(EAnimationID::Idle);
	}
	else
	{
		if (m_CurrFrameID < m_AnimData[m_CurrAnimDataIndex].FrameE)
		{
			m_CurrFrameID++;

			if (!m_AnimData[m_CurrAnimDataIndex].bForceCycle)
			{
				m_bAnimated = true;
			}
			else
			{
				m_bAnimated = false;
			}
		}
		else
		{
			// Animation cycle ended
			m_CurrFrameID = m_AnimData[m_CurrAnimDataIndex].FrameS;

			if (!m_bShouldRepeatAnimation)
				m_bAnimated = true;
		}
	}
	
	SetFrame(m_CurrFrameID);
}

void JWLife::SetDirection(EAnimationDirection Direction)
{
	m_AnimDir = Direction;
}

auto JWLife::GetDirection() const->const EAnimationDirection
{
	return m_AnimDir;
}

void JWLife::CalculateGlobalPositionInverse()
{
	m_GlobalPositionInverse = m_GlobalPosition;
	m_GlobalPositionInverse.y = m_pJWWindow->GetWindowData()->WindowHeight - m_ScaledSize.y - m_GlobalPosition.y;
}

void JWLife::CalculateGlobalPosition()
{
	m_GlobalPosition = m_GlobalPositionInverse;
	m_GlobalPosition.y = m_pJWWindow->GetWindowData()->WindowHeight - m_ScaledSize.y - m_GlobalPositionInverse.y;
}

auto JWLife::SetGlobalPosition(const D3DXVECTOR2 Position)->JWLife*
{
	m_GlobalPosition = Position;
	CalculateGlobalPositionInverse();

	if (m_GlobalPositionInverse.x > m_pJWWindow->GetWindowData()->WindowHalfWidth)
		m_GlobalPositionInverse.x = m_pJWWindow->GetWindowData()->WindowHalfWidth;

	if (m_GlobalPositionInverse.y < m_pJWWindow->GetWindowData()->WindowHalfHeight)
		m_GlobalPositionInverse.y = m_pJWWindow->GetWindowData()->WindowHalfHeight;

	SetPosition(m_GlobalPositionInverse);

	return this;
}

auto JWLife::GetGlobalPosition() const->const D3DXVECTOR2
{
	return m_GlobalPosition;
}

auto JWLife::GetGlobalPositionInverse() const->const D3DXVECTOR2
{
	return m_GlobalPositionInverse;
}

auto JWLife::GetVelocity() const->const D3DXVECTOR2
{
	return m_Velocity;
}

auto JWLife::GetOffsetForMapMove() const->const D3DXVECTOR2
{
	D3DXVECTOR2 Result;
	Result.x = m_GlobalPosition.x - m_pJWWindow->GetWindowData()->WindowHalfWidth;
	Result.y = m_GlobalPositionInverse.y - m_pJWWindow->GetWindowData()->WindowHalfHeight;

	if (Result.x < 0)
		Result.x = 0;

	if (Result.y > 0)
		Result.y = 0;

	return Result;
}

auto JWLife::GetScaledUnitSize() const->const D3DXVECTOR2
{
	return m_ScaledSize;
}

void JWLife::Accelerate(const D3DXVECTOR2 Accel)
{
	m_Velocity += Accel;
}

void JWLife::AddVelocity(const D3DXVECTOR2 Vel)
{
	m_Velocity += Vel;
}

void JWLife::SetVelocity(const D3DXVECTOR2 Vel)
{
	m_Velocity = Vel;
}

void JWLife::MoveWithVelocity()
{
	m_GlobalPosition.x += m_Velocity.x;
	m_GlobalPosition.y -= m_Velocity.y; // @warning: Y value of GlobalPos is in the opposite direction!

	CalculateGlobalPositionInverse();

	if (m_GlobalPositionInverse.x < m_pJWWindow->GetWindowData()->WindowHalfWidth)
	{
		m_Position.x = m_GlobalPosition.x;
	}
	if (m_GlobalPositionInverse.y > m_pJWWindow->GetWindowData()->WindowHalfHeight)
	{
		m_Position.y = m_GlobalPositionInverse.y;
	}

	SetPosition(m_Position);
}

void JWLife::MoveConst(const D3DXVECTOR2 dXY)
{
	m_GlobalPosition.x += dXY.x;
	m_GlobalPosition.y -= dXY.y; // @warning: Y value of GlobalPos is in the opposite direction!

	CalculateGlobalPositionInverse();

	if (m_GlobalPositionInverse.x < m_pJWWindow->GetWindowData()->WindowHalfWidth)
	{
		m_Position.x = m_GlobalPosition.x;
	}
	else
	{
		if (m_Position.x != m_pJWWindow->GetWindowData()->WindowHalfWidth)
			m_Position.x = m_pJWWindow->GetWindowData()->WindowHalfWidth;
	}

	if (m_GlobalPositionInverse.y > m_pJWWindow->GetWindowData()->WindowHalfHeight)
	{
		m_Position.y = m_GlobalPositionInverse.y;
	}
	else
	{
		if (m_Position.y != m_pJWWindow->GetWindowData()->WindowHalfHeight)
			m_Position.y = m_pJWWindow->GetWindowData()->WindowHalfHeight;
	}

	SetPosition(m_Position);
}

void JWLife::Walk(const EAnimationDirection Direction)
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

	SetAnimation(EAnimationID::Walk);
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
		}
	}
	else
	{
		if (m_Velocity.y < 0)
		{
			if ((m_CurrAnimID == EAnimationID::Idle) || (m_CurrAnimID == EAnimationID::Walk))
			{
				// Idle or Walk is ignored when jumping
				SetAnimation(EAnimationID::Jumping, true);
			}
		}
			

		if (m_Velocity.y > 0)
		{
			if ((m_CurrAnimID == EAnimationID::Idle) || (m_CurrAnimID == EAnimationID::Walk))
			{
				// Idle or Walk is ignored when falling
				SetAnimation(EAnimationID::Falling, true);
			}
		}
		
		m_bHitGround = false;
	}

	SetVelocity(tNewVel);
	MoveWithVelocity();
}