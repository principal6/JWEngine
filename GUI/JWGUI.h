#pragma once

#include "JWGUIWindow.h"

namespace JWENGINE
{
	using PF_MAINLOOP = void(*)();

	class JWGUI final
	{
	public:
		JWGUI();
		~JWGUI() {};

		auto Create(SWindowCreationData& WindowCreationData, JWGUIWindow** OutPtrMainGUIWindow)->EError;
		void Destroy();

		void AddGUIWindow(SWindowCreationData& WindowCreationData, JWGUIWindow** OutPtrGUIWindow);

		void SetMainLoopFunction(const PF_MAINLOOP pfMainLoop);

		void Run();

	private:
		friend LRESULT CALLBACK GUIWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	private:
		static SGUIIMEInputInfo ms_IMEInfo;
		static HWND ms_QuitWindowHWND;

		bool m_bIsGUIRunning;

		MSG m_MSG;

		PF_MAINLOOP m_pfMainLoop;

		JWGUIWindow* m_pMainGUIWindow;
		TLinkedList<JWGUIWindow**> m_ppGUIWindows;
	};
};