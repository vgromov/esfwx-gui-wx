#ifndef _es_opened_document_list_h_
#define _es_opened_document_list_h_

class ESMDIGUI_CLASS EsOpenedDocumentList : public EsListViewBase
{
private:
  // ES Event subscriber
  class EvtHandler : public EsEventSubscriber
  {
  public:
    EvtHandler(EsOpenedDocumentList& This) ES_NOTHROW;

    ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

  protected:
    EsOpenedDocumentList& m_this;
  };
  friend class EvtHandler;

public:
  EsOpenedDocumentList(wxWindow* parent, wxWindowID id = wxID_ANY);

protected:
  virtual void ctxMenuInit() ES_OVERRIDE;
  virtual void toolBarInit() ES_OVERRIDE;
  virtual void evtHandlersSet(wxEvtHandler* subj, bool set) ES_OVERRIDE;

  // ES event handlers
  void onDocumentCreated(const EsVariant& key);
  void onDocumentOpened(const EsVariant& key);
  void onDocumentClosed(const EsVariant& key);
  void onDocumentKeyChanged(const EsVariant& keyOld, const EsVariant& keyNew);
  void onDocumentActivated(const EsVariant& key);
  void onI18nLocaleChanged(const EsString& lang);

  // WX event handlers
  void onDocumentSelected(wxDataViewEvent& evt);
  void onCloseRequest(wxCommandEvent& evt);
  void onCloseAllRequest(wxCommandEvent& evt);
  void onUpdateUi(wxUpdateUIEvent& evt);

protected:
  EvtHandler m_evtHandler;
  EsDocumentManagerIntf::Ptr m_docmgr;

  ES_NO_DEFAULT_CTR(EsOpenedDocumentList);
  ES_NON_COPYABLE(EsOpenedDocumentList);
};

#endif // _es_opened_document_list_h_
