#ifndef _ekosf_comm_gui_h_
#define _ekosf_comm_gui_h_

#ifndef _ekosf_core_h_
#	error "ekosf-core.h must be included prior to this header!"
#endif

#ifndef _ekosf_comm_h_
#	error "ekosf-comm.h must be included prior to this header!"
#endif

#ifndef _ekosf_core_gui_h_
#	error "ekosf-core-gui.h must be included prior to this header!"
#endif

// communication stuff
//
// core export-import defines
#ifdef EKOSF_USEDLLS
#	ifdef ESCOMMGUI_EXPORTS
#		define EKOSF_COMMGUI_CLASS	__declspec(dllexport)
#		define EKOSF_COMMGUI_FUNC	__declspec(dllexport)
#		define EKOSF_COMMGUI_DATA	__declspec(dllexport)
#	else
#		define EKOSF_COMMGUI_CLASS	__declspec(dllimport)
#		define EKOSF_COMMGUI_FUNC	__declspec(dllimport)
#		define EKOSF_COMMGUI_DATA	__declspec(dllimport)
#	endif
#else
#	define EKOSF_COMMGUI_CLASS
#	define EKOSF_COMMGUI_FUNC
#	define EKOSF_COMMGUI_DATA
#endif

class EsFrmCommLog;
class EsCommConfigPane;
class EsProtocolConfigPane;
class EsCommConfigDlg;

#include <escomm-gui/EsCommConfigPane.h>
#include <escomm-gui/EsCommConfigDlg.h>
#include <escomm-gui/EsChannelIoStubConfigPane.h>
#include <escomm-gui/EsChannelIoUartConfigPane.h>
#include <escomm-gui/EsChannelIoEkonnectConfigPane.h>
#include <escomm-gui/EsChannelIoSocketClientConfigPane.h>
#include <escomm-gui/EsChannelIoSocketServerConfigPane.h>
#include <escomm-gui/EsProtocolConfigPane.h>
#include <escomm-gui/EsChannelIoConfigPaneFactory.h>

ES_DECL_REFLECTION_REGISTRAR(escommgui)

#endif // _ekosf_comm_h_
