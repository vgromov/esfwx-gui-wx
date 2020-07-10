#ifndef _es_app_log_h_
#define _es_app_log_h_

/// Log entries presented in list control
///
class ESCORE_GUI_CLASS EsAppLog : public EsListViewBase
{
private:
  class EvtHandler : public EsEventSubscriber
  {
  public:
    EvtHandler(EsAppLog& owner);
    
    ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

  private:
    EsAppLog& m_owner;

    ES_NO_DEFAULT_CTR(EvtHandler);
    ES_NON_COPYABLE(EvtHandler);
  };
	friend class EvtHandler;
	
public:
	enum { 
		// Logger entries count restriction
    MAX_LOG_DEPTH = 32000, 
		MIN_LOG_DEPTH = 100,
	};

public:
	EsAppLog( wxWindow* parent, int logDepth = MAX_LOG_DEPTH );
	virtual ~EsAppLog();
	
  /// Base class overrides
  ///
  virtual void clear();

	/// Disable|enable file logging
	bool isFileLoggingEnabled() const ES_NOTHROW { return m_doFileLogging; }
	void enableFileLogging(bool enable);

  /// Settings persistence
  ///
  void settingsLoad(const EsStreamIntf::Ptr& in);
  void settingsSave(const EsStreamIntf::Ptr& out);

  static EsString::const_pointer levelStringGet(EsAppLogLevel lvl);

protected:
  virtual void doRefresh();

  virtual void ctxMenuInit() ES_OVERRIDE;
  virtual void toolBarInit() ES_OVERRIDE;
  virtual void evtHandlersSet(wxEvtHandler* subj, bool set) ES_OVERRIDE;

  // ES event handlers
  void onLogEntryAdd(const EsDateTime& ts, EsAppLogLevel lvl, const EsString& msg, const EsVariant& payload);
  void onI18nLocaleChanged(const EsString& loc);

	// WX event handlers
  void onClear(wxCommandEvent& evt);
  void onLogToFile(wxCommandEvent& evt);
  void onUpdateUi(wxUpdateUIEvent& evt);
  void onRefresh(wxTimerEvent& evt);

	static EsString constructNewLogFileName();

protected:
  EvtHandler m_handler;
  wxFile m_flog;
	wxTimer m_tmrRefresh;
  ulong m_logDepth;
	bool m_doFileLogging;  
  bool m_debugData;

  ES_NO_DEFAULT_CTR(EsAppLog);
  ES_NON_COPYABLE(EsAppLog);
};
//--------------------------------------------------------------------------------

#endif //< _es_app_log_h_
