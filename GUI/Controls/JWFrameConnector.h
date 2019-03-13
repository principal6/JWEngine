#pragma once

#include "JWControl.h"

namespace JWENGINE
{
	class JWFrameConnector final : public JWControl
	{
	friend class JWFrame;

	public:
		JWFrameConnector() {};
		~JWFrameConnector() {};

		// @warning: 'Position' will be ignored in JWFrameConnector's Create()
		void Create(const D3DXVECTOR2& Position, const D3DXVECTOR2& Size, const SGUIWindowSharedData& SharedData) noexcept override;

		auto MakeVerticalConnector(JWControl& LeftFrame, JWControl& RightFrame)->JWControl& override;

		auto MakeHorizontalConnector(JWControl& UpFrame, JWControl& DownFrame)->JWControl& override;

		void UpdateControlState(JWControl** ppControlWithMouse, JWControl** ppControlWithFocus) noexcept override;

		void Draw() noexcept override;

		// @warning: JWFrameConnector must make SetPosition meaningless!
		auto SetPosition(const D3DXVECTOR2& Position) noexcept->JWControl& override;

		auto SetSize(const D3DXVECTOR2& Size) noexcept->JWControl& override;

		auto GetFrameConnectorType() const noexcept->EFrameConnectorType override;

	protected:
		void UpdateFrameConectorPositionAndSize() noexcept override;

		void SetSizableCursor() noexcept;

	private:
		JWImage m_Background;

		EFrameConnectorType m_FrameConnectorType{ EFrameConnectorType::Invalid };

		JWControl* m_pFrameA{ nullptr };
		JWControl* m_pFrameB{ nullptr };

		bool m_IsBeingCaptured{ false };
		D3DXVECTOR2 m_CapturedPosition{};
		POINT m_CapturedMouseDownPosition{};
	};
};