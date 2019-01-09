#ifndef EsReflectedClassConfigPaneH
#define EsReflectedClassConfigPaneH
//---------------------------------------------------------------------------

/// Object configuration pane interface
///
ES_DECL_INTF_BEGIN(8CC19CF9, D0B24b4c, 8D361982, 26E276BA, EsReflectedObjectConfigPaneIntf)
  /// Return reflected class' name, pane's dedicated of editing
  ES_DECL_INTF_METHOD(EsString, supportedTypeNameGet)() const ES_NOTHROW = 0;
  /// Access underlying reflected data source
  ES_DECL_INTF_METHOD(EsReflectedClassDataSource&, dataSourceAccess)() ES_NOTHROW = 0;
  /// Access pane's self window
  ES_DECL_INTF_METHOD(wxWindow*, paneGet)() ES_NOTHROW = 0;
  /// Access pane contents
  ES_DECL_INTF_METHOD(wxSizer*, contentsGet)() const ES_NOTHROW = 0;
  /// Update pane controls from linked Reflected object source
  ES_DECL_INTF_METHOD(void, controlsUpdateFromObject)(const EsVariant& obj) = 0;
  /// Update Reflected object properties from linked controls
  ES_DECL_INTF_METHOD(void, objectUpdateFromControls)(const EsVariant& obj) = 0;
  /// Reset configuration controls to default values
  ES_DECL_INTF_METHOD(void, controlsResetToDefault)() = 0;
ES_DECL_INTF_END

/// Configuration pane events category
///
#define ES_EVTC_CLASSCFG_PANE_NOTIFICATION esT("EsReflectedClassConfigPane")

enum class EsReflectedClassConfigPaneEvent : ulong
{
  SizeChanged = 1,  ///< payload[0] contains pointer to pane instance, which size was changed,
                    ///  payload[1] = x (int), payload[2] = y (int)
  PaneCreated = 2,  ///< payload[0] contains pointer to event source instance (sender),
                    ///  payload[1] = pinter to created pane instance
};

/// Base class for reflected objects configuration panes
///
class ESCORE_GUI_CLASS ES_INTF_IMPL1(EsReflectedClassConfigPane, EsReflectedObjectConfigPaneIntf)

protected:
  /// Base class, implementing configuration pane window
  ///
  class ESCORE_GUI_CLASS PaneWnd : public wxPanel
  {
  public:
    PaneWnd(EsReflectedClassConfigPane& intf, wxWindow* parent) ES_NOTHROW;

    /// True, while setting control properties from the reflected object.
    /// False otherwise
    ///
    bool isApplyingFromObject() const ES_NOTHROW;

    inline wxFlexGridSizer* contentsGet() const ES_NOTHROW { return m_layContents; }

  protected:
    void sizeChangedPost(int x, int y);

  protected:
    EsReflectedClassConfigPane& m_intf;
    wxFlexGridSizer* m_layContents;
    bool m_orphan;

  private:
    PaneWnd() ES_REMOVEDECL;
    PaneWnd(const PaneWnd&) ES_REMOVEDECL;
    PaneWnd& operator=(const PaneWnd&) ES_REMOVEDECL;

    friend class EsReflectedClassConfigPane;
  };
  friend class PaneWnd;
  
protected:
  EsReflectedClassConfigPane();

public:
  virtual ~EsReflectedClassConfigPane() ES_NOTHROW;

  /// EsReflectedObjectConfigPaneIntf implementation
  ///
    
  /// Return reflected class' name, pane's meant to editing
  ES_DECL_INTF_METHOD(EsString, supportedTypeNameGet)() const ES_NOTHROW ES_OVERRIDE;
    
  /// Access pane's self window
  ES_DECL_INTF_METHOD(wxWindow*, paneGet)() ES_NOTHROW ES_OVERRIDE { return m_pane; }

  /// Access underlying reflected data source
  ES_DECL_INTF_METHOD(EsReflectedClassDataSource&, dataSourceAccess)() ES_NOTHROW ES_OVERRIDE { ES_ASSERT(m_src); return *m_src.get(); }
    
  /// Access pane contents
  ES_DECL_INTF_METHOD(wxSizer*, contentsGet)() const ES_NOTHROW ES_OVERRIDE { if(m_pane) return m_pane->contentsGet(); return nullptr; }

  /// Update pane controls from linked Reflected object source
  ES_DECL_INTF_METHOD(void, controlsUpdateFromObject)(const EsVariant& obj) ES_OVERRIDE;
    
  /// Update Reflected object properties from linked controls
  ES_DECL_INTF_METHOD(void, objectUpdateFromControls)(const EsVariant& obj) ES_OVERRIDE;

  /// Reset configuration controls to default values
  ES_DECL_INTF_METHOD(void, controlsResetToDefault)() ES_OVERRIDE;

protected:
  void init(wxWindow* parent, const EsString& className, const EsMetaclassIntf::Ptr& meta);

  virtual PaneWnd* doPaneWndCreate(wxWindow* parent) = 0;
  virtual EsReflectedClassDataSource* doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta) = 0;

  virtual void doApplyFromObject(const EsReflectedClassIntf::Ptr& obj);
  virtual void doApplyToObject(const EsReflectedClassIntf::Ptr& obj);
  virtual void i18nStringsUpdate(const EsString& loc);

protected:
  wxWeakRef<PaneWnd> m_pane;
  std::unique_ptr<EsReflectedClassDataSource> m_src;
  bool m_applyingFromObject;

private:
  EsReflectedClassConfigPane(const EsReflectedClassConfigPane&) ES_REMOVEDECL;
  EsReflectedClassConfigPane& operator=(const EsReflectedClassConfigPane&) ES_REMOVEDECL;

ES_INTF_IMPL_END
//---------------------------------------------------------------------------
#endif
