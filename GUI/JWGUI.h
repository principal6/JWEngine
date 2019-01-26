#pragma once

#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWLine.h"
#include "../CoreBase/JWWindow.h"
#include "JWControl.h"
#include "JWButton.h"

// @WARNING: ONE GUI PER ONE WINDOW!!

namespace JWENGINE
{
	static const wchar_t PROJECT_FOLDER[]{ L"\\GUI" };

	class JWGUI final
	{
	public:
		JWGUI() {};
		~JWGUI() {};

		auto Create(JWWindow* pWindow)->EError;
		void Destroy();

		void Run();

		auto AddControl(EControlType Type, WSTRING Text, D3DXVECTOR2 Position, D3DXVECTOR2 Size)->EError;

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
	};
};