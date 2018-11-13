#ifndef _es_core_gui_utilities_h_
#define _es_core_gui_utilities_h_

enum class EsStdBitmapId {
  Null,
  CheckUnchecked,
  CheckChecked,
  RadioUnchecked,
  RadioChecked
};

namespace EsUtilities
{
  /// Return empty space string (may be used for initializing GUI items, for which the text string should be assigned later at a i18n handling stage)
  ESCORE_GUI_FUNC(const wxString&, wxSpaceString());

  /// Convert wxArrayString to EsString::Array
  ESCORE_GUI_FUNC(void, wxArrayStringToEsStringArray(EsString::Array& dest, const wxArrayString& src));

  /// Convert EsString::Array to wxArrayString
  ESCORE_GUI_FUNC(void, esStringArrayToWxArrayString(wxArrayString& dest, const EsString::Array& src));

  /// Get MRU files into EsString::Array
  ESCORE_GUI_FUNC(void, mruToEsStringArray(EsString::Array& dest, const wxFileHistory& src));

  /// Set EsString::Array files to MRU
  ESCORE_GUI_FUNC(void, esStringArrayToMru(wxFileHistory& dest, const EsString::Array& src));

  /// Window position serialization
  ESCORE_GUI_FUNC(void, windowPosSave(const EsStreamIntf::Ptr& out, const wxPoint& pos));
  ESCORE_GUI_FUNC(void, windowPosSave(const EsStreamIntf::Ptr& out, const wxWindow* wnd));

  ESCORE_GUI_FUNC(void, windowPosLoad(const EsStreamIntf::Ptr& in, wxPoint& pos, const wxPoint& defaultPos));
  ESCORE_GUI_FUNC(void, windowPosLoad(const EsStreamIntf::Ptr& in, wxWindow* wnd, const wxSize& minSize));

  /// Window state serialization
  ESCORE_GUI_FUNC(void, windowStateSave(const EsStreamIntf::Ptr& out, const wxFrame* wnd));
  ESCORE_GUI_FUNC(void, windowStateLoad(const EsStreamIntf::Ptr& in, wxFrame* wnd, long fullScreenStyle = wxFULLSCREEN_ALL));

  /// GUI adjustments
  ESCORE_GUI_FUNC(void, menuFontApply(wxMenuBar* mnu, const wxFont& fnt));
  ESCORE_GUI_FUNC(void, ctlsFontApply(wxWindow* wnd, const wxFont& fnt));

  /// Return top level window given the current one
  ESCORE_GUI_FUNC(wxWindow*, topLevelWindowGetFor(wxWindow* wnd));

  /// Standard (stock) images
  ESCORE_GUI_FUNC(const wxBitmap&, stdBitmapGet(EsStdBitmapId id));

  /// Menu helpers
  ///

  /// Generic item
  ESCORE_GUI_FUNC(wxMenuItem*, menuItemCreate(wxMenu* parent, int id, const wxString& label, const wxString& help,
	  wxItemKind kind, const wxBitmap& normalChecked,	const wxBitmap& unchecked, wxMenu* submenu));
  
  /// Normal item
  ESCORE_GUI_FUNC(wxMenuItem*, menuNormalItemCreate(wxMenu* parent, int id, const wxString& label,
	  const wxString& help, const wxBitmap& normal = wxNullBitmap, wxMenu* submenu = nullptr));
  
  /// Check item
  ESCORE_GUI_FUNC(wxMenuItem*, menuCheckItemCreate(wxMenu* parent, int id, const wxString& label,
	  const wxString& help, const wxBitmap& checked = wxNullBitmap, const wxBitmap& unchecked = wxNullBitmap));

  /// Radio item
  ESCORE_GUI_FUNC(wxMenuItem*, menuRadioItemCreate(wxMenu* parent, int id, const wxString& label,
	  const wxString& help, const wxBitmap& checked = wxNullBitmap, const wxBitmap& unchecked = wxNullBitmap));
  
  /// Create language selection menu
  ESCORE_GUI_FUNC(wxMenu*, menuLanguageSelectionCreate(const EsString::Array& langs, int startId));

  /// Assign specific bitmap to menu item by its ID, using wxArtProvider
  ESCORE_GUI_FUNC(void, menuItemBitmapSet(wxMenu* mnu, int id, const wxSize& bmSize, const wxArtClient& client, const wxArtID& bm, const wxArtID& bmDisabled = wxEmptyString));

  /// Create menu clone from the source menu object
  ESCORE_GUI_FUNC(void, menuClone(wxMenu* dest, const wxMenu* src));
  ESCORE_GUI_FUNC(wxMenu*, menuCloneCreate(const wxMenu* src));

  /// Message Prompt helpers
  ///

  /// Our own message dialog implementation (use centralized font settings)
  ESCORE_GUI_FUNC(int, messageShow(const EsString& msg, const EsString& caption, long lStyle = wxOK|wxCANCEL, const wxPoint& pos = wxDefaultPosition));

  /// Show modal error message box
  ESCORE_GUI_FUNC(void, errorShow(const EsString& msg, const EsString& caption = _("Error") ));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10));
  ESCORE_GUI_FUNC(void, errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11));

  /// Get longest string width in pixels, using wnd visual settings
  ESCORE_GUI_FUNC(int, longestStringWidthGet(const EsString::Array& strings, const wxWindow& wnd, const wxFont& fnt));

  /// wxAUI Toolbar helpers
  ///

  /// Create tool from menu item with specified ID
  ESCORE_GUI_FUNC(wxAuiToolBarItem*, toolbarToolAddFromMenuItem(wxAuiToolBar* tb, wxMenu* mnu, int id, const wxArtClient& cli = wxEmptyString, const wxArtID& bm = wxEmptyString, const wxArtID& bmDisabled = wxEmptyString));

  /// Create tool from menu item with specified ID
  ESCORE_GUI_FUNC(wxToolBarToolBase*, toolbarToolAddFromMenuItem(wxToolBar* tb, wxMenu* mnu, int id, const wxArtClient& cli = wxEmptyString, const wxArtID& bm = wxEmptyString, const wxArtID& bmDisabled = wxEmptyString));

  /// Standard application settings retrieval
  ESCORE_GUI_FUNC(wxSize, appSizePropertyGet(const wxString& prop));

  /// Standard application main form min size retrieval
  ESCORE_GUI_FUNC(wxSize, appMainFrameMinSizeGet());

} // namespace EsUtilities

#endif // _es_core_gui_utilities_h_
