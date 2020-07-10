#include "esmdi-guipch.h"
#pragma hdrstop

#include "EsOpenedDocumentList.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsOpenedDocumentList::EvtHandler::EvtHandler(EsOpenedDocumentList& This) ES_NOTHROW :
EsEventSubscriber(
  ES_EVTC_MDI ES_EVTC_AND
  ES_EVTC_MDIGUI ES_EVTC_AND
  ES_EVTC_I18N
),
m_this(This)
{}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::EvtHandler::notify(const EsEventIntf::Ptr& evt)
{
  ES_ASSERT(evt);

  const EsString& cat = evt->categoryGet();
  ulong id = evt->idGet();
  const EsVariant& payload = evt->payloadGet();

  if(ES_EVTC_MDI == cat)
  {
    switch(as_<EsMdiEvt>(id))
    {
    case EsMdiEvt::DocumentCreated:
      m_this.onDocumentCreated(
        payload[0]
      );
      break;
    case EsMdiEvt::DocumentOpened:
      m_this.onDocumentOpened(
        payload[0]
      );
      break;
    case EsMdiEvt::DocumentClosed:
      m_this.onDocumentClosed(
        payload[0]
      );
      break;
    case EsMdiEvt::DocumentKeyChanged:
      m_this.onDocumentKeyChanged(
        payload[0],
        payload[1]
      );
      break;
    }
  }
  else if(ES_EVTC_MDIGUI == cat)
  {
    switch( as_<EsMdiGuiEvt>(id) )
    {
    case EsMdiGuiEvt::DocumentActivated:
      {
        EsWxWindowPtrIntf::Ptr wndRefPtr = payload[1].asObject();
        if( !wndRefPtr || wndRefPtr->windowRefGet() == &m_this )
          m_this.onDocumentActivated(
            payload[0]
          );
      }
      break;
    }
  }
  else if(
    ES_EVTC_I18N == cat && 
    EsEvtI18n::LocaleChanged == as_<EsEvtI18n>(id)
  )
  {
    m_this.onI18nLocaleChanged(
      evt->payloadGet().asString()
    );
  }
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsOpenedDocumentList::EsOpenedDocumentList(wxWindow* parent, wxWindowID id /*= wxID_ANY*/) :
EsListViewBase(parent, id),
m_evtHandler(*this)
{
  // Set-up model
  EsListViewModelBase::Ptr model = EsListViewModelArray::create();
  ES_ASSERT(model);

  model->fieldAdd(
    _i("Name"),
    "string",
    100
  ).
  fieldAdd(
    _i("Path"),
    "string",
    300
  ).
  fieldAdd(
    _i("Type"),
    "string",
    60
  );
  modelSet(model);

  Bind(
    wxEVT_DATAVIEW_ITEM_ACTIVATED,
    &EsOpenedDocumentList::onDocumentSelected,
    this
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::ctxMenuInit()
{
  ES_ASSERT(m_pmnu);

  m_pmnu->Append(
    EsMdiManagerView::esmdiID_DOC_CLOSE,
    EsUtilities::wxSpaceString(),
    EsUtilities::wxSpaceString()
  );
  m_pmnu->Append(
    EsMdiManagerView::esmdiID_DOC_CLOSE_ALL,
    EsUtilities::wxSpaceString(),
    EsUtilities::wxSpaceString()
  );

  const wxSize& appmnuimgsze = EsUtilities::appSizePropertyGet(ES_APP_OPT_MENU_IMG_WH);

  EsUtilities::menuItemBitmapSet(
    m_pmnu.get(),
    EsMdiManagerView::esmdiID_DOC_CLOSE,
    appmnuimgsze,
    wxART_OTHER,
    esART_CLOSE
  );

  EsUtilities::menuItemBitmapSet(
    m_pmnu.get(),
    EsMdiManagerView::esmdiID_DOC_CLOSE_ALL,
    appmnuimgsze,
    wxART_OTHER,
    esART_CLOSE_ALL
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::toolBarInit()
{
  wxMenuSharedPtr pmnu = ctxMenuGet();

  EsUtilities::toolbarToolAddFromMenuItem(
    m_ptb,
    pmnu.get(),
    EsMdiManagerView::esmdiID_DOC_CLOSE,
    wxART_TOOLBAR,
    esART_CLOSE
  );
  EsUtilities::toolbarToolAddFromMenuItem(
    m_ptb,
    pmnu.get(),
    EsMdiManagerView::esmdiID_DOC_CLOSE_ALL,
    wxART_TOOLBAR,
    esART_CLOSE_ALL
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::evtHandlersSet(wxEvtHandler* subj, bool set)
{
  ES_ASSERT(subj);

  if( set )
  {
    subj->Bind(
      wxEVT_MENU,
      &EsOpenedDocumentList::onCloseRequest,
      this,
      EsMdiManagerView::esmdiID_DOC_CLOSE
    );
    subj->Bind(
      wxEVT_MENU,
      &EsOpenedDocumentList::onCloseAllRequest,
      this,
      EsMdiManagerView::esmdiID_DOC_CLOSE_ALL
    );
    subj->Bind(
      wxEVT_UPDATE_UI,
      &EsOpenedDocumentList::onUpdateUi,
      this,
      EsMdiManagerView::esmdiID_DOC_CLOSE,
      EsMdiManagerView::esmdiID_DOC_CLOSE_ALL
    );
  }
  else
  {
    subj->Unbind(
      wxEVT_MENU,
      &EsOpenedDocumentList::onCloseRequest,
      this,
      EsMdiManagerView::esmdiID_DOC_CLOSE
    );
    subj->Unbind(
      wxEVT_MENU,
      &EsOpenedDocumentList::onCloseAllRequest,
      this,
      EsMdiManagerView::esmdiID_DOC_CLOSE_ALL
    );
    subj->Unbind(
      wxEVT_UPDATE_UI,
      &EsOpenedDocumentList::onUpdateUi,
      this,
      EsMdiManagerView::esmdiID_DOC_CLOSE,
      EsMdiManagerView::esmdiID_DOC_CLOSE_ALL
    );
  }
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onDocumentCreated(const EsVariant& key)
{
#ifdef ES_DEBUG
  const EsVariant& found = m_model->recordsFind(1, key); //< Field index 1 contains full document key by design
  ES_ASSERT(found.isEmpty());
#endif

  const EsPath& path = EsPath::createFromFilePath( key.asString() );
  const EsVariant::Array& rec = {
    path.fileNameExtGet(),
    key.asString(),
    path.fileExtGet()
  };

  m_model->recordAppend(
    rec
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onDocumentOpened(const EsVariant& key)
{
  const EsVariant& found = m_model->recordsFind(1, key); //< Field index 1 contains full document key by design

  if(!found.isEmpty())
    return;

  const EsPath& path = EsPath::createFromFilePath(key.asString());
  const EsVariant::Array& rec = {
    path.fileNameExtGet(),
    key.asString(),
    path.fileExtGet()
  };

  m_model->recordAppend(
    rec
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onDocumentClosed(const EsVariant& key)
{
  const EsVariant& found = m_model->recordsFind(
    1,   //< Field index 1 contains full document key by design
    key
  );
  if(found.isEmpty())
    return;

  ES_ASSERT(1 == found.countGet());
  m_model->recordDelete(
    found[0].asULong() //< Delete record by its index
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onDocumentKeyChanged(const EsVariant& keyOld, const EsVariant& keyNew)
{
  const EsVariant& found = m_model->recordsFind(1, keyOld); //< Field index 1 contains full document key by design
  if(found.isEmpty())
    return;

  const EsPath& path = EsPath::createFromFilePath(keyNew.asString());
  const EsVariant::Array& rec = {
    path.fileNameExtGet(),
    keyNew.asString(),
    path.fileExtGet()
  };

  ES_ASSERT(1 == found.countGet());
  m_model->recordSet(
    found[0].asULong(),
    rec
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onDocumentActivated(const EsVariant& key)
{
  const EsVariant& found = m_model->recordsFind(
    1,   //< Field index 1 contains full document key by design
    key
  );
  if(found.isEmpty())
    return;

  ES_ASSERT(1 == found.countGet());
  
  wxDataViewItem selItem = GetItemByRow(found[0].asULong());
  ES_ASSERT(selItem.IsOk());

  wxDataViewItemArray sel;
  sel.push_back(selItem);

  try
  {
    Unbind(
      wxEVT_DATAVIEW_ITEM_ACTIVATED,
      &EsOpenedDocumentList::onDocumentSelected,
      this
    );

    SetSelections(
      sel
    );
  }
  catch(...)
  {}

  Bind(
    wxEVT_DATAVIEW_ITEM_ACTIVATED,
    &EsOpenedDocumentList::onDocumentSelected,
    this
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onI18nLocaleChanged(const EsString& ES_UNUSED(lang))
{
  if(m_pmnu)
  {
    EsUtilities::menuItemLabelAndHintUpdate(
      m_pmnu.get(),
      EsMdiManagerView::esmdiID_DOC_CLOSE,
      _("Close").as<wxString>(),
      _("Close document, selected in list").as<wxString>()
    );
    EsUtilities::menuItemLabelAndHintUpdate(
      m_pmnu.get(),
      EsMdiManagerView::esmdiID_DOC_CLOSE_ALL,
      _("Close all").as<wxString>(),
      _("Close all open documents").as<wxString>()
    );
  }

  EsUtilities::toolbarItemsLabelsAndHintsUpdateFromMenu(
    m_ptb,
    m_pmnu.get()
  );

  if(!m_model)
    return;

  wxString name;
  unsigned cnt = GetColumnCount();
  for(unsigned idx = 0; idx < cnt; ++idx)
  {
    wxDataViewColumn* col = GetColumn(idx);
    if(!col)
      continue;

    name = m_model->fieldNameGet(idx);
    col->SetTitle(
      EsStringI18n::translate(
        name.t_str()
      ).as<wxString>()
    );
  }
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onDocumentSelected(wxDataViewEvent& evt)
{
  wxDataViewItem item = evt.GetItem();
  if(!item.IsOk() || !m_model)
    return;

  unsigned recIdx = m_model->GetRow(item);
  const EsVariant& rec = m_model->recordGet(recIdx);

  // Post document activation request
  const EsVariant::Array& payload = {
    rec[1],                                     //< Document key to activate  
    EsWxWindowPtr::create(this)                 //< Request initiator
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_MDIGUI,
    as_<ulong>(EsMdiGuiEvt::DocumentActivateRequest),
    payload
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onCloseRequest(wxCommandEvent& ES_UNUSED(evt))
{
  EsString::Array keys;

  wxDataViewItemArray sel;
  GetSelections(sel);

  for(auto const& selItem : sel)
  {
    unsigned recIdx = m_model->GetRow(selItem);
    const EsString& key = m_model->fieldDataGet(
      recIdx,
      1 //< Document key field index
    ).asString();

    keys.push_back(key);
  }

  if( keys.empty() )
    return;

  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentsCloseRequest),
    keys
  );
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onCloseAllRequest(wxCommandEvent& ES_UNUSED(evt))
{
  ES_ASSERT(m_docmgr);
  m_docmgr->documentsCloseAllRequest();
}
//--------------------------------------------------------------------------------

void EsOpenedDocumentList::onUpdateUi(wxUpdateUIEvent& evt)
{
  switch( evt.GetId() )
  {
  case EsMdiManagerView::esmdiID_DOC_CLOSE:
    evt.Enable(
      m_model && 
      !m_model->isEmpty() &&
      GetSelection().IsOk()
    );
    break;
  case EsMdiManagerView::esmdiID_DOC_CLOSE_ALL:
    evt.Enable(
      m_model &&
      !m_model->isEmpty()
    );
    break;
  }
}
//--------------------------------------------------------------------------------
