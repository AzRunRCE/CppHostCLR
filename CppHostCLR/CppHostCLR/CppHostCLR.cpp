

#include "RuntimeHost.h"
#include <windows.h>




INT __stdcall  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{


	ICorRuntimeHost *pCorRuntimeHost = NULL;
	_AppDomainPtr spDefaultAppDomain = NULL;
	_AssemblyPtr spAssembly = NULL;
	HRESULT hr = NULL;
	if ((spDefaultAppDomain = RuntimeHost(L"v4.0.30319")))
	{
		if (spAssembly = Load_Assembly(spDefaultAppDomain))
		{
			VARIANT vResult = LaunchAssembly(spDefaultAppDomain, spAssembly);
		}
	}

	return 0;
}
