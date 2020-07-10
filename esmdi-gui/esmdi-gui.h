#ifndef _es_mdi_gui_h_
#define _es_mdi_gui_h_

#ifndef _es_core_h_
#	error "escore.h must be included prior to this header!"
#endif

#ifndef _es_core_gui_h_
#	error "escore-gui.h must be included prior to this header!"
#endif

// ESCOMM export-import defines
#ifdef ES_DYNAMIC_LINKAGE
#	ifdef ESMDIGUI_EXPORTS
#		define ESMDIGUI_CLASS	            ES_EXPORT_SPEC
#		define ESMDIGUI_FUNC(type, decl)	ES_FUNCEXPORT_SPEC(type, decl)
#		define ESMDIGUI_DATA(type, decl)	ES_DATAEXPORT_SPEC(type, decl)
#	else
#		define ESMDIGUI_CLASS	            ES_IMPORT_SPEC
#		define ESMDIGUI_FUNC(type, decl)	ES_FUNCIMPORT_SPEC(type, decl)
#		define ESMDIGUI_DATA(type, decl)	ES_DATAIMPORT_SPEC(type, decl)
#	endif
#else
#	define ESMDIGUI_CLASS
#	define ESMDIGUI_FUNC(type, decl)    type decl
#	define ESMDIGUI_DATA(type, decl)    type decl
#endif

/// Document manager notification events declarations
///
#define ES_EVTC_MDIGUI esT("es_mdi_gui")

enum class EsMdiGuiEvt : ulong
{
  DocumentActivateRequest,       ///< Event Payload: [<key of the document to activate>, <activation initiator wx weak ref>]
  DocumentActivated,             ///< Event Payload: [<key of an activated document>, <activation initiator wx weak ref>]
  DocumentCloseRequest           ///< Event Payload: [<key of an activated document>, <activation initiator wx weak ref>] 
};

#include <esmdi-gui/EsOpenedDocumentList.h>
#include <esmdi-gui/EsMdiManagerView.h>

#endif // _es_mdi_gui_h_
