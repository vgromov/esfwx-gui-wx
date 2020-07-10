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
  /// Convert EsVariant to wxVariant
  ESCORE_GUI_FUNC(void, wxVariantFromEsVariant(wxVariant& wxvar, const EsVariant& var));
  ESCORE_GUI_FUNC(wxVariant, wxVariantFromEsVariant(const EsVariant& var));

  /// Convert wxVariant to EsVariant
  ESCORE_GUI_FUNC(void, esVariantFromWxVariant(EsVariant& var, const wxVariant& wxvar));
  ESCORE_GUI_FUNC(EsVariant, esVariantFromWxVariant(const wxVariant& wxvar));

  /// Return empty space string (may be used for initializing GUI items, for which the text string should be assigned later at a i18n handling stage)
  ESCORE_GUI_FUNC(const wxString&, wxSpaceString());

  /// Convert wxArrayString to EsString::Array
  ESCORE_GUI_FUNC(void, wxArrayStringToEsStringArray(EsString::Array& dest, const wxArrayString& src));
  ESCORE_GUI_FUNC(EsString::Array, wxArrayStringToEsStringArray(const wxArrayString& src));

  /// Convert EsString::Array to wxArrayString
  ESCORE_GUI_FUNC(void, esStringArrayToWxArrayString(wxArrayString& dest, const EsString::Array& src));
  ESCORE_GUI_FUNC(wxArrayString, esStringArrayToWxArrayString(const EsString::Array& src));

  /// Get MRU files into EsString::Array
  ESCORE_GUI_FUNC(void, mruToEsStringArray(EsString::Array& dest, const wxFileHistory& src));

  /// Set EsString::Array files to MRU
  ESCORE_GUI_FUNC(void, esStringArrayToMru(wxFileHistory& dest, const EsString::Array& src));

  /// Datetime conversions
  ESCORE_GUI_FUNC(wxDateTime, esDateTimeToWxDateTime(const EsDateTime& dt));
  ESCORE_GUI_FUNC(EsDateTime, wxDateTimeToEsDateTime(const wxDateTime& dt));

  /// Extract only time part from wxDateTime
  ESCORE_GUI_FUNC(void, wxDateTimeTimeToEsDateTime(EsDateTime& dest, const wxDateTime& dt));

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

  /// Create menu clone from the source menu object into dest at atPos position, or at the end, if position is not specified.
  /// Return collection of weak reference to the cloned menu top items.
  ///
  ESCORE_GUI_FUNC(wxMenuItemPtrs, menuClone(wxMenu* dest, const wxMenu* src, size_t atPos = static_cast<size_t>(-1)));

  /// Create menu clone from the source menu object.
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
  ESCORE_GUI_FUNC(int, longestStringWidthGet(const EsString::Array& strings, const wxWindow& wnd, const wxFont& fnt = wxNullFont));
  ESCORE_GUI_FUNC(int, longestStringWidthGet(const wxArrayString& strings, const wxWindow& wnd, const wxFont& fnt = wxNullFont));

  /// Menu and Toolbar helpers
  ///

  /// Split hint string into short and long parts (using '|' as separator char)
  ESCORE_GUI_FUNC(wxString, shortHintGet(const wxString& hint));
  ESCORE_GUI_FUNC(wxString, longHintGet(const wxString& hint));
  
  /// Create tool from menu item with specified ID
  ESCORE_GUI_FUNC(wxAuiToolBarItem*, toolbarToolAddFromMenuItem(wxAuiToolBar* tb, wxMenu* mnu, int id, const wxArtClient& cli = wxEmptyString, const wxArtID& bm = wxEmptyString, const wxArtID& bmDisabled = wxEmptyString, const wxString& helpLong = wxEmptyString));

  /// Create tool from menu item with specified ID
  ESCORE_GUI_FUNC(wxToolBarToolBase*, toolbarToolAddFromMenuItem(wxToolBar* tb, wxMenu* mnu, int id, const wxArtClient& cli = wxEmptyString, const wxArtID& bm = wxEmptyString, const wxArtID& bmDisabled = wxEmptyString, const wxString& helpLong = wxEmptyString));

  /// @brief  Try to update menu item label, hint, and, optionally, a bitmap, if needed.
  ///          
  /// @param  menu    [in] Menu, for whose item with itemId we'll try to update label, hint, bitmap
  /// @param  itemId  [in] Menu item ID
  /// @param  label   [in] Menu item label to set
  /// @param  hint    [in] Menu item hint to set
  /// @param  bm      [in] Menu item bitmap to set, depending on item state
  ///                 
  /// @return              true, if item label and/or bitmap, and/or hint were changed, false otherwise
  ///
  ESCORE_GUI_FUNC(bool, menuItemLabelAndHintUpdate(wxMenuBase* menu, wxWindowID itemId, const wxString& label, const wxString& hint, const wxBitmap& bm = wxNullBitmap));

  ESCORE_GUI_FUNC(bool, toolbarItemLabelAndHintUpdate(wxToolBarBase* tb, wxWindowID itemId, const wxString& label, const wxString& hint, const wxBitmap& bm = wxNullBitmap, bool doUpdateTarget = true));

  ESCORE_GUI_FUNC(bool, toolbarItemLabelAndHintUpdate(wxAuiToolBar* tb, wxWindowID itemId, const wxString& label, const wxString& hint, const wxBitmap& bm = wxNullBitmap, bool doUpdateTarget = true));

  ESCORE_GUI_FUNC(bool, menuOrToolbarItemLabelAndHintUpdate(wxObject* menuOrToolbar, wxWindowID itemId, const wxString& label, const wxString& hint, const wxBitmap& bm = wxNullBitmap, bool doUpdateTarget = true));

  /// @brief  Update toolbar tool labels and hints from menu items by their IDs, if there are any matches
  /// @param  tb        [in]  Target toolbar.
  /// @param  menu      [in]  Source menu
  ///
  ESCORE_GUI_FUNC(void, toolbarItemsLabelsAndHintsUpdateFromMenu(wxToolBarBase* tb, wxMenuBase* menu));

  /// Standard application settings retrieval
  ESCORE_GUI_FUNC(wxSize, appSizePropertyGet(const wxString& prop));

  /// Standard application main form min size retrieval
  ESCORE_GUI_FUNC(wxSize, appMainFrameMinSizeGet());

  /// Try to get typed validator from control, return nullptr, if not set or is not of requested type
  template <typename ValidatorT>
  ValidatorT* validatorGetAs(wxWindow* wnd)
  {
    if( !wnd )
      return nullptr;

    return dynamic_cast<ValidatorT*>( wnd->GetValidator() );
  }

  /// Assign wxObjectDataPtr for related types
  template <typename Obj1T, typename Obj2T>
  wxObjectDataPtr<Obj1T> wxObjDataPtrFromOther( const wxObjectDataPtr<Obj2T>& other )
  {
    wxObjectDataPtr<Obj2T> src = other;
    wxObjectDataPtr<Obj1T> dest( src.get() );
    if( dest )
      dest->IncRef();

    return dest;
  }

} // namespace EsUtilities

#endif // _es_core_gui_utilities_h_
