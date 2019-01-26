#include "JWControl.h"
#include "../CoreBase/JWWindow.h"

using namespace JWENGINE;

// Static member variable
WSTRING JWControl::ms_BaseDir;
JWWindow* JWControl::ms_pWindow;

auto JWControl::Create(JWWindow* pWindow, WSTRING BaseDir)->EError
{
	if (nullptr == (ms_pWindow = pWindow))
		return EError::NULLPTR_WINDOW;

	ms_BaseDir = BaseDir;

	return EError::OK;
}

void JWControl::Destroy()
{

}