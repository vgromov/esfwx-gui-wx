#ifndef _es_progress_monitor_view_h_
#define _es_progress_monitor_view_h_

/// Forward declarations
class EsProgressMonitorView;

/// Progress monitor view item class
///
class ESCORE_GUI_CLASS EsProgressMonitorViewItem : public wxPanel
{
protected:
  EsProgressMonitorViewItem(wxWindow* parent, const EsString& path);

public:
  void pulsingSet(bool pulsing);
  void textSet(const EsString& txt);
  void progressTextSet(const EsString& txt);
  void progressSet(const EsVariant& val);

protected:
  void nestedItemAdd(EsProgressMonitorViewItem* item);
  void nestedItemRemove(EsProgressMonitorViewItem* item);
  void reLayout();

  void onPulsing(wxTimerEvent& evt);

protected:
  wxTimer m_tmrPulser;
  wxStaticText* m_txt;
  wxStaticText* m_txtProgress;
  wxGauge* m_progress;
  wxBoxSizer* m_contents;
  bool m_pulsing;

private:
  EsProgressMonitorViewItem() ES_REMOVEDECL;
  EsProgressMonitorViewItem(const EsProgressMonitorViewItem&) ES_REMOVEDECL;
  EsProgressMonitorViewItem& operator=(const EsProgressMonitorViewItem&) ES_REMOVEDECL;

  friend class EsProgressMonitorView;
};

/// Progress monitor view class
///
class ESCORE_GUI_CLASS EsProgressMonitorView : public wxPanel
{
public:
  typedef std::function<void(EsProgressMonitorView&)> NotificationT;

private:
  class EvtHandler : public EsEventSubscriber
  {
  public:
    EvtHandler(EsProgressMonitorView& owner);
    ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

  protected:
    EsProgressMonitorView& m_owner;
  };
  friend class EvtHandler;

public:
  EsProgressMonitorView(wxWindow* parent);

  /// Progress monitor interface setter
  void progressMonitorSet(const EsProgressMonitorIntf::Ptr& mon);

  /// Progress monitor events
  ///

  /// Event is fired if progress monitor reflects some activity
  void onActivitySet(NotificationT fn) ES_NOTHROW { m_onActivity = fn; }

  /// Event is fired if progress monitor becomes empty (ready, AKA idle state)
  void onReadySet(NotificationT fn) ES_NOTHROW { m_onReady = fn; }

protected:
  // Internal view services
  void viewClear();
  void viewRebuild();
  void reLayout();
  EsProgressMonitorViewItem* taskItemFind(const EsString& path, size_t& idx) const;
  EsProgressMonitorViewItem* parentTaskItemFind(const EsString& path) const;
  EsProgressMonitorViewItem* taskViewCreate(const EsProgressMonitorTaskIntf::Ptr& task);
  EsProgressMonitorViewItem* taskViewCreate(
    const EsString& path, 
    const EsString& text,
    const EsString& progressText, 
    const EsVariant& completion
  );
  void taskViewDestroy(const EsString& path);
  void newItemInit(
    EsProgressMonitorViewItem* newItem,
    const EsString& path,
    const EsString& text,
    const EsString& progressText,
    const EsVariant& completion
  );

  // EsEvent handlers
  void onTaskTextChanged(const EsVariant& payload);
  void onTaskStateChanged(const EsVariant& payload);
  void onTaskProgressChanged(const EsVariant& payload);
  void onTaskAdded(const EsVariant& payload);
  void onTaskRemoved(const EsVariant& payload);

protected:
  EvtHandler m_handler;
  EsProgressMonitorIntf::Ptr m_mon;
  EsStringIndexedMap m_items;
  wxBoxSizer* m_contents;
  NotificationT m_onActivity;
  NotificationT m_onReady;

private:
  EsProgressMonitorView() ES_REMOVEDECL;
  EsProgressMonitorView(const EsProgressMonitorView&) ES_REMOVEDECL;
  EsProgressMonitorView& operator=(const EsProgressMonitorView&) ES_REMOVEDECL;

  friend class EsProgressMonitorViewItem;
};
//---------------------------------------------------------------------------
#endif // _es_progress_monitor_view_h_
