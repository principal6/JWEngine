#include "JWGUI.h"

#ifdef _DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

using namespace JWENGINE;

void MainLoop();

static JWGUI myGUI;
JWGUIWindow* pMainGUIWindow;
JWGUIWindow* pNewWindow;

THandle menubar = THandle_Null;
THandleItem mb_file_new = THandle_Null;
THandleItem mb_file_open = THandle_Null;
THandleItem mb_help_info = THandle_Null;

THandle textbutton1 = THandle_Null;

// TODO: THandle, THandleItem to structure?? (To safely handle these)

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	SWindowCreationData myWindowData;
	myWindowData.x = 0;
	myWindowData.y = 100;
	myWindowData.width = 800;
	myWindowData.height = 600;
	myWindowData.color_background = DEFAULT_COLOR_LESS_BLACK;

	myGUI.Create(myWindowData);
	pMainGUIWindow = myGUI.GetMainGUIWindowPtr();

	menubar = pMainGUIWindow->AddControl(EControlType::MenuBar, D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 0));
	THandleItem mb_file = pMainGUIWindow->GetControlPtr(menubar)->AddMenuBarItem(L"파일");
		mb_file_new = pMainGUIWindow->GetControlPtr(menubar)->AddMenuBarSubItem(mb_file, L"새로 만들기");
		mb_file_open = pMainGUIWindow->GetControlPtr(menubar)->AddMenuBarSubItem(mb_file, L"열기");
	THandleItem mb_help = pMainGUIWindow->GetControlPtr(menubar)->AddMenuBarItem(L"도움말");
		mb_help_info = pMainGUIWindow->GetControlPtr(menubar)->AddMenuBarSubItem(mb_help, L"정보");
	
	textbutton1 = pMainGUIWindow->AddControl(EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"ABCDE");

	THandle textbutton2 = pMainGUIWindow->AddControl(EControlType::TextButton, D3DXVECTOR2(25, 25), D3DXVECTOR2(100, 50), L"FGHIJ");
	pMainGUIWindow->GetControlPtr(textbutton2)->ShouldUseToggleSelection(true);

	THandle scrollbar1 = pMainGUIWindow->AddControl(EControlType::ScrollBar, D3DXVECTOR2(340, 0), D3DXVECTOR2(160, 0));
	pMainGUIWindow->GetControlPtr(scrollbar1)->MakeScrollBar(EScrollBarDirection::Horizontal);
	pMainGUIWindow->GetControlPtr(scrollbar1)->SetScrollRange(2, 5);
	pMainGUIWindow->GetControlPtr(scrollbar1)->SetScrollPosition(0);

	THandle scrollbar2 = pMainGUIWindow->AddControl(EControlType::ScrollBar, D3DXVECTOR2(300, 100), D3DXVECTOR2(0, 200));
	pMainGUIWindow->GetControlPtr(scrollbar2)->MakeScrollBar(EScrollBarDirection::Vertical);
	pMainGUIWindow->GetControlPtr(scrollbar2)->SetScrollRange(10, 30);
	pMainGUIWindow->GetControlPtr(scrollbar2)->SetScrollPosition(0);

	THandle label1 = pMainGUIWindow->AddControl(EControlType::Label, D3DXVECTOR2(100, 80), D3DXVECTOR2(150, 40), L"레이블입니다!");
	pMainGUIWindow->GetControlPtr(label1)->SetTextVerticalAlignment(EVerticalAlignment::Middle);
	pMainGUIWindow->GetControlPtr(label1)->AttachScrollBar(pMainGUIWindow->GetControlPtr(scrollbar2));

	THandle imagebutton1 = pMainGUIWindow->AddControl(EControlType::ImageButton, D3DXVECTOR2(120, 0), D3DXVECTOR2(100, 50));
	pMainGUIWindow->GetControlPtr(imagebutton1)->MakeSystemArrowButton(ESystemArrowDirection::Left);

	THandle checkbox1 = pMainGUIWindow->AddControl(EControlType::CheckBox, D3DXVECTOR2(250, 0), D3DXVECTOR2(50, 50));

	THandle radiobox1 = pMainGUIWindow->AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 0), D3DXVECTOR2(0, 0));
	THandle radiobox2 = pMainGUIWindow->AddControl(EControlType::RadioBox, D3DXVECTOR2(320, 20), D3DXVECTOR2(0, 0));

	LPDIRECT3DTEXTURE9 test_texture;
	D3DXIMAGE_INFO test_texture_info;

	THandle listbox1 = pMainGUIWindow->AddControl(EControlType::ListBox, D3DXVECTOR2(360, 100), D3DXVECTOR2(200, 90));
	pMainGUIWindow->CreateTexture(L"test_atlas.png", &test_texture, &test_texture_info);
	pMainGUIWindow->GetControlPtr(listbox1)->ShouldUseImageItem(test_texture, &test_texture_info);

	pMainGUIWindow->GetControlPtr(listbox1)->AddListBoxItem(L"1. 안녕하세요?", D3DXVECTOR2(0, 64), D3DXVECTOR2(32, 32));
	pMainGUIWindow->GetControlPtr(listbox1)->AddListBoxItem(L"2. Hello.", D3DXVECTOR2(32, 64), D3DXVECTOR2(16, 16));
	pMainGUIWindow->GetControlPtr(listbox1)->AddListBoxItem(L"3. Hola.", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64));
	pMainGUIWindow->GetControlPtr(listbox1)->AddListBoxItem(L"4. こんにちは。", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64));
	pMainGUIWindow->GetControlPtr(listbox1)->AddListBoxItem(L"5. Nihao", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64));
	pMainGUIWindow->GetControlPtr(listbox1)->AddListBoxItem(L"6. Привет");
	pMainGUIWindow->GetControlPtr(listbox1)->AddListBoxItem(L"7. ...");

	THandle image1 = pMainGUIWindow->AddControl(EControlType::ImageBox, D3DXVECTOR2(600, 100), D3DXVECTOR2(140, 90));
	pMainGUIWindow->GetControlPtr(image1)
		->SetTextureAtlas(test_texture, &test_texture_info)
		->SetAtlasUV(D3DXVECTOR2(0, 64), D3DXVECTOR2(32, 32));
	pMainGUIWindow->GetControlPtr(image1)->SetSize(D3DXVECTOR2(100, 20));

	THandle edit1 = pMainGUIWindow->AddControl(EControlType::Edit, D3DXVECTOR2(100, 140), D3DXVECTOR2(80, 30));
	pMainGUIWindow->GetControlPtr(edit1)->ShouldUseMultiline(true);
	pMainGUIWindow->GetControlPtr(edit1)->ShouldUseAutomaticLineBreak(false);
	//pMainGUIWindow->GetControlPtr(edit1)->SetText(L"ABCDE\nfg\nHIJKLMNOP");
	pMainGUIWindow->GetControlPtr(edit1)->SetText(L"This is JWEdit control.\nTest it!\nThird line it is!\nAnd forth this is.");

	myGUI.SetMainLoopFunction(MainLoop);

	myGUI.Run();

	return 0;
}

void MainLoop()
{
	THandleItem clicked_subitem = THandle_Null;

	if ((clicked_subitem = pMainGUIWindow->GetControlPtr(menubar)->OnSubItemClick()) != THandle_Null)
	{
		if (clicked_subitem == mb_file_new)
		{
			std::cout << "FILE - NEW" << std::endl;

			SWindowCreationData myWindowData = SWindowCreationData(100, 100, 300, 200, DEFAULT_COLOR_LESS_BLACK);
			
			pNewWindow = myGUI.AddGUIWindow(myWindowData);

			THandle textbutton = pNewWindow->AddControl(EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"KLMNO");
			pNewWindow->GetControlPtr(textbutton)->ShouldUseToggleSelection(true);
		}
	}
	
	if (pMainGUIWindow->GetControlPtr(textbutton1)->OnMouseCliked())
	{
		std::cout << "CLICK" << std::endl;
	}
}