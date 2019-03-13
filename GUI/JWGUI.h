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

		void CreateMainWindow(SWindowCreationData& WindowCreationData) noexcept;

		void CreateAdditionalWindow(SWindowCreationData& WindowCreationData) noexcept;
		void DestroyAdditionalWindow(size_t AdditionalWindowIndex) noexcept;

		auto IsAdditionalWindowAlive(size_t AdditionalWindowIndex) const->bool;

		auto MainWindow() const->JWGUIWindow&;
		auto AdditionalWindow(size_t AdditionalWindowIndex = SIZE_T_MAX) const->JWGUIWindow&;

		void SetMainLoopFunction(const PF_MAINLOOP pfMainLoop) noexcept;

		void Run() noexcept;

	private:
		friend LRESULT CALLBACK GUIWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

		void AssertValidObject() const noexcept;

		void EraseGUIWindow(size_t index) noexcept;
		void ClearGUIWindow() noexcept;

	private:
		static SGUIIMEInputInfo ms_IMEInfo;
		static VECTOR<HWND> ms_hWndQuitStack;

		bool m_IsMainWindowCreated{ false };
		bool m_IsGUIRunning{ false };

		MSG m_MSG{};

		PF_MAINLOOP m_pfMainLoop{ nullptr };

		TLinkedList<UNIQUE_PTR<JWGUIWindow>> m_pGUIWindows;
	};
};