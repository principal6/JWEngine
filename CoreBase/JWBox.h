#pragma once

#include "JWCommon.h"
#include "JWLine.h"

namespace JWENGINE
{
	// ***
	// *** Forward declaration ***
	class JWWindow;
	// ***

	class JWBox
	{
		JWBox();
		virtual ~JWBox() {};

		virtual auto Create(JWWindow* pJWWindow, WSTRING BaseDir)->EError;
		virtual void Destroy();

		virtual void Draw();
	};
};