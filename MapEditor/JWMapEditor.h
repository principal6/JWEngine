#pragma once

#include "resource.h"
#include "../CoreBase/JWWindow.h"
#include "../Core/JWMap.h"
#include "JWTileMapSelector.h"

namespace JWENGINE
{
	static const wchar_t MAP_EDITOR_NAME[]{ L"JW Map Editor" };
	static const wchar_t PROJECT_FOLDER[]{ L"\\MapEditor" };

	class JWMapEditor final
	{
	private:
		static const int WINDOW_X = 50;
		static const int WINDOW_Y = 50;
		static const int WINDOW_SEPERATE_X = 250;
		static const int WINDOW_SEPERATE_INTERVAL = 10;
		static const int WINDOW_PADDING_X = 10;

		static UNIQUE_PTR<JWWindow> ms_WindowParent;
		static UNIQUE_PTR<JWWindow> ms_WindowLeft;
		static UNIQUE_PTR<JWWindow> ms_WindowRight;
		static SMapInfo ms_MapInfo;

		// For left child window
		static UNIQUE_PTR<JWImage> ms_TileImage;
		static UNIQUE_PTR<JWImage> ms_MoveImage;
		static UNIQUE_PTR<JWImage> ms_TileBG;
		
		// For right child window
		static UNIQUE_PTR<JWMap> ms_Map;
		static UNIQUE_PTR<JWImage> ms_MapBG;

		// For both child windows
		static UNIQUE_PTR<JWTileMapSelector> ms_MapTileSelector;

		WSTRING m_BaseDir;
		bool m_Keys[NUM_KEYS];
		HWND m_hWndMain;
		MSG m_MSG;
		HACCEL m_hAccel;

	private:
		void JWMapEditor::Destroy();

		void JWMapEditor::MainLoop();

		static void LoadTileImages();
		static void UpdateMapEditorCaption();
		static void UpdateScrollbarSize();
		
		friend auto GetLeftChildPositionAndSizeFromParent(RECT Rect)->RECT;
		friend auto GetRightChildPositionAndSizeFromParent(RECT Rect)->RECT;
		friend LRESULT CALLBACK ParentWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
		friend LRESULT CALLBACK LeftChildWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
		friend LRESULT CALLBACK RightChildWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
		friend LRESULT CALLBACK DlgProcNewMap(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

	public:
		JWMapEditor() {};
		~JWMapEditor() {};

		auto JWMapEditor::Create(int Width, int Height)->EError;

		void JWMapEditor::Run();
	};
};