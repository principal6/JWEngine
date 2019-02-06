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
#include "JWRadioBox.h"
#include "JWScrollBar.h"
#include "JWListBox.h"

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

		auto AddControl(EControlType Type, D3DXVECTOR2 Position, D3DXVECTOR2 Size, WSTRING Text = L"")->const THandle;
		auto GetControlPointer(const THandle ControlHandle)->JWControl*;
		
		void SetMainLoopFunction(PF_MAINLOOP pfMainLoop);

		void DrawAllControls();

	private:
		void MainLoop();
		void HandleMessage();
		
	private:
		SGUISharedData m_SharedData;

		VECTOR<JWControl*> m_Controls;
		MSG m_MSG;

		SMouseData m_MouseData;

		JWControl* m_pControlWithFocus;

		PF_MAINLOOP m_pfMainLoop;
	};
};