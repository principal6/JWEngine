#pragma once

#include "JWGUIWindow.h"

namespace JWENGINE
{
	using PF_MAINLOOP = void(*)();

	class JWGUI final
	{
	public:
		JWGUI() {};
		~JWGUI() {};

		auto Create(SWindowCreationData& WindowCreationData)->EError;
		void Destroy();

		auto AddGUIWindow(SWindowCreationData& WindowCreationData)->JWGUIWindow*;

		void SetMainLoopFunction(PF_MAINLOOP pfMainLoop);

		void Run();

		auto GetMainGUIWindowPtr()->JWGUIWindow*;

	private:
		friend LRESULT CALLBACK GUIWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	private:
		static SGUIIMEInputInfo ms_IMEInfo;
		static HWND ms_QuitWindowHWND;

		bool m_bIsGUIRunning;

		MSG m_MSG;

		PF_MAINLOOP m_pfMainLoop;

		TLinkedList<JWGUIWindow*> m_pGUIWindows;
	};
};