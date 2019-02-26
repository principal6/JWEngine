#include "JWGUI.h"

#ifdef _DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

using namespace JWENGINE;

void MainLoop();
void ShowDialogueNewMap();

static JWGUI myGUI;
JWGUIWindow* pMainGUIWindow;
JWGUIWindow* pDialogueNewMap;

THandleItem mb_file_new = THandleItem_Null;
THandleItem mb_file_open = THandleItem_Null;
THandleItem mb_help_info = THandleItem_Null;

// TODO: THandleItem to structure?? (To safely handle these)

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	SWindowCreationData myWindowData = SWindowCreationData(L"JWGUI Test Window", 0, 100, 800, 600, DEFAULT_COLOR_LESS_BLACK);
	myGUI.Create(myWindowData, &pMainGUIWindow);

	pMainGUIWindow->AddControl(L"menubar", EControlType::MenuBar, D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 0));
	THandleItem mb_file = pMainGUIWindow->GetControlPtr(L"menubar")->AddMenuBarItem(L"파일");
		mb_file_new = pMainGUIWindow->GetControlPtr(L"menubar")->AddMenuBarSubItem(mb_file, L"새로 만들기");
		mb_file_open = pMainGUIWindow->GetControlPtr(L"menubar")->AddMenuBarSubItem(mb_file, L"열기");
	THandleItem mb_help = pMainGUIWindow->GetControlPtr(L"menubar")->AddMenuBarItem(L"도움말");
		mb_help_info = pMainGUIWindow->GetControlPtr(L"menubar")->AddMenuBarSubItem(mb_help, L"정보");
	
	pMainGUIWindow->AddControl(L"textbutton1", EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"Normal button");

	pMainGUIWindow->AddControl(L"textbutton2", EControlType::TextButton, D3DXVECTOR2(25, 25), D3DXVECTOR2(100, 50), L"Toggle button")
		->ShouldUseToggleSelection(true);

	pMainGUIWindow->AddControl(L"scrollbar1", EControlType::ScrollBar, D3DXVECTOR2(340, 0), D3DXVECTOR2(160, 0))
		->MakeScrollBar(EScrollBarDirection::Horizontal)
		->SetScrollRange(2, 5)
		->SetScrollPosition(0);

	pMainGUIWindow->AddControl(L"scrollbar2", EControlType::ScrollBar, D3DXVECTOR2(320, 100), D3DXVECTOR2(0, 200))
		->MakeScrollBar(EScrollBarDirection::Vertical)
		->SetScrollRange(10, 30)
		->SetScrollPosition(0);

	pMainGUIWindow->AddControl(L"label1", EControlType::Label, D3DXVECTOR2(120, 80), D3DXVECTOR2(150, 40), L"레이블입니다!")
		->SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle)
		->SetBackgroundColor(D3DCOLOR_ARGB(100, 0, 255, 255))
		->AttachScrollBar(pMainGUIWindow->GetControlPtr(L"scrollbar2"));

	pMainGUIWindow->AddControl(L"imagebutton1", ::ImageButton, D3DXVECTOR2(120, 0), D3DXVECTOR2(100, 50))
		->MakeSystemArrowButton(ESystemArrowDirection::Left);

	pMainGUIWindow->AddControl(L"checkbox1", EControlType::CheckBox, D3DXVECTOR2(250, 0), D3DXVECTOR2(50, 50));

	pMainGUIWindow->AddControl(L"radiobox1", EControlType::RadioBox, D3DXVECTOR2(320, 0), D3DXVECTOR2(0, 0));

	pMainGUIWindow->AddControl(L"radiobox2", EControlType::RadioBox, D3DXVECTOR2(320, 20), D3DXVECTOR2(0, 0));

	LPDIRECT3DTEXTURE9 test_texture;
	D3DXIMAGE_INFO test_texture_info;

	pMainGUIWindow->CreateTexture(L"test_atlas.png", &test_texture, &test_texture_info);
	
	pMainGUIWindow->AddControl(L"listbox1", EControlType::ListBox, D3DXVECTOR2(360, 100), D3DXVECTOR2(200, 90))
		->ShouldUseImageItem(true)
		->SetImageItemTextureAtlas(test_texture, &test_texture_info)
		->AddListBoxItem(L"1. 안녕하세요?", D3DXVECTOR2(0, 64), D3DXVECTOR2(32, 32))
		->AddListBoxItem(L"2. Hello.", D3DXVECTOR2(32, 64), D3DXVECTOR2(16, 16))
		->AddListBoxItem(L"3. Hola.", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64))
		->AddListBoxItem(L"4. こんにちは。", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64))
		->AddListBoxItem(L"5. Nihao", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64))
		->AddListBoxItem(L"6. Привет")
		->AddListBoxItem(L"7. ...");

	pMainGUIWindow->AddControl(L"imagebox1", EControlType::ImageBox, D3DXVECTOR2(600, 100), D3DXVECTOR2(140, 90))
		->SetTextureAtlas(test_texture, &test_texture_info)
		->SetAtlasUV(D3DXVECTOR2(0, 64), D3DXVECTOR2(32, 32))
		->SetSize(D3DXVECTOR2(100, 20));

	pMainGUIWindow->AddControl(L"edit1", EControlType::Edit, D3DXVECTOR2(0, 140), D3DXVECTOR2(280, 200))
		->ShouldUseMultiline(true)
		->SetText(L"This is JWEdit-control.\nTest it!\nThird line it is!\nAnd forth this is.")
		->SetWatermark(L"Edit 1");

	pMainGUIWindow->AddControl(L"edit2", EControlType::Edit, D3DXVECTOR2(0, 360), D3DXVECTOR2(180, 60))
		->SetWatermark(L"Edit 2");

	myGUI.SetMainLoopFunction(MainLoop);

	myGUI.Run();

	return 0;
}

void MainLoop()
{
	THandleItem clicked_subitem = THandleItem_Null;

	if ((clicked_subitem = pMainGUIWindow->GetControlPtr(L"menubar")->OnSubItemClick()) != THandleItem_Null)
	{
		if ((clicked_subitem == mb_file_new) && (!pDialogueNewMap))
		{
			ShowDialogueNewMap();
		}
	}

	if (pDialogueNewMap)
	{
		if (pDialogueNewMap->GetControlPtr(L"btn_close")->OnMouseCliked())
		{
			WSTRING text;
			pDialogueNewMap->GetControlPtr(L"edit_name")->GetText(&text);
			std::wcout << text.c_str() << std::endl;
		}
	}
}

void ShowDialogueNewMap()
{
	SWindowCreationData myWindowData = SWindowCreationData(L"New map", 100, 100, 300, 200, DEFAULT_COLOR_LESS_BLACK);

	myGUI.AddGUIWindow(myWindowData, &pDialogueNewMap);

	pDialogueNewMap->AddControl(L"btn_close", EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"CLOSE");

	float edit_offset_x = 100.0f;

	pDialogueNewMap->AddControl(L"label1", EControlType::Label, D3DXVECTOR2(0, 60), D3DXVECTOR2(edit_offset_x, 20))
		->SetTextAlignment(EHorizontalAlignment::Right, EVerticalAlignment::Middle)
		->SetBackgroundColor(0)
		->SetText(L"맵 이름: ");

	pDialogueNewMap->AddControl(L"label2", EControlType::Label, D3DXVECTOR2(0, 90), D3DXVECTOR2(edit_offset_x, 20))
		->SetTextAlignment(EHorizontalAlignment::Right, EVerticalAlignment::Middle)
		->SetBackgroundColor(0)
		->SetText(L"맵 가로 크기: ");

	pDialogueNewMap->AddControl(L"label3", EControlType::Label, D3DXVECTOR2(0, 120), D3DXVECTOR2(edit_offset_x, 20))
		->SetTextAlignment(EHorizontalAlignment::Right, EVerticalAlignment::Middle)
		->SetBackgroundColor(0)
		->SetText(L"맵 세로 크기: ");

	pDialogueNewMap->AddControl(L"edit_name", EControlType::Edit, D3DXVECTOR2(edit_offset_x, 60), D3DXVECTOR2(160, 20))
		->SetWatermark(L"맵 이름을 입력하세요.");

	pDialogueNewMap->AddControl(L"edit_x_size", EControlType::Edit, D3DXVECTOR2(edit_offset_x, 90), D3DXVECTOR2(160, 20))
		->SetWatermark(L"맵의 가로 크기");

	pDialogueNewMap->AddControl(L"edit_y_size", EControlType::Edit, D3DXVECTOR2(edit_offset_x, 120), D3DXVECTOR2(160, 20))
		->SetWatermark(L"맵의 세로 크기");
}