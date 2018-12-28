#ifndef EsReflectedClassConfigPaneH
#define EsReflectedClassConfigPaneH
//---------------------------------------------------------------------------

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
public:		// User declarations
  EsReflectedClassConfigPane(wxWindow* parent, const EsString& className, const EsMetaclassIntf::Ptr& meta = nullptr);

  EsString supportedTypeNameGet() const;
  void applyFromObject(const EsReflectedClassIntf::Ptr& obj);
  void applyToObject(const EsReflectedClassIntf::Ptr& obj);

  EsReflectedClassDataSource& sourceGet();

  // Return total height of frame contents
  virtual int totalHeightGet() const = 0;

  // True, while setting control properties from the reflected object.
  // False otherwise
  //
  inline bool isApplyingFromObject() const ES_NOTHROW { return m_applyingFromObject; }

protected:
  void sizeChangedPost(int x, int y);

  virtual void doApplyFromObject(const EsReflectedClassIntf::Ptr& obj);
  virtual void doApplyToObject(const EsReflectedClassIntf::Ptr& obj);
  virtual void i18nStringsUpdate(const EsString& loc);

protected:
  wxGridBagSizer m_layContents;
  EsReflectedClassDataSource m_src;
  bool m_applyingFromObject;

private:
  EsReflectedClassConfigPane(const EsReflectedClassConfigPane&) ES_REMOVEDECL;
  EsReflectedClassConfigPane& operator=(const EsReflectedClassConfigPane&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------
#endif
