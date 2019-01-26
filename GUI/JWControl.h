#pragma once

#include "../CoreBase/JWCommon.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	// ***

	class JWControl
	{
	public:
		JWControl() {};
		virtual ~JWControl() {};

		virtual auto Create(JWWindow* pWindow, WSTRING BaseDir)->EError;
		virtual void Destroy();

	protected:
		static WSTRING ms_BaseDir;
		static JWWindow* ms_pWindow;
	};
};