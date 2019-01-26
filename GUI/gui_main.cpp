#include "JWGUI.h"

using namespace JWENGINE;

int main()
{
	static JWWindow myWindow;
	static JWGUI myGUI;

	if (JW_SUCCEEDED(myWindow.CreateGUIWindow(0, 100, 800, 600, D3DCOLOR_XRGB(0, 120, 255))))
	{
		myGUI.Create(&myWindow);

		myGUI.Run();
	}

	return 0;
}