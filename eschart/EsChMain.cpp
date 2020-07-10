#include "eschart_pch.h"
#pragma hdrstop
//---------------------------------------------------------------------------

#if defined(WIN32) && defined(ES_DYNAMIC_LINKAGE)
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpReserved) 
{
  switch( fdwReason ) 
  { 
  case DLL_PROCESS_ATTACH:               
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  }

  return TRUE;  
}
#endif
//---------------------------------------------------------------------------
