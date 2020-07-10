#ifndef _es_core_gui_h_
#define _es_core_gui_h_

#ifndef _es_core_h_
#	error escore.h header file must be included prior to this header!
#endif

#include <EsCoreGuiConfig.h>

// escore export-import defines
#ifdef ES_DYNAMIC_LINKAGE
#	ifdef ESCOREGUI_EXPORTS
#		define ESCORE_GUI_CLASS	            ES_EXPORT_SPEC
#		define ESCORE_GUI_FUNC(type, decl)	ES_FUNCEXPORT_SPEC(type, decl)
#		define ESCORE_GUI_DATA(type, decl)	ES_DATAEXPORT_SPEC(type, decl)
#	else
#		define ESCORE_GUI_CLASS	            ES_IMPORT_SPEC
#		define ESCORE_GUI_FUNC(type, decl)	ES_FUNCIMPORT_SPEC(type, decl)
#		define ESCORE_GUI_DATA(type, decl)	ES_DATAIMPORT_SPEC(type, decl)
#	endif
#else
#	define ESCORE_GUI_CLASS
#	define ESCORE_GUI_FUNC(type, decl)    type decl
#	define ESCORE_GUI_DATA(type, decl)    type decl
#endif

#ifndef WXINTL_NO_GETTEXT_MACRO
# define WXINTL_NO_GETTEXT_MACRO
#endif

// WX Non-GUI and GUI stuff
#ifndef WX_PRECOMP
# include <wx/wx.h>
#else
# include <wx/wxprec.h>
#endif

#include <wx/file.h>
#include <wx/config.h>
#include <wx/filepicker.h>
#include <wx/filehistory.h>
#include <wx/wupdlock.h>
#include <wx/artprov.h>
#include <wx/listbase.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>
#include <wx/aui/aui.h>
#include <wx/sysopt.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dataview.h>

#include <escore-gui/EsCoreGuiDefs.h>

// our headers
#if defined(ES_USE_AUTO_UPDATER) && (ES_OS == ES_OS_WINDOWS)
# include <escore-gui/EsAutoUpdater.h>
#endif
#include <escore-gui/EsDlgBase.h>
#include <escore-gui/EsCoreGuiUtilities.h>
#include <escore-gui/EsReflectionGuiLinks.h>
#include <escore-gui/EsMultilineTextRenderer.h>
#include <escore-gui/EsDataViewStyle.h>
#include <escore-gui/EsListViewModelBase.h>
#include <escore-gui/EsListViewModelArray.h>
#include <escore-gui/EsListViewBase.h>
#include <escore-gui/EsAppLog.h>
#include <escore-gui/EsProgressMonitorView.h>
#include <escore-gui/EsReflectedClassConfigPane.h>
#include <escore-gui/EsWxWindowPtr.h>

#endif // _es_core_gui_h_
