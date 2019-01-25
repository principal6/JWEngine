#include "JWGUI.h"

using namespace JWENGINE;

int main()
{
	static JWGUI myGUI;

	myGUI.Create();

	myGUI.Run();
	
	return 0;
}