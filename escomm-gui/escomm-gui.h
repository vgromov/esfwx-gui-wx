#ifndef _ekosf_comm_gui_h_
#define _ekosf_comm_gui_h_

#ifndef _es_core_h_
#	error "escore.h must be included prior to this header!"
#endif

#ifndef _es_comm_h_
#	error "escomm.h must be included prior to this header!"
#endif

#ifndef _es_core_gui_h_
#	error "escore-gui.h must be included prior to this header!"
#endif

// ESCOMM export-import defines
#ifdef ES_DYNAMIC_LINKAGE
#	ifdef ESCOMMGUI_EXPORTS
#		define ESCOMM_GUI_CLASS	            ES_EXPORT_SPEC
#		define ESCOMM_GUI_FUNC(type, decl)	ES_FUNCEXPORT_SPEC(type, decl)
#		define ESCOMM_GUI_DATA(type, decl)	ES_DATAEXPORT_SPEC(type, decl)
#	else
#		define ESCOMM_GUI_CLASS	            ES_IMPORT_SPEC
#		define ESCOMM_GUI_FUNC(type, decl)	ES_FUNCIMPORT_SPEC(type, decl)
#		define ESCOMM_GUI_DATA(type, decl)	ES_DATAIMPORT_SPEC(type, decl)
#	endif
#else
#	define ESCOMM_GUI_CLASS
#	define ESCOMM_GUI_FUNC(type, decl)    type decl
#	define ESCOMM_GUI_DATA(type, decl)    type decl
#endif

#include <escomm-gui/EsChannelIoConfigPane.h>
#include <escomm-gui/EsCommConfigDlg.h>
#include <escomm-gui/EsChannelIoStubConfigPane.h>
#include <escomm-gui/EsChannelIoUartConfigPane.h>
#include <escomm-gui/EsChannelIoEkonnectConfigPane.h>
#include <escomm-gui/EsChannelIoSocketClientConfigPane.h>
#include <escomm-gui/EsChannelIoSocketServerConfigPane.h>
#include <escomm-gui/EsChannelIoBluetoothClientConfigPane.h>
#include <escomm-gui/EsChannelIoBluetoothLeConfigPane.h>
#include <escomm-gui/EsProtocolEcoeConfigPane.h>
#include <escomm-gui/EsIoConfigPaneFactory.h>

ES_DECL_REFLECTION_REGISTRAR(ESCOMM_GUI_FUNC, escommgui)

#endif // _ekosf_comm_h_
