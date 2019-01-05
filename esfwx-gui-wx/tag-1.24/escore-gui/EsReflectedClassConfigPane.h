#ifndef EsReflectedClassConfigPaneH
#define EsReflectedClassConfigPaneH
//---------------------------------------------------------------------------

/// Object configuration pane interface
///
ES_DECL_INTF_BEGIN(8CC19CF9, D0B24b4c, 8D361982, 26E276BA, EsReflectedObjectConfigPaneIntf)
  /// Return reflected class' name, pane's dedicated of editing
  ES_DECL_INTF_METHOD(EsString, objectClassNameGet)() const ES_NOTHROW = 0;
  /// Access pane's self window
  ES_DECL_INTF_METHOD(wxWindow*, paneGet)() ES_NOTHROW = 0;
  /// Access pane owner (parent)
  ES_DECL_INTF_METHOD(wxWindow*, parentGet)() const ES_NOTHROW = 0;
  /// Access pane contents
  ES_DECL_INTF_METHOD(wxSizer*, contentsGet)() const ES_NOTHROW = 0;
  /// Update pane controls from linked Reflected object source
  ES_DECL_INTF_METHOD(void, controlsUpdateFromObject)(const EsVariant& obj) = 0;
  /// Update Reflected object properties from linked controls
  ES_DECL_INTF_METHOD(void, objectUpdateFromControls)(const EsVariant& obj) const = 0;
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
class ESCORE_GUI_CLASS EsReflectedClassConfigPane : public wxPanel
{
protected:
  class ES_INTF_IMPL1(Impl, EsReflectedObjectConfigPaneIntf)
 
  public:
    Impl(EsReflectedClassConfigPane& pane) ES_NOTHROW;

    /// Minimal EsBaseIntf implementation
    ///
    ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;
    ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE;
 
    /// EsReflectedObjectConfigPaneIntf implementation
    ///
    
    /// Return reflected class' name, pane's dedicated of editing
    ES_DECL_INTF_METHOD(EsString, objectClassNameGet)() const ES_NOTHROW ES_OVERRIDE;
    
    /// Access pane's self window
    ES_DECL_INTF_METHOD(wxWindow*, paneGet)() ES_NOTHROW ES_OVERRIDE;
    
    /// Access pane parent
    ES_DECL_INTF_METHOD(wxWindow*, parentGet)() const ES_NOTHROW ES_OVERRIDE;

    /// Access pane contents
    ES_DECL_INTF_METHOD(wxSizer*, contentsGet)() const ES_NOTHROW ES_OVERRIDE;

    /// Update pane controls from linked Reflected object source
    ES_DECL_INTF_METHOD(void, controlsUpdateFromObject)(const EsVariant& obj) ES_OVERRIDE;
    
    /// Update Reflected object properties from linked controls
    ES_DECL_INTF_METHOD(void, objectUpdateFromControls)(const EsVariant& obj) const ES_OVERRIDE;
  
  private:
    EsReflectedClassConfigPane& m_pane;

  private:
    Impl() ES_REMOVEDECL;
    Impl(const Impl&) ES_REMOVEDECL;
    Impl& operator=(const Impl&) ES_REMOVEDECL;
  
  ES_INTF_IMPL_END
  
  friend class Impl;

protected:
  EsReflectedClassConfigPane(wxWindow* parent, const EsString& className, const EsMetaclassIntf::Ptr& meta = nullptr);

  EsReflectedObjectConfigPaneIntf::Ptr intfGet() ES_NOTHROW;

public:
  EsString supportedTypeNameGet() const;
  void applyFromObject(const EsReflectedClassIntf::Ptr& obj);
  void applyToObject(const EsReflectedClassIntf::Ptr& obj);

  EsReflectedClassDataSource& sourceGet();

  /// True, while setting control properties from the reflected object.
  /// False otherwise
  ///
  inline bool isApplyingFromObject() const ES_NOTHROW { return m_applyingFromObject; }

protected:
  void sizeChangedPost(int x, int y);

  virtual void doApplyFromObject(const EsReflectedClassIntf::Ptr& obj);
  virtual void doApplyToObject(const EsReflectedClassIntf::Ptr& obj);
  virtual void i18nStringsUpdate(const EsString& loc);

  virtual void destroy() ES_NOTHROW;

protected:
  Impl m_intf;
  wxFlexGridSizer* m_layContents;
  EsReflectedClassDataSource m_src;
  bool m_applyingFromObject;

private:
  EsReflectedClassConfigPane() ES_REMOVEDECL;
  EsReflectedClassConfigPane(const EsReflectedClassConfigPane&) ES_REMOVEDECL;
  EsReflectedClassConfigPane& operator=(const EsReflectedClassConfigPane&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------
#endif
