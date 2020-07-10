#include "escore_gui_pch.h"
#pragma hdrstop
//---------------------------------------------------------------------------

#ifdef WIN32
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
