#pragma once

#include "../CoreBase/JWImage.h"
#include "../CoreBase/JWLine.h"
#include "../CoreBase/JWFont.h"
#include "../CoreBase/JWWindow.h"

namespace JWENGINE
{
	class JWGUI final
	{
	public:
		JWGUI() {};
		~JWGUI() {};

		auto Create()->EError;
		void Destroy();

		void Run();
		
	private:
		void MainLoop();
		void HandleMessage();

	private:
		UNIQUE_PTR<JWWindow> m_Window;

		WSTRING m_BaseDir;
		MSG m_MSG;
	};
};