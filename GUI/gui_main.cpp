#include "JWGUI.h"
#include <crtdbg.h>

#ifdef _DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

using namespace JWENGINE;

void MainLoop();

static JWGUI myGUI;

THandle mb = THandle_Null;
THandleItem mb_file_new;
THandleItem mb_file_open;
THandleItem mb_help_info;

int main()
{
	myGUI.Create(100, 100, 800, 600, DEFAULT_COLOR_LESS_BLACK);

	mb = myGUI.AddControl(EControlType::MenuBar, D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 0));
	THandleItem mb_file = myGUI.GetControlPtr(mb)->AddMenuBarItem(L"파일");
	mb_file_new = myGUI.GetControlPtr(mb)->AddMenuBarSubItem(mb_file, L"새로 만들기");
	mb_file_open = myGUI.GetControlPtr(mb)->AddMenuBarSubItem(mb_file, L"열기");
	THandleItem mb_help = myGUI.GetControlPtr(mb)->AddMenuBarItem(L"도움말");
	mb_help_info = myGUI.GetControlPtr(mb)->AddMenuBarSubItem(mb_help, L"정보");

	THandle tb1 = myGUI.AddControl(EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"ABCDE");
		
	THandle ib1 = myGUI.AddControl(EControlType::ImageButton, D3DXVECTOR2(120, 0), D3DXVECTOR2(100, 50));
	myGUI.GetControlPtr(ib1)->MakeSystemArrowButton(ESystemArrowDirection::Left);

	THandle checkbox1 = myGUI.AddControl(EControlType::CheckBox, D3DXVECTOR2(250, 0), D3DXVECTOR2(50, 50));

	THandle label1 = myGUI.AddControl(EControlType::Label, D3DXVECTOR2(100, 80), D3DXVECTOR2(150, 40), L"레이블입니다!\n두 줄!");
	myGUI.GetControlPtr(label1)->SetUseMultiline(true);
	myGUI.GetControlPtr(label1)->SetVerticalAlignment(EVerticalAlignment::Middle);

	THandle edit1 = myGUI.AddControl(EControlType::Edit, D3DXVECTOR2(100, 140), D3DXVECTOR2(160, 0),
		L"This is JWEdit control");

	THandle edit2 = myGUI.AddControl(EControlType::Edit, D3DXVECTOR2(100, 200), D3DXVECTOR2(160, 100),
		L"This is JWEdit control\nMulti-line edit");
	myGUI.GetControlPtr(edit2)->SetUseMultiline(true);

	THandle radio1 = myGUI.AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 0), D3DXVECTOR2(0, 0));
	THandle radio2 = myGUI.AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 20), D3DXVECTOR2(0, 0));

	THandle sb1 = myGUI.AddControl(EControlType::ScrollBar, D3DXVECTOR2(340, 0), D3DXVECTOR2(160, 0));
	myGUI.GetControlPtr(sb1)->MakeScrollBar(EScrollBarDirection::Horizontal);
	myGUI.GetControlPtr(sb1)->SetScrollRange(2, 5);
	myGUI.GetControlPtr(sb1)->SetScrollPosition(0);

	THandle sb2 = myGUI.AddControl(EControlType::ScrollBar, D3DXVECTOR2(300, 100), D3DXVECTOR2(0, 200));
	myGUI.GetControlPtr(sb2)->MakeScrollBar(EScrollBarDirection::Vertical);
	myGUI.GetControlPtr(sb2)->SetScrollRange(10, 30);
	myGUI.GetControlPtr(sb2)->SetScrollPosition(0);

	LPDIRECT3DTEXTURE9 test_texture;
	D3DXIMAGE_INFO test_texture_info;

	THandle lb1 = myGUI.AddControl(EControlType::ListBox, D3DXVECTOR2(360, 100), D3DXVECTOR2(200, 90));
	myGUI.CreateTexture(L"test_atlas.png", &test_texture, &test_texture_info);
	myGUI.GetControlPtr(lb1)->UseImageItem(test_texture, &test_texture_info);

	myGUI.GetControlPtr(lb1)->AddListBoxItem(L"1. 안녕하세요?", D3DXVECTOR2(0, 64), D3DXVECTOR2(32, 32));
	myGUI.GetControlPtr(lb1)->AddListBoxItem(L"2. Hello.", D3DXVECTOR2(32, 64), D3DXVECTOR2(16, 16));
	myGUI.GetControlPtr(lb1)->AddListBoxItem(L"3. Hola.", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64));
	myGUI.GetControlPtr(lb1)->AddListBoxItem(L"4. こんにちは。", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64));
	myGUI.GetControlPtr(lb1)->AddListBoxItem(L"5. Nihao", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64));
	myGUI.GetControlPtr(lb1)->AddListBoxItem(L"6. Привет");
	myGUI.GetControlPtr(lb1)->AddListBoxItem(L"7. ...");

	THandle image1 = myGUI.AddControl(EControlType::ImageBox, D3DXVECTOR2(600, 100), D3DXVECTOR2(140, 90));
	myGUI.GetControlPtr(image1)->SetTextureAtlas(test_texture, &test_texture_info);
	myGUI.GetControlPtr(image1)->SetAtlasUV(D3DXVECTOR2(0, 64), D3DXVECTOR2(32, 32));
	myGUI.GetControlPtr(image1)->SetSize(D3DXVECTOR2(100, 20));

	myGUI.SetMainLoopFunction(MainLoop);

	myGUI.Run();

	return 0;
}

void MainLoop()
{
	THandleItem clicked_subitem = THandle_Null;
	if ((clicked_subitem = myGUI.GetControlPtr(mb)->OnSubItemClick()) != THandle_Null)
	{
		if (clicked_subitem == mb_file_new)
		{
			std::cout << "FILE - NEW" << std::endl;
		}
	}

	myGUI.DrawAllControls();
}