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

		void Create(SWindowCreationData& WindowCreationData, JWGUIWindow*& OutPtrMainGUIWindow) noexcept;

		void AddGUIWindow(SWindowCreationData& WindowCreationData, JWGUIWindow*& OutPtrGUIWindow) noexcept;

		void DestroyGUIWindow(const JWGUIWindow* pGUIWindow) noexcept;

		void SetMainLoopFunction(const PF_MAINLOOP pfMainLoop) noexcept;

		void Run() noexcept;

	private:
		friend LRESULT CALLBACK GUIWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

		void EraseGUIWindow(size_t index) noexcept;
		void ClearGUIWindow() noexcept;

	private:
		static SGUIIMEInputInfo ms_IMEInfo;
		static VECTOR<HWND> ms_hWndQuitStack;

		bool m_bIsGUIRunning{ false };

		MSG m_MSG{};

		PF_MAINLOOP m_pfMainLoop{ nullptr };

		TLinkedList<UNIQUE_PTR<JWGUIWindow>> m_pGUIWindows;

		// This is required to keep track of
		// the outter pointers to the instance of JWGUIWindows
		// in order to make them nullptr when JWGUIWindows are destroyed.
		TLinkedList<JWGUIWindow**> m_ppGUIWindows;
	};
};