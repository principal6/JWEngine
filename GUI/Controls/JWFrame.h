#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWFrame final : public JWControl
	{
	public:
		JWFrame() {};
		~JWFrame() {};

		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		auto AddChildControl(JWControl& ChildControl) noexcept->JWControl& override;

		void Draw() noexcept override;

		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl& override;
		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl& override;

	protected:
		// Must be overridden.
		void UpdateViewport() noexcept;

		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept override;

		// This function is called in JWFrameConnector class.
		auto SetFrameConnector(JWControl& Connector, bool IsBasisFrame) noexcept->JWControl& override;

		void SetFramePositionFromInside(const D3DXVECTOR2& Position) noexcept override;
		void SetFrameSizeFromInside(const D3DXVECTOR2& Size) noexcept override;

	private:
		JWImage m_Background;
		
		VECTOR<JWControl*> m_pChildControls;

		JWControl* m_pFrameConnector{ nullptr };

		bool m_IsBasisFrame{ false };
	};
};