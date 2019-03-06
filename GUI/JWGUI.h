#pragma once

#include "JWGUIWindow.h"
#include "../CoreType/TLinkedList.h"

namespace JWENGINE
{
	using PF_MAINLOOP = void(*)();

	class JWGUI final
	{
	public:
		JWGUI() {};
		~JWGUI() {};

		void Create(SWindowCreationData& WindowCreationData, JWGUIWindow*& OutPtrMainGUIWindow);
		void Destroy() noexcept;

		void AddGUIWindow(SWindowCreationData& WindowCreationData, JWGUIWindow*& OutPtrGUIWindow);

		void DestroyGUIWindow(const JWGUIWindow* pGUIWindow) noexcept;

		void SetMainLoopFunction(const PF_MAINLOOP pfMainLoop) noexcept;

		void Run() noexcept;

	private:
		friend LRESULT CALLBACK GUIWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	private:
		static SGUIIMEInputInfo ms_IMEInfo;
		static VECTOR<HWND> ms_hWndQuitStack;

		bool m_bIsGUIRunning = false;

		MSG m_MSG{};

		PF_MAINLOOP m_pfMainLoop = nullptr;

		JWGUIWindow* m_pMainGUIWindow = nullptr;
		TLinkedList<JWGUIWindow**> m_ppGUIWindows;
	};
};