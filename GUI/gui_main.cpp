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
		myGUI.GetControlPointer(1)->MakeSystemArrowButton(ESystemArrowDirection::Left);

		myGUI.AddControl(EControlType::CheckBox, D3DXVECTOR2(250, 0), D3DXVECTOR2(50, 50));

		myGUI.AddControl(EControlType::Label, D3DXVECTOR2(100, 80), D3DXVECTOR2(150, 50), L"레이블입니다");

		myGUI.AddControl(EControlType::Edit, D3DXVECTOR2(100, 120), D3DXVECTOR2(160, 300),
			L"This is JWEdit control");
		myGUI.GetControlPointer(4)->SetUseMultiline(true);

		myGUI.AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 0), D3DXVECTOR2(0, 0));
		myGUI.AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 20), D3DXVECTOR2(0, 0));

		myGUI.AddControl(EControlType::ScrollBar, D3DXVECTOR2(340, 0), D3DXVECTOR2(160, 20));
		myGUI.GetControlPointer(7)->MakeScrollBar(EScrollBarDirection::Horizontal);
		myGUI.GetControlPointer(7)->SetScrollRange(10);
		myGUI.GetControlPointer(7)->SetScrollPosition(0);

		myGUI.AddControl(EControlType::ScrollBar, D3DXVECTOR2(340, 100), D3DXVECTOR2(20, 160));
		myGUI.GetControlPointer(8)->MakeScrollBar(EScrollBarDirection::Vertical);
		myGUI.GetControlPointer(8)->SetScrollRange(10);
		myGUI.GetControlPointer(8)->SetScrollPosition(1);

		myGUI.SetMainLoopFunction(MainLoop);

		myGUI.Run();
	}

	return 0;
}

void MainLoop()
{

}