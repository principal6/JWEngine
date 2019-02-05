#include "JWGUI.h"

using namespace JWENGINE;

void MainLoop();

static JWGUI myGUI;

int main()
{
	static JWWindow myWindow;

	if (JW_SUCCEEDED(myWindow.CreateGUIWindow(0, 100, 800, 600, DEFAULT_COLOR_BACKGROUND)))
	{
		myGUI.Create(&myWindow);

		myGUI.AddControl(EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"ABCDE");
		
		myGUI.AddControl(EControlType::ImageButton, D3DXVECTOR2(120, 0), D3DXVECTOR2(100, 50));
		myGUI.GetControlPointer()->MakeSystemArrowButton(ESystemArrowDirection::Left);

		myGUI.AddControl(EControlType::CheckBox, D3DXVECTOR2(250, 0), D3DXVECTOR2(50, 50));

		myGUI.AddControl(EControlType::Label, D3DXVECTOR2(100, 80), D3DXVECTOR2(150, 40), L"레이블입니다");

		myGUI.AddControl(EControlType::Edit, D3DXVECTOR2(100, 140), D3DXVECTOR2(160, 0),
			L"This is JWEdit control");

		myGUI.AddControl(EControlType::Edit, D3DXVECTOR2(100, 200), D3DXVECTOR2(160, 100),
			L"This is JWEdit control\nMulti-line edit");
		myGUI.GetControlPointer()->SetUseMultiline(true);

		myGUI.AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 0), D3DXVECTOR2(0, 0));
		myGUI.AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 20), D3DXVECTOR2(0, 0));

		myGUI.AddControl(EControlType::ScrollBar, D3DXVECTOR2(340, 0), D3DXVECTOR2(160, 20));
		myGUI.GetControlPointer()->MakeScrollBar(EScrollBarDirection::Horizontal);
		myGUI.GetControlPointer()->SetScrollRange(3);
		myGUI.GetControlPointer()->SetScrollPosition(0);

		myGUI.AddControl(EControlType::ScrollBar, D3DXVECTOR2(340, 100), D3DXVECTOR2(20, 160));
		myGUI.GetControlPointer()->MakeScrollBar(EScrollBarDirection::Vertical);
		myGUI.GetControlPointer()->SetScrollRange(30);
		myGUI.GetControlPointer()->SetScrollPosition(1);

		myGUI.SetMainLoopFunction(MainLoop);

		myGUI.Run();
	}

	return 0;
}

void MainLoop()
{

}