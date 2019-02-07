#include "JWGUI.h"
#include <crtdbg.h>

#ifdef _DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

using namespace JWENGINE;

void MainLoop();

static JWGUI myGUI;

int main()
{
	static JWWindow myWindow;

	if (JW_SUCCEEDED(myWindow.CreateGUIWindow(100, 100, 800, 600, DEFAULT_COLOR_LESS_BLACK)))
	{
		myGUI.Create(&myWindow);

		THandle tb1 = myGUI.AddControl(EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"ABCDE");
		
		THandle ib1 = myGUI.AddControl(EControlType::ImageButton, D3DXVECTOR2(120, 0), D3DXVECTOR2(100, 50));
		myGUI.GetControlPointer(ib1)->MakeSystemArrowButton(ESystemArrowDirection::Left);

		THandle checkbox1 = myGUI.AddControl(EControlType::CheckBox, D3DXVECTOR2(250, 0), D3DXVECTOR2(50, 50));

		THandle label1 = myGUI.AddControl(EControlType::Label, D3DXVECTOR2(100, 80), D3DXVECTOR2(150, 40), L"레이블입니다");
		myGUI.GetControlPointer(label1)->SetVerticalAlignment(EVerticalAlignment::Middle);

		THandle edit1 = myGUI.AddControl(EControlType::Edit, D3DXVECTOR2(100, 140), D3DXVECTOR2(160, 0),
			L"This is JWEdit control");

		THandle edit2 = myGUI.AddControl(EControlType::Edit, D3DXVECTOR2(100, 200), D3DXVECTOR2(160, 100),
			L"This is JWEdit control\nMulti-line edit");
		myGUI.GetControlPointer(edit2)->SetUseMultiline(true);

		THandle radio1 = myGUI.AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 0), D3DXVECTOR2(0, 0));
		THandle radio2 = myGUI.AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 20), D3DXVECTOR2(0, 0));

		THandle sb1 = myGUI.AddControl(EControlType::ScrollBar, D3DXVECTOR2(340, 0), D3DXVECTOR2(160, 20));
		myGUI.GetControlPointer(sb1)->MakeScrollBar(EScrollBarDirection::Horizontal);
		myGUI.GetControlPointer(sb1)->SetScrollRange(3, 5);
		myGUI.GetControlPointer(sb1)->SetScrollPosition(0);

		THandle sb2 = myGUI.AddControl(EControlType::ScrollBar, D3DXVECTOR2(340, 100), D3DXVECTOR2(20, 240));
		myGUI.GetControlPointer(sb2)->MakeScrollBar(EScrollBarDirection::Vertical);
		myGUI.GetControlPointer(sb2)->SetScrollRange(10, 30);
		myGUI.GetControlPointer(sb2)->SetScrollPosition(1);

		THandle lb1 = myGUI.AddControl(EControlType::ListBox, D3DXVECTOR2(400, 100), D3DXVECTOR2(140, 70));
		myGUI.GetControlPointer(lb1)->AddTextItem(L"1. 안녕하세요?");
		myGUI.GetControlPointer(lb1)->AddTextItem(L"2. Hello.");
		myGUI.GetControlPointer(lb1)->AddTextItem(L"3. Hola.");
		myGUI.GetControlPointer(lb1)->AddTextItem(L"4. こんにちは。");
		myGUI.GetControlPointer(lb1)->AddTextItem(L"5. Nihao");
		myGUI.GetControlPointer(lb1)->AddTextItem(L"6. Привет");
		myGUI.GetControlPointer(lb1)->AddTextItem(L"7. ...");

		myGUI.SetMainLoopFunction(MainLoop);

		myGUI.Run();
	}

	return 0;
}

void MainLoop()
{
	myGUI.DrawAllControls();
}