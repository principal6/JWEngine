#pragma once

#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWLine.h"
#include "../CoreBase/JWWindow.h"
#include "JWControl.h"
#include "JWTextButton.h"
#include "JWImageButton.h"
#include "JWLabel.h"
#include "JWEdit.h"
#include "JWCheckBox.h"

// @WARNING: ONE GUI PER ONE WINDOW!!

namespace JWENGINE
{
	static const wchar_t PROJECT_FOLDER[]{ L"\\GUI" };

	using PF_MAINLOOP = void(*)();

	class JWGUI final
	{
	public:
		JWGUI();
		~JWGUI() {};

		auto Create(JWWindow* pWindow)->EError;
		void Destroy();

		void Run();

		auto AddControl(EControlType Type, D3DXVECTOR2 Position, D3DXVECTOR2 Size, WSTRING Text = L"")->EError;
		auto GetControlPointer(size_t ControlIndex)->JWControl*;

		void SetMainLoopFunction(PF_MAINLOOP pfMainLoop);

	private:
		void MainLoop();
		void HandleMessage();
		void Draw();

	private:
		JWWindow* m_pWindow;

		VECTOR<JWControl*> m_Controls;

		WSTRING m_BaseDir;
		MSG m_MSG;

		SMouseData m_MouseData;

		JWControl* m_pControlWithFocus;

		PF_MAINLOOP m_pfMainLoop;
	};
};