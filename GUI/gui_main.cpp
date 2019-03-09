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
// Add FrameConnector(Verical & Horizontal)

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	try
	{
		SWindowCreationData myWindowData = SWindowCreationData(L"JWGUI Test", 0, 100, 800, 600, DEFAULT_COLOR_LESS_BLACK, EWindowStyle::OverlappedWindow);
		myGUI.Create(myWindowData, pMainGUIWindow);

		pMainGUIWindow->AddControl(EControlType::MenuBar, D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 0), L"menubar");
		THandleItem mb_file = pMainGUIWindow->GetControl(L"menubar").AddMenuBarItem(L"파일");
		mb_file_new = pMainGUIWindow->GetControl(L"menubar").AddMenuBarSubItem(mb_file, L"새로 만들기");
		mb_file_open = pMainGUIWindow->GetControl(L"menubar").AddMenuBarSubItem(mb_file, L"열기");
		THandleItem mb_help = pMainGUIWindow->GetControl(L"menubar").AddMenuBarItem(L"도움말");
		mb_help_info = pMainGUIWindow->GetControl(L"menubar").AddMenuBarSubItem(mb_help, L"정보");

		pMainGUIWindow->AddControl(EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"textbutton1")
			->SetText(L"Normal button");

		pMainGUIWindow->AddControl(EControlType::TextButton, D3DXVECTOR2(280, 25), D3DXVECTOR2(100, 50), L"textbutton2")
			->SetText(L"Toggle button")
			->ShouldUseToggleSelection(true);

		pMainGUIWindow->AddControl(EControlType::ScrollBar, D3DXVECTOR2(160, 0), D3DXVECTOR2(160, 0), L"scrollbar1")
			->MakeScrollBar(EScrollBarDirection::Horizontal)
			->SetScrollRange(2, 5)
			->SetScrollPosition(0);

		pMainGUIWindow->AddControl(EControlType::ScrollBar, D3DXVECTOR2(320, 200), D3DXVECTOR2(0, 240), L"scrollbar2")
			->MakeScrollBar(EScrollBarDirection::Vertical)
			->SetScrollRange(10, 30)
			->SetScrollPosition(0);

		pMainGUIWindow->AddControl(EControlType::Label, D3DXVECTOR2(120, 80), D3DXVECTOR2(150, 40), L"label1")
			->SetText(L"레이블입니다!")
			->SetTextAlignment(EHorizontalAlignment::Center, EVerticalAlignment::Middle)
			->SetBackgroundColor(D3DCOLOR_ARGB(100, 0, 255, 255))
			->AttachScrollBar(pMainGUIWindow->GetControl(L"scrollbar2"));

		pMainGUIWindow->AddControl(EControlType::Edit, D3DXVECTOR2(0, 0), D3DXVECTOR2(240, 180), L"edit1")
			->ShouldUseMultiline(true)
			->SetText(L"This is JWEdit-control.\nTest it!\nThird line it is!\nAnd forth this is.")
			->SetWatermark(L"Edit 1");

		pMainGUIWindow->AddControl(EControlType::Edit, D3DXVECTOR2(200, 200), D3DXVECTOR2(180, 60), L"edit2")
			->SetWatermark(L"Edit 2");

		pMainGUIWindow->AddControl(EControlType::Frame, D3DXVECTOR2(50, 20), D3DXVECTOR2(350, 400), L"frame1")
			->AddChildControl(pMainGUIWindow->GetControl(L"textbutton1"))
			->AddChildControl(pMainGUIWindow->GetControl(L"textbutton2"))
			->AddChildControl(pMainGUIWindow->GetControl(L"scrollbar1"))
			->AddChildControl(pMainGUIWindow->GetControl(L"scrollbar2"))
			->AddChildControl(pMainGUIWindow->GetControl(L"label1"))
			->AddChildControl(pMainGUIWindow->GetControl(L"edit1"))
			->AddChildControl(pMainGUIWindow->GetControl(L"edit2"))
			->SetBackgroundColor(D3DCOLOR_ARGB(255, 60, 180, 60))
			->SetBorderColor(D3DCOLOR_ARGB(255, 0, 0, 0));

		pMainGUIWindow->GetControl(L"edit1").SetPosition(D3DXVECTOR2(20, 150));
		pMainGUIWindow->GetControl(L"edit2").SetPosition(D3DXVECTOR2(20, 350));

		pMainGUIWindow->AddControl(EControlType::ImageButton, D3DXVECTOR2(20, 0), D3DXVECTOR2(20, 20), L"imagebutton1")
			->MakeSystemArrowButton(ESystemArrowDirection::Left);

		pMainGUIWindow->AddControl(EControlType::CheckBox, D3DXVECTOR2(20, 30), D3DXVECTOR2(30, 30), L"checkbox1");

		pMainGUIWindow->AddControl(EControlType::RadioBox, D3DXVECTOR2(20, 70), D3DXVECTOR2(0, 0), L"radiobox1");

		pMainGUIWindow->AddControl(EControlType::RadioBox, D3DXVECTOR2(20, 90), D3DXVECTOR2(0, 0), L"radiobox2");

		LPDIRECT3DTEXTURE9 test_texture;
		D3DXIMAGE_INFO test_texture_info;

		pMainGUIWindow->CreateTexture(L"test_atlas.png", &test_texture, &test_texture_info);

		pMainGUIWindow->AddControl(EControlType::ListBox, D3DXVECTOR2(20, 120), D3DXVECTOR2(200, 90), L"listbox1")
			->ShouldUseImageItem(true)
			->SetImageItemTextureAtlas(test_texture, &test_texture_info)
			->AddListBoxItem(L"1. 안녕하세요?", D3DXVECTOR2(0, 64), D3DXVECTOR2(32, 32))
			->AddListBoxItem(L"2. Hello.", D3DXVECTOR2(32, 64), D3DXVECTOR2(16, 16))
			->AddListBoxItem(L"3. Hola.", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64))
			->AddListBoxItem(L"4. こんにちは。", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64))
			->AddListBoxItem(L"5. Nihao", D3DXVECTOR2(0, 0), D3DXVECTOR2(64, 64))
			->AddListBoxItem(L"6. Привет")
			->AddListBoxItem(L"7. ...");

		pMainGUIWindow->AddControl(EControlType::ImageBox, D3DXVECTOR2(0, 240), D3DXVECTOR2(140, 100), L"imagebox1")
			->SetTextureAtlas(test_texture, &test_texture_info)
			->SetAtlasUV(D3DXVECTOR2(0, 64), D3DXVECTOR2(32, 32))
			->SetSize(D3DXVECTOR2(100, 20));

		pMainGUIWindow->AddControl(EControlType::Frame, D3DXVECTOR2(80, 20), D3DXVECTOR2(300, 500), L"frame2")
			->AddChildControl(pMainGUIWindow->GetControl(L"imagebutton1"))
			->AddChildControl(pMainGUIWindow->GetControl(L"checkbox1"))
			->AddChildControl(pMainGUIWindow->GetControl(L"radiobox1"))
			->AddChildControl(pMainGUIWindow->GetControl(L"radiobox2"))
			->AddChildControl(pMainGUIWindow->GetControl(L"listbox1"))
			->AddChildControl(pMainGUIWindow->GetControl(L"imagebox1"))
			->SetBackgroundColor(D3DCOLOR_ARGB(255, 60, 180, 120))
			->SetBorderColor(D3DCOLOR_ARGB(255, 0, 0, 0));

		pMainGUIWindow->AddControl(EControlType::FrameConnector, D3DXVECTOR2(0, 0), D3DXVECTOR2(50, 50), L"frameconnector1")
			->MakeVerticalConnector(pMainGUIWindow->GetControl(L"frame1"), pMainGUIWindow->GetControl(L"frame2"))
			//->MakeHorizontalConnector(pMainGUIWindow->GetControl(L"frame1"), pMainGUIWindow->GetControl(L"frame2"))
			->SetSize(D3DXVECTOR2(10, 10));
		
		pMainGUIWindow->GetControl(L"frame1").SetPosition(D3DXVECTOR2(0, 0));
		pMainGUIWindow->GetControl(L"frame2").SetSize(D3DXVECTOR2(200, 50));

		myGUI.SetMainLoopFunction(MainLoop);

		myGUI.Run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		
		abort();
	}
	
	return 0;
}

void MainLoop()
{
	THandleItem clicked_subitem = THandleItem_Null;
	
	if ((clicked_subitem = pMainGUIWindow->GetControl(L"menubar").OnSubItemClick()) != THandleItem_Null)
	{
		if ((clicked_subitem == mb_file_new) && (!pDialogueNewMap))
		{
			ShowDialogueNewMap();
		}
		else if (clicked_subitem == mb_file_open)
		{

		}
		else if (clicked_subitem == mb_help_info)
		{
			MessageBoxA(nullptr, "만든이: 김장원 (헤수스 김)", "JW 엔진", MB_OK);
		}
	}
	
	if (pDialogueNewMap)
	{
		if (pDialogueNewMap->GetControl(L"btn_close").OnMouseCliked())
		{
			WSTRING text;
			pDialogueNewMap->GetControl(L"edit_name").GetText(text);
			std::wcout << text.c_str() << std::endl;

			myGUI.DestroyGUIWindow(pDialogueNewMap);
		}
	}
}

void ShowDialogueNewMap()
{
	SWindowCreationData myWindowData = SWindowCreationData(L"New map", 100, 100, 300, 200, DEFAULT_COLOR_LESS_BLACK, EWindowStyle::OverlappedWindow);

	myGUI.AddGUIWindow(myWindowData, pDialogueNewMap);

	pDialogueNewMap->AddControl(EControlType::TextButton, D3DXVECTOR2(0, 0), D3DXVECTOR2(100, 50), L"btn_close")
		->SetText(L"Close");

	float edit_offset_x = 100.0f;

	pDialogueNewMap->AddControl(EControlType::Label, D3DXVECTOR2(0, 60), D3DXVECTOR2(edit_offset_x, 20))
		->SetText(L"맵 이름: ")
		->SetTextAlignment(EHorizontalAlignment::Right, EVerticalAlignment::Middle)
		->SetBackgroundColor(0);

	pDialogueNewMap->AddControl(EControlType::Label, D3DXVECTOR2(0, 90), D3DXVECTOR2(edit_offset_x, 20))
		->SetText(L"맵 가로 크기: ")
		->SetTextAlignment(EHorizontalAlignment::Right, EVerticalAlignment::Middle)
		->SetBackgroundColor(0);

	pDialogueNewMap->AddControl(EControlType::Label, D3DXVECTOR2(0, 120), D3DXVECTOR2(edit_offset_x, 20))
		->SetText(L"맵 세로 크기: ")
		->SetTextAlignment(EHorizontalAlignment::Right, EVerticalAlignment::Middle)
		->SetBackgroundColor(0);

	pDialogueNewMap->AddControl(EControlType::Edit, D3DXVECTOR2(edit_offset_x, 60), D3DXVECTOR2(160, 20), L"edit_name")
		->SetWatermark(L"맵 이름을 입력하세요.");

	
	pDialogueNewMap->AddControl(EControlType::Edit, D3DXVECTOR2(edit_offset_x, 90), D3DXVECTOR2(160, 20), L"edit_x_size")
		->ShouldUseNumberInputsOnly(true)
		->SetWatermark(L"맵의 가로 크기");

	pDialogueNewMap->AddControl(EControlType::Edit, D3DXVECTOR2(edit_offset_x, 120), D3DXVECTOR2(160, 20), L"edit_y_size")
		->ShouldUseNumberInputsOnly(true)
		->SetWatermark(L"맵의 세로 크기");
}