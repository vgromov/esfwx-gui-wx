// file used solely for precomiled symbols generation
#include "escomm_gui_pch.h"
#pragma hdrstop

ES_REFLECTION_REGISTRAR_BEGIN(escommgui)
	//ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoUartConfigPaneWnd::EsChannelIoUartConfigPane)
	//ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoEkonnectConfigPaneWnd::EsChannelIoEkonnectConfigPane)
	//ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoSocketClientConfigPaneWnd::EsChannelIoSocketClientConfigPane)
	//ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoSocketServerConfigPaneWnd::EsChannelIoSocketServerConfigPane)
	//ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoStubConfigPaneWnd::EsChannelIoStubConfigPane)
	//ES_REFLECTION_REGISTRAR_ENTRY(EsProtocolConfigPaneWnd::EsProtocolConfigPane)
ES_REFLECTION_REGISTRAR_END

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
