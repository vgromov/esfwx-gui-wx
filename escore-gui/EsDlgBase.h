#ifndef EsDlgBaseH
#define EsDlgBaseH
//---------------------------------------------------------------------------

class ESCORE_GUI_CLASS EsDlgBase : public wxDialog
{
private:
  class EvtHandler : public EsEventSubscriber
  {
  public:
    EvtHandler(EsDlgBase& owner);

    ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);
  
  protected:
    EsDlgBase& m_owner;

  private:
    EvtHandler() ES_REMOVEDECL;
    EvtHandler(const EvtHandler&) ES_REMOVEDECL;
    EvtHandler& operator=(const EvtHandler&) ES_REMOVEDECL;
  };
  friend class EvtHandler;

public:		// User declarations
	EsDlgBase(wxWindow* parent, const wxString& title = wxEmptyString, long style = wxDEFAULT_DIALOG_STYLE);
	virtual ~EsDlgBase();

  wxBoxSizer& contentsAccess() { return *m_contents; }
  wxStdDialogButtonSizer& btnsPaneAccess() { return *m_btns; }

  void showModal(std::function<void(const EsDlgBase& Sender, int modalResult)> onClose);

  void payloadSet(const EsVariant& payload) ES_NOTHROW { m_payload = payload; }
  const EsVariant& payloadGet() const ES_NOTHROW { return m_payload; }

protected:
  virtual void onNotify(const EsEventIntf::Ptr& evt);
  virtual void onI18nLocaleChanged(const EsString& loc);

  void contentsFinalize();

protected:
  EvtHandler m_handler;
  wxString m_title;
  wxBoxSizer* m_internals;
  wxBoxSizer* m_contents;
  wxStdDialogButtonSizer* m_btns;
  EsVariant m_payload;
  bool m_contentsFinalized;

private:
  EsDlgBase(const EsDlgBase&) ES_REMOVEDECL;
  EsDlgBase& operator=(const EsDlgBase&) ES_REMOVEDECL;
};

//---------------------------------------------------------------------------
#endif
