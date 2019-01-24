#include "JWMapEditor.h"
#include <crtdbg.h>

#ifdef _DEBUG
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

using namespace JWENGINE;

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	static JWMapEditor myMapEditor;

	if (JW_SUCCEEDED(myMapEditor.Create(800, 600)))
	{
		myMapEditor.Run();
	}	

	return 0;
}