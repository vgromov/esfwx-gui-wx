#ifndef _es_list_view_base_h_
#define _es_list_view_base_h_

/// @file EsListViewBase.h
/// Base class for information or data list view panes.
///

/// Base list ctrl class to present styled data from model
///
class ESCORE_GUI_CLASS EsListViewBase : public wxDataViewCtrl
{
public:
  enum {
    styleDefault        = wxDV_SINGLE |
                          wxDV_HORIZ_RULES |
                          wxDV_VERT_RULES,
    styleUseIndicator   = 0x08000000            ///< Additional user style - display special indicator (leftmost) column
  };

public:
  EsListViewBase(wxWindow* parent, const EsListViewModelBase::Ptr& model, wxWindowID id = wxID_ANY, long style = styleDefault);
  EsListViewBase(wxWindow* parent, wxWindowID id, long style = styleDefault);

  virtual ~EsListViewBase() ES_OVERRIDE;

  EsListViewModelBase::Ptr modelGet() ES_NOTHROW { return m_model; }
  void modelSet(const EsListViewModelBase::Ptr& model);

  bool isEmpty() const ES_NOTHROW { return m_model->isEmpty(); }

  bool hasInidcator() const ES_NOTHROW { return styleUseIndicator == (GetWindowStyle() & styleUseIndicator); }

  void clear();
  void refresh(bool force = false);

  /// Access context menu, initialize it once, if does not exist
  wxMenuSharedPtr ctxMenuGet();

  /// Assign auxiliary context menu object
  void ctxMenuSet(const wxMenuSharedPtr& pmnu);

  /// Access context tool bar, initialize it once, if does not exist
  wxToolBar* toolBarGet(wxWindow* parent);

protected:
  /// Initialize columns from assigned model
  void fromModelInit();

  /// Overridable refresh handler
  virtual void doRefresh();

  /// Overridable model-to-column mapper
  void modelFieldColumnAppend(ulong fldIdx);
  virtual wxDataViewColumn* doCreateColumnFromField(ulong fldIdx, const wxString& type, const wxString& name, const wxString& hint, int minWidth) const;

  /// ES event handlers
  virtual void onI18nLocaleChanged(const EsString& loc);

  virtual void ctxMenuInit() {}
  virtual void toolBarInit() {}
  virtual void evtHandlersSet(wxEvtHandler* subj, bool set) {}

  // WX event handlers
  void onCtxMenu(wxContextMenuEvent& evt);
  void onCtxMenuItem(wxDataViewEvent& evt);

protected:
  EsListViewModelBase::Ptr m_model;
  wxMenuSharedPtr m_pmnu;
  wxToolBar* m_ptb;
  bool m_needRefresh;

  ES_NO_DEFAULT_CTR(EsListViewBase);
  ES_NON_COPYABLE(EsListViewBase);
};

#endif // _es_list_view_base_h_
