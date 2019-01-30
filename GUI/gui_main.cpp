#include "JWGUI.h"

using namespace JWENGINE;

void MainLoop();

static JWGUI myGUI;

int main()
{
	static JWWindow myWindow;

	if (JW_SUCCEEDED(myWindow.CreateGUIWindow(0, 100, 800, 600, D3DCOLOR_XRGB(0, 120, 255))))
	{
		myGUI.Create(&myWindow);

		myGUI.AddControl(EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"ABCDE");
			
		myGUI.AddControl(EControlType::ImageButton, D3DXVECTOR2(120, 0), D3DXVECTOR2(100, 50));
		myGUI.GetControlPointer(1)->SetImageButtonDireciton(EImageButtonDirection::Right);
		
		myGUI.AddControl(EControlType::CheckBox, D3DXVECTOR2(250, 0), D3DXVECTOR2(50, 50));

		myGUI.AddControl(EControlType::Label, D3DXVECTOR2(100, 80), D3DXVECTOR2(150, 50), L"레이블입니다");

		myGUI.AddControl(EControlType::Edit, D3DXVECTOR2(100, 120), D3DXVECTOR2(150, 60), L"This is my custom edit control");

		myGUI.SetMainLoopFunction(MainLoop);

		myGUI.Run();
	}

	return 0;
}

void MainLoop()
{

}