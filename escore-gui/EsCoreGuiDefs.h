#ifndef _es_core_gui_defs_h_
#define _es_core_gui_defs_h_

/// The following minimum system options must be defined for toolbar and menu images to be sized properly:
///"app.mnu.imgwh"
///"app.tb.imgwh"
#define ES_APP_OPT_MAINFRM_MIN_WIDTH  esT("app.mainfrm.minw")
#define ES_APP_OPT_MAINFRM_MIN_HEIGHT esT("app.mainfrm.minh")
#define ES_APP_OPT_MENU_IMG_WH        esT("app.mnu.imgwh")
#define ES_APP_OPT_TOOLBAR_IMG_WH     esT("app.tb.imgwh")
#define ES_APP_OPT_TOOLBAR_MARGINS_WH esT("app.tb.marginwh")
#define ES_APP_OPT_LAYOUT_VGAP        esT("app.layout.vgap")
#define ES_APP_OPT_LAYOUT_HGAP        esT("app.layout.hgap")

/// Common typedefs
///
typedef wxWeakRef<wxMenu> wxMenuWeakRef;
typedef std::shared_ptr<wxMenu> wxMenuSharedPtr;
typedef std::vector<wxMenuItem*> wxMenuItemPtrs;

/// CoreGUI command and window IDs
///
enum {
  /// APPLOG commands
  escoreguiID_APPLOG_REFRESH        = wxID_HIGHEST,
  escoreguiID_ACTIONS_CLEAR_LOG,
  escoreguiID_ACTIONS_LOG_TO_FILE,

  escoreguiID_EXTBASE               = (wxID_HIGHEST+100),   ///< Base ID for escoregui extensions
  escoreguiID_HIGHEST               = (wxID_HIGHEST+1000)   ///< Top ID for escoregui + extensions, may be used as safe base ID value for user core UI commands
};

/// CoreGUI standard ART IDs
///
#define esART_ABORT             wxART_MAKE_ART_ID(esART_ABORT)
#define esART_CLOSE             wxART_MAKE_ART_ID(esART_CLOSE)
#define esART_CLOSE_ALL         wxART_MAKE_ART_ID(esART_CLOSE_ALL)
#define esART_SAVE              wxART_MAKE_ART_ID(esART_SAVE)
#define esART_SAVE_ALL          wxART_MAKE_ART_ID(esART_SAVE_ALL)
#define esART_INFO              wxART_MAKE_ART_ID(esART_INFO)
#define esART_ABOUT             wxART_MAKE_ART_ID(esART_ABOUT)
#define esART_EXIT              wxART_MAKE_ART_ID(esART_EXIT)

#endif //< _es_core_gui_defs_h_
