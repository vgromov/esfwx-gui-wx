// file used solely for precomiled symbols generation
#include "escomm_gui_pch.h"
#pragma hdrstop

#if defined(_DEBUG) && (1 == USE_MEMLEAK_DETECTION)
#	include <vld.h>
#endif

ES_REFLECTION_REGISTRAR_BEGIN(escommgui)
	ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoUartConfigPaneWnd::EsChannelIoUartConfigPane)
	ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoEkonnectConfigPaneWnd::EsChannelIoEkonnectConfigPane)
	ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoSocketClientConfigPaneWnd::EsChannelIoSocketClientConfigPane)
	ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoSocketServerConfigPaneWnd::EsChannelIoSocketServerConfigPane)
	ES_REFLECTION_REGISTRAR_ENTRY(EsChannelIoStubConfigPaneWnd::EsChannelIoStubConfigPane)
	ES_REFLECTION_REGISTRAR_ENTRY(EsProtocolConfigPaneWnd::EsProtocolConfigPane)
ES_REFLECTION_REGISTRAR_END

#ifdef EKOSF_USEDLLS
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		reflectionInit();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif // EKOSF_USEDLLS