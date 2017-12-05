

#include "RuntimeHost.h"

_AppDomainPtr RuntimeHost(PCWSTR  pszVersion)
{
	HRESULT hr;

	ICLRMetaHost *pMetaHost = NULL;
	ICLRRuntimeInfo *pRuntimeInfo = NULL;
	ICorRuntimeHost *pCorRuntimeHost = NULL;
	IUnknownPtr spAppDomainThunk = NULL;
	_AppDomainPtr spDefaultAppDomain = NULL;
	_AssemblyPtr spAssembly = NULL;

	

	wprintf(L"Load and start the .NET runtime %s \n", pszVersion);

	hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&pMetaHost));
	if (FAILED(hr))
	{
		wprintf(L"CLRCreateInstance failed w/hr 0x%08lx\n", hr);
		goto Cleanup;
	}

	// Get the ICLRRuntimeInfo corresponding to a particular CLR version. It 
	// supersedes CorBindToRuntimeEx with STARTUP_LOADER_SAFEMODE.
	hr = pMetaHost->GetRuntime(pszVersion, IID_PPV_ARGS(&pRuntimeInfo));
	if (FAILED(hr))
	{
		wprintf(L"ICLRMetaHost::GetRuntime failed w/hr 0x%08lx\n", hr);
		goto Cleanup;
	}

	// Check if the specified runtime can be loaded into the process. This 
	// method will take into account other runtimes that may already be 
	// loaded into the process and set pbLoadable to TRUE if this runtime can 
	// be loaded in an in-process side-by-side fashion. 
	BOOL fLoadable;
	hr = pRuntimeInfo->IsLoadable(&fLoadable);
	if (FAILED(hr))
	{
		wprintf(L"ICLRRuntimeInfo::IsLoadable failed w/hr 0x%08lx\n", hr);
		goto Cleanup;
	}

	if (!fLoadable)
	{
		wprintf(L".NET runtime %s cannot be loaded\n", pszVersion);
		goto Cleanup;
	}

	hr = pRuntimeInfo->GetInterface(CLSID_CorRuntimeHost,
		IID_PPV_ARGS(&pCorRuntimeHost));
	if (FAILED(hr))
	{
		wprintf(L"ICLRRuntimeInfo::GetInterface failed w/hr 0x%08lx\n", hr);
		goto Cleanup;
	}

	// Start the CLR.
	hr = pCorRuntimeHost->Start();
	if (FAILED(hr))
	{
		wprintf(L"CLR failed to start w/hr 0x%08lx\n", hr);
		goto Cleanup;
	}

	hr = pCorRuntimeHost->GetDefaultDomain(&spAppDomainThunk);

	if (FAILED(hr))
	{
		wprintf(L"ICorRuntimeHost::GetDefaultDomain failed w/hr 0x%08lx\n", hr);
		goto Cleanup;
	}

	hr = spAppDomainThunk->QueryInterface(IID_PPV_ARGS(&spDefaultAppDomain));
	if (FAILED(hr))
	{
		wprintf(L"Failed to get default AppDomain w/hr 0x%08lx\n", hr);
		goto Cleanup;
	}

	// Load the .NET assembly.
	wprintf(L"Load the assembly %s\n", L"MainTest.exe");
	

Cleanup:
//
//	if (pMetaHost)
//	{
//		pMetaHost->Release();
//		pMetaHost = NULL;
//	}
//	if (pRuntimeInfo)
//	{
//		pRuntimeInfo->Release();
//		pRuntimeInfo = NULL;
//	}
//	if (pCorRuntimeHost)
//	{
//	
//		//pCorRuntimeHost->Stop();
//
//		pCorRuntimeHost->Release();
//		pCorRuntimeHost = NULL;
//	}
//
//	
//
	return spDefaultAppDomain;
}
///Load a assembly by entryPoint method;
VARIANT LaunchAssembly(_AppDomainPtr spDefaultAppDomain, _AssemblyPtr spAssembly) {

	
	mscorlib::_MethodInfo *spEntryPoint = NULL;
	HRESULT hr;

	VARIANT vtLengthRet;
	VariantInit(&vtLengthRet);

	SAFEARRAY *spEntryPointParameters;
	SAFEARRAY *args = NULL;

	hr = spAssembly->get_EntryPoint(&spEntryPoint);
	hr = spEntryPoint->GetParameters(&spEntryPointParameters);

	if (spEntryPointParameters && spEntryPointParameters->cDims > 0 && spEntryPointParameters->rgsabound[0].cElements > 0)
		args = get_CommandLineAgrs();
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	hr = spEntryPoint->Invoke_3(_variant_t(), args, &vtLengthRet);
	return vtLengthRet;
}

SAFEARRAY *get_CommandLineAgrs() {
	SAFEARRAY *args = SafeArrayCreateVector(VT_VARIANT, 0, 1); // create an array of the length of 1 ( Main(string[]) )
	int     argc;
	LPWSTR  cmdLine = GetCommandLineW();
	LPWSTR  *argv = CommandLineToArgvW(cmdLine, &argc); // get an array of arguments to this function
	VARIANT vtPsa;
	vtPsa.vt = (VT_ARRAY | VT_BSTR);
	vtPsa.parray = SafeArrayCreateVector(VT_BSTR, 0, argc); // create an array of strings
	for (long i = 0; i < argc; i++)
	{
		SafeArrayPutElement(vtPsa.parray, &i, SysAllocString(argv[i])); // insert the string from argv[i] into the safearray
	}
	long idx[1] = { 0 };
	SafeArrayPutElement(args, idx, &vtPsa); // insert an array of BSTR into the VT_VARIANT args array
	return args;
}

SAFEARRAY *get_Assembly() {
	HMODULE hModule = GetModuleHandle(NULL); // get the handle to the current module (the executable file)
	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(IDR_RCDATA1), RT_RCDATA); // substitute RESOURCE_ID and RESOURCE_TYPE.
	HGLOBAL hMemory = LoadResource(hModule, hResource);
	DWORD dwSize = SizeofResource(hModule, hResource);
	LPVOID lpAddress = LockResource(hMemory);
	char *bytes = new char[dwSize];
	memcpy(bytes, lpAddress, dwSize);

	SAFEARRAYBOUND bounds = { dwSize, 0 };
	SAFEARRAY *psa = SafeArrayCreate(VT_UI1, 1, &bounds);
	void* data;
	SafeArrayAccessData(psa, &data);
	CopyMemory(data, bytes, dwSize);
	SafeArrayUnaccessData(psa);
	return psa;
}

_AssemblyPtr Load_Assembly(_AppDomainPtr spDefaultAppDomain){
	_AssemblyPtr spAssembly = NULL;
	 spDefaultAppDomain->Load_3(get_Assembly(), &spAssembly);
	 return spAssembly;
}