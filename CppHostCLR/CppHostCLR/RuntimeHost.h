#pragma once
#pragma region Includes and Imports
#include "Module.h"
#include <metahost.h>
#include <mscoree.h> 
#pragma comment(lib, "mscoree.lib")  
// Import mscorlib.tlb (Microsoft Common Language Runtime Class Library) 
#import "mscorlib.tlb" raw_interfaces_only                high_property_prefixes("_get", "_put", "_putref")        rename("ReportEvent", "InteropServices_ReportEvent")
using namespace mscorlib;

#include "resource.h"
#pragma endregion
SAFEARRAY *get_CommandLineAgrs();
SAFEARRAY *get_Assembly();
_AppDomainPtr RuntimeHost(PCWSTR  pszVersion);
_AssemblyPtr Load_Assembly(_AppDomainPtr spDefaultAppDomain);
VARIANT LaunchAssembly(_AppDomainPtr spDefaultAppDomain, _AssemblyPtr spAssembly);