#ifndef _es_list_view_base_h_
#define _es_list_view_base_h_

/// @file EsListViewBase.h
/// Base class for information list view panes.
/// Simple non-editable list of messages, optionally styled, and with icons.
///

/// EsListViewStyle - collection of list item style attributes mapped by integer key
///
class ESCORE_GUI_CLASS EsListViewStyle
{
public:
  bool isEmpty() const ES_NOTHROW { return m_attrs.empty(); }

  EsListViewStyle& attrSet(int key, const wxColour &colText, const wxColour &colBack, const wxFont &font = wxNullFont);
  wxListItemAttr* attrGet(int key) const;
  void attrDelete(int key);

private:
  std::map<int, wxListItemAttr> m_attrs;
};
//--------------------------------------------------------------------------------

/// ListCtrl data model abstraction
///
class ESCORE_GUI_CLASS EsListViewModel
{
public:
  typedef std::shared_ptr<EsListViewModel> Ptr;

protected:
  EsListViewModel() ES_NOTHROW {}

public:
  /// Create and return new data model object
  static EsListViewModel::Ptr create();

  /// Return true if there is no data records
  bool isEmpty() const ES_NOTHROW { return m_data.empty(); }

  /// Append field metadata, and return reference to the modle object, to 
  /// augment chained fieldAdd calls
  ///
  EsListViewModel& fieldAdd(const wxString& name, ulong minWidthHint = 0, const wxString& hint = wxEmptyString);

  /// Return total data fields count
  ulong fieldCountGet() const ES_NOTHROW { return m_fldNames.size(); }

  /// Return total data records count
  ulong recordCountGet() const ES_NOTHROW { return m_data.size(); }

  /// Return field name, if any
  const wxString& fieldNameGet(ulong idx) const { return m_fldNames[idx]; }

  /// Return field mind width hint, if any
  int fieldMinWidthGet(ulong idx) const { return m_fldMinWidthHints[idx]; }

  /// Return field hint, if any
  const wxString& fieldHintGet(ulong idx) const { return m_fldHints[idx];  }

  /// Return textual data representation for speciific field
  virtual wxString fieldTextGet(ulong recIdx, ulong idx) const;

  /// Return raw data for speciific field
  const EsVariant& fieldDataGet(ulong recIdx, ulong idx) const;

  /// Find record(s) by one or more field values
  /// @param   [in]  - Collection of the field indexes to look up, or the single index value
  /// @param   [in]  - Collection of the field values to look up, or the single value. Must have the same size as 
  ///                  the field indexes collection
  /// @return        - Collection of the found record indexes, or empty variant, if no records were found
  ///
  EsVariant recordsFind(const EsVariant& fldIndexes, const EsVariant& fldValues) const;

  /// Individual record access
  const EsVariant::Array& recordGet(ulong recIdx) const;

  /// Rewrite record data at specified record index
  void recordSet(ulong recIdx, const EsVariant::Array& record);

  /// Append record to the end of container, return newly added record index
  ulong recordAppend(const EsVariant::Array& record);

  /// Remove record at specified index
  void recordDelete(ulong recIdx);

  /// Remove all data records from model
  void clear();

  /// Get all existing data in container as string
  wxString asString() const;

  /// Get record as string
  wxString asString(ulong recIdx) const;

  /// Get only selected portion of existing data as string
  wxString asString(ulong fldStart, ulong fldEnd, ulong recStart, ulong recEnd) const;

protected:
  wxArrayString m_fldNames;
  wxArrayString m_fldHints;
  std::vector<EsVariant::Array> m_data;
  wxArrayInt m_fldMinWidthHints;

private:
  EsListViewModel(const EsListViewModel&) ES_REMOVEDECL;
  EsListViewModel& operator=(const EsListViewModel&) ES_REMOVEDECL;
};
//--------------------------------------------------------------------------------

/// Base list ctrl class to present styled data from model
///
class ESCORE_GUI_CLASS EsListViewBase : public wxListCtrl
{
public:
  EsListViewBase(wxWindow* parent, const EsListViewModel::Ptr& model, wxWindowID id = wxID_ANY);
  EsListViewBase(wxWindow* parent, wxWindowID id = wxID_ANY);
  virtual ~EsListViewBase() ES_OVERRIDE;
  
  EsListViewStyle& styleAccess() ES_NOTHROW { return m_style; }

  EsListViewModel::Ptr modelGet() ES_NOTHROW { return m_model; }
  void modelSet(const EsListViewModel::Ptr& model);

  bool isEmpty() const ES_NOTHROW { return m_model->isEmpty(); }
  void clear();
  void refresh(bool force = false);

  /// Virtual list interface implementation
  ///
  virtual wxString OnGetItemText(long item, long column) const { return m_model->fieldTextGet(item, column); }
  virtual wxListItemAttr* OnGetItemAttr(long item) const { return m_style.attrGet(keyFromItemGet(item)); }

  wxMenuPtr ctxMenuGet();
  wxToolBar* toolBarGet(wxWindow* parent);

protected:
  void fromModelInit();

  /// ES event handlers
  virtual void onI18nLocaleChanged(const EsString& loc);

  virtual void ctxMenuInit() {}
  virtual void toolBarInit() {}
  virtual void evtHandlersSet(wxEvtHandler* subj, bool set) {}

  /// To be implemented in derived classes
  virtual int keyFromItemGet(long item) const = 0;

  // WX event handlers
  void onCtxMenu(wxContextMenuEvent& evt);

protected:
  EsListViewStyle m_style;
  EsListViewModel::Ptr m_model;
  wxMenuPtr m_pmnu;
  wxToolBar* m_ptb;

  bool m_needRefresh;
};


#endif // _es_list_view_base_h_
