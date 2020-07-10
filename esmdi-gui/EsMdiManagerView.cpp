#include "esmdi-guipch.h"
#pragma hdrstop

#include "EsMdiManagerView.h"
//--------------------------------------------------------------------------------

EsMdiManagerView::Handler::Handler(EsMdiManagerView& This, ulong precedence) :
  EsEventSubscriber(
    ES_EVTC_MDI ES_EVTC_AND
    ES_EVTC_MDIGUI ES_EVTC_AND
    ES_EVTC_I18N,
    precedence
  ),
  m_this(This)
{}
//---------------------------------------------------------------------------

void EsMdiManagerView::Handler::notify(const EsEventIntf::Ptr& evt)
{
  const EsString& cat = evt->categoryGet();
  ulong id = evt->idGet();
  const EsVariant& payload = evt->payloadGet();

  if( ES_EVTC_MDI == cat )
  {
    switch(as_<EsMdiEvt>(id))
    {
    case EsMdiEvt::DocumentChanged:
      m_this.onDocumentChanged(
        payload[0],
        payload[1].asString(),
        payload[2].asPointer()
      );
      break;
    case EsMdiEvt::DocumentStateChanged:
      m_this.onDocumentStateChanged(
        payload
      );
      break;
    case EsMdiEvt::DocumentCreated:
      m_this.onDocumentCreated(
        payload[0],
        payload[1]
      );
      break;
    case EsMdiEvt::DocumentOpened:
      m_this.onDocumentOpened(
        payload[0],
        payload[1]
      );
      break;
    case EsMdiEvt::DocumentClosed:
      m_this.onDocumentClosed(
        payload[0],
        payload[1].asString()
      );
      break;
    case EsMdiEvt::DocumentKeyChanged:
      m_this.onDocumentKeyChanged(
        payload[0],
        payload[1]
      );
      break;
    case EsMdiEvt::AllDocumentsClosed:
      m_this.onAllDocumentsClosed();
      break;
    case EsMdiEvt::DocumentsCloseRequest:
      m_this.onDocumentsCloseRequest(
        payload
      );
      break;
    }
  }
  else if( ES_EVTC_MDIGUI == cat )
  {
    switch(as_<EsMdiGuiEvt>(id))
    {
    case EsMdiGuiEvt::DocumentActivateRequest:
      {
        EsWxWindowPtrIntf::Ptr wndRefPtr = payload[1].asObject();
        if( !wndRefPtr || wndRefPtr->windowRefGet() != &m_this )
          m_this.onDocumentActivating(
            payload[0]
          );
      }
      break;
    //case EsMdiGuiEvt::DocumentActivated:
    //  break;
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

EsMdiManagerView::EsMdiManagerView(wxWindow* parent, wxWindowID id, const EsDocumentManagerIntf::Ptr& docmgr, ulong eventPrecedence /*= 100*/) :
wxAuiNotebook(
  parent,
  id,
  wxDefaultPosition,
  wxDefaultSize,
  wxAUI_NB_TOP |
  wxAUI_NB_TAB_SPLIT |
  wxAUI_NB_TAB_MOVE |
  wxAUI_NB_SCROLL_BUTTONS |
  wxAUI_NB_WINDOWLIST_BUTTON |
  wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
  wxBORDER_NONE
),
m_evtHandler(
  *this,
  eventPrecedence
),
m_docmgr(docmgr),
m_rclickedIdx(wxNOT_FOUND)
{
  ES_ASSERT(m_docmgr);

  Bind(
    wxEVT_AUINOTEBOOK_PAGE_CLOSE, 
    &EsMdiManagerView::onDocumentTabClose, 
    this
  );
  Bind(
    wxEVT_AUINOTEBOOK_PAGE_CHANGED, 
    &EsMdiManagerView::onDocumentTabChanged, 
    this
  );
  Bind(
    wxEVT_AUINOTEBOOK_TAB_RIGHT_UP,
    &EsMdiManagerView::onDocumentTabRclicked,
    this
  );
}
//--------------------------------------------------------------------------------

EsMdiManagerView::~EsMdiManagerView()
{
  ES_DEBUG_TRACE(esT("EsMdiManagerView::~EsMdiManagerView()"));
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::documentSaveInternal(const EsDocumentIntf::Ptr& doc, const EsVariant& target, bool asCopy /*= false*/)
{
  EsVariant oldKey = doc->keyGet();

  if( asCopy )
  {
    
  }
  else
    doc->write(
      target,
      documentAdditionalParamsGet(
        doc,
        target
      )
    );
    ES_ASSERT(target == doc->keyGet());

  // Document key may change here, but out view mapping is still using an old one
  // Make consistent changes at once, because we know what we're doing
  //
  if(oldKey != target)
    onDocumentKeyChanged(
      oldKey,
      target
    );
}
//--------------------------------------------------------------------------------

int EsMdiManagerView::newFilePathSelect(const EsDocumentIntf::Ptr& doc, EsString& newPath)
{
  ES_ASSERT(doc);
  newPath = EsString::null();

  const EsVariant& regInfo = m_docmgr->documentRegistrationPayloadGet(
    doc->complexTypeGet()
  );

  const EsString& title = EsString::format(
    _("Save '%s'"),
    doc->keyGet()
  );
  
  wxFileDialog dlgSave(
    this,
    title.c_str(),
    wxEmptyString,
    wxEmptyString,
    regInfo[esVT("filterStr")][1].asString().c_str(),
    wxFD_SAVE | 
      wxFD_OVERWRITE_PROMPT | 
      wxFD_CHANGE_DIR | 
      wxFD_NO_FOLLOW,
    wxDefaultPosition,
    wxDefaultSize
  );

  wxFileName fname( doc->keyGet().asString().c_str() );

  dlgSave.SetDirectory(fname.GetPath());
  dlgSave.SetFilename(fname.GetFullName());

  if(wxID_OK != dlgSave.ShowModal())
    return wxNO;

  newPath = wxFileName(
    dlgSave.GetDirectory(),
    dlgSave.GetFilename()
  ).GetFullPath().t_str();

  return wxOK;
}
//--------------------------------------------------------------------------------

bool EsMdiManagerView::documentSave(const EsDocumentIntf::Ptr& doc, bool promptToSaveModified /*= true*/)
{
  ES_ASSERT(doc);

  if(!doc->isNew() && !doc->isModified())
    return true;

  ES_DEBUG_TRACE(
    esT("EsMdiManagerView::documentSave(doc=%s, promptToSaveModified=%d)"),
    EsVariant(doc).trace(),
    (int)promptToSaveModified
  );

  EsString newPath;
  ulong flags = promptToSaveModified ?
    (
      doc->isModified() ?
      PromptSaveModified :
      SaveAs
    ) :
    SaveFlagNone;

  int result = documentPrepareForSave(
    doc,
    flags,
    newPath
  );

  if(wxOK == result)
  {
    const EsString& target = (doc->isNew() || (flags & SaveAs)) ? 
      newPath : 
      doc->keyGet().asString();

    documentSaveInternal(
      doc,
      target
    );

    return true;
  }

  return wxNO == result;
}
//--------------------------------------------------------------------------------

bool EsMdiManagerView::documentSaveAs(const EsDocumentIntf::Ptr& doc, bool asCopy /*= false*/)
{
  ES_ASSERT(doc);

  EsString newPath;
  int result = documentPrepareForSave(
    doc,
    asCopy ? 
      SaveAsCopy :  
      SaveAs,
    newPath
  );

  if(wxOK == result)
  {
    ES_ASSERT(!newPath.empty());

    documentSaveInternal(
      doc,
      newPath
    );

    return true;
  }

  return wxNO == result;
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::documentsCloseRequest(const EsVariant& keys /*= EsVariant::null()*/) const
{
  const EsVariant::Array& docs = keys.isEmpty() ?
    m_docmgr->documentsGet().asVariantCollection() :
    keys.asVariantCollection();

  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentsCloseRequest),
    docs
  );
}
//--------------------------------------------------------------------------------

bool EsMdiManagerView::documentsClose(EsVariant keys /*= EsVariant::null()*/)
{
  bool result = true;

  if( keys.isEmpty() )
    keys = m_docmgr->allDocumentKeysGet();

  EsVariant keyAfterSave;
  if( keys.isCollection() )
  {
    const EsVariant::Array& akeys = keys.asVariantCollection();
    for( auto const& key : akeys )
    {
      bool continueAnyway = true;
      if( 
        !doHandleDocumentCloseRequest(
          key,
          continueAnyway,
          keyAfterSave
        ) 
      )
      {
        if( !continueAnyway )
        {
          result = false;
          break;
        }
      }
      else
        m_docmgr->documentClose(keyAfterSave);
    }
  }
  else //< Interpret keys as single key
  {
    ES_ASSERT( !keys.isEmpty() );

    bool continueAnyway = true;
    if( 
      doHandleDocumentCloseRequest(
        keys, 
        continueAnyway,
        keyAfterSave
      ) 
    )
      m_docmgr->documentClose(keyAfterSave);
    else
      result = false;    
  }

  return result;
}
//--------------------------------------------------------------------------------

int EsMdiManagerView::documentPrepareForSave(const EsDocumentIntf::Ptr& doc, ulong flags, EsString& newPath)
{
  ES_ASSERT(doc);

  newPath = EsString::null();
  if(doc->isModified() || (flags & (SaveAs|SaveAsCopy)))
  {
    if(doc->isModified() && (flags & PromptSaveModified))
    {
      const EsString& message = EsString::format(
        _("A document\n'%s'\nhas unsaved changes.\nDo you want to save it?"),
        doc->keyGet()
      );

      int response = wxMessageBox(
        message.c_str(),
        _("Save document").c_str(),
        wxICON_EXCLAMATION | 
          wxYES | 
          wxNO | 
          wxCANCEL | 
          wxYES_DEFAULT | 
          wxCENTER,
        this
      );

      switch(response)
      {
      case wxNO:
      case wxCANCEL:
        return response;
      case wxYES:
        break;
      default:
        return wxNO;
      }
    }

    if(doc->isNew() || (flags & (SaveAs|SaveAsCopy)))
    {
_re_query_new_path:

      int response = newFilePathSelect(
        doc,
        newPath
      );

      if(wxOK != response)
        return wxNO;

      bool uniqueCheckOk = (flags & SaveAsCopy) ? true : m_docmgr->documentFind(newPath).isEmpty();

      if( !uniqueCheckOk )
      {
        const EsString& message = EsString::format(
          _("A document\n'%s'\ncould not be saved to '%s',\nbecause it is already open in another window."),
          doc->keyGet(),
          newPath
        );

        wxMessageBox(
          message.c_str(),
          _("Document with same name already open").c_str(),
          wxICON_WARNING | 
            wxOK | 
            wxOK_DEFAULT | 
            wxCENTER,
          this
        );

        goto _re_query_new_path;
      }
    }
  }

  return wxOK;
}
//--------------------------------------------------------------------------------

EsMdiManagerView::DocViewPtr EsMdiManagerView::documentViewByKeyGet(const EsVariant& key) const ES_NOTHROW
{
  if( key.isEmpty() )
    return nullptr;

  auto cit = m_views.find(key);
  if(cit != m_views.end())
    return (*cit).second;

  return nullptr;
}
//--------------------------------------------------------------------------------

EsVariant EsMdiManagerView::keyByDocumentViewGet(const EsMdiManagerView::DocViewPtr& view) const ES_NOTHROW
{
  if( !view )
    return EsVariant::null();

  for(auto const& item : m_views)
  {
    if(view == item.second)
      return item.first;
  }

  return EsVariant::null();
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::documentsOpen(const EsString::Array& docTypesFilter /*= EsString::nullArray()*/)
{
  const EsString::Array& docTypes = m_docmgr->registeredDocumentTypesGet();
  wxString filterStr;
  for( auto const& type : docTypes )
  {
    if( 
      !docTypesFilter.empty() && 
      docTypesFilter.end() == std::find(
        docTypesFilter.begin(),
        docTypesFilter.end(),
        EsDocumentManager::isComplexType(type) ? 
          EsDocumentManager::complexSubTypeGet(type) :
          type
      )
    )
      continue;

    const EsVariant& regInfo = m_docmgr->documentRegistrationPayloadGet(type);
    const EsString& filter = regInfo[esVT("filterStr")][1].asString();

    if(!filterStr.empty())
      filterStr += esT("|");

    filterStr += filter.c_str();
  }
  
  if( filterStr.empty() )
    return;

  wxFileDialog dlgOpen(
    this,
    _("Open document").as<wxString>(),
    wxEmptyString,
    wxEmptyString,
    filterStr,
    wxFD_OPEN | 
    wxFD_FILE_MUST_EXIST |
    wxFD_MULTIPLE |
    wxFD_CHANGE_DIR | 
    wxFD_NO_FOLLOW,
    wxDefaultPosition,
    wxDefaultSize
  );

  if(wxID_OK != dlgOpen.ShowModal())
    return;

  const wxString& dir = dlgOpen.GetDirectory();
  wxArrayString files;
  dlgOpen.GetFilenames(files);

  bool preferActivate = 0 == m_docmgr->documentsCountGet();

  for(auto const& file : files)
  {
    wxFileName fname(
      dir,
      file
    );
    fname.Normalize();

    const EsVariant& key = esVC( fname.GetFullPath().t_str() );

    documentOpenOrActivate(
      key,
      EsVariant::null(),
      preferActivate
    );
  }
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::documentTabUpdate(const EsMdiManagerView::DocViewPtr& view, const EsDocumentIntf::Ptr& doc)
{
  ES_ASSERT(view);
  ES_ASSERT(doc);

  int idx = GetPageIndex(view);
  ES_ASSERT(wxNOT_FOUND < idx);

  wxString tabTxt = documentTabTextGet(doc);
  if(doc->isModified())
    tabTxt += esT("*");

  SetPageText(
    idx,
    tabTxt
  );

  const wxString& tabTooltip = documentTabTooltipGet(doc);
  SetPageToolTip(
    idx,
    tabTooltip
  );

  onDocumentTabUpdated(view, doc);

  if( idx == GetSelection() ) //< Do extra stuff if we're updating currently selected tab 
    onSelectedTabUpdated();
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::currentDocViewChangeCheck()
{
  ES_DEBUG_TRACE(esT("EsMdiManagerView::currentDocViewChangeCheck"));

  wxWindow* view = GetCurrentPage();
  if(m_viewActive != view)
  {
    onActiveViewChanging();

    m_viewActive = view;
    m_keyActive = keyByDocumentViewGet(view);

    onActiveViewChanged();

    if( m_viewActive )
    {
      // Post document activation notification
      const EsVariant::Array& payload = {
        m_keyActive,                                //< key of activated Document
        EsWxWindowPtr::create(this)                 //< Notification initiator
      };

      EsEventDispatcher::eventPost(
        ES_EVTC_MDIGUI,
        as_<ulong>(EsMdiGuiEvt::DocumentActivated),
        payload
      );
    }
  }
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::ctxmnuI18nUpdate()
{
  if(!m_pmnuTab)
    return;

  EsUtilities::menuItemLabelAndHintUpdate(
    m_pmnuTab.get(),
    EsMdiManagerView::esmdiID_DOC_CLOSE,
    _("Close").as<wxString>(),
    _("Close document, selected in list").as<wxString>()
  );
  EsUtilities::menuItemLabelAndHintUpdate(
    m_pmnuTab.get(),
    EsMdiManagerView::esmdiID_DOC_CLOSE_ALL,
    _("Close all").as<wxString>(),
    _("Close all open documents").as<wxString>()
  );
}
//--------------------------------------------------------------------------------

wxMenuSharedPtr EsMdiManagerView::ctxmenuTabGet()
{
  if(m_pmnuTab)
    return m_pmnuTab;

  m_pmnuTab = ES_MAKE_SHARED(wxMenu);
  ES_ASSERT(m_pmnuTab);

  m_pmnuTab->Append(
    esmdiID_DOC_CLOSE,
    EsUtilities::wxSpaceString(),
    EsUtilities::wxSpaceString()
  );
  m_pmnuTab->Append(
    esmdiID_DOC_CLOSE_ALL,
    EsUtilities::wxSpaceString(),
    EsUtilities::wxSpaceString()
  );

  const wxSize& appmnuimgsze = EsUtilities::appSizePropertyGet(ES_APP_OPT_MENU_IMG_WH);

  EsUtilities::menuItemBitmapSet(
    m_pmnuTab.get(),
    esmdiID_DOC_CLOSE,
    appmnuimgsze,
    wxART_OTHER,
    esART_CLOSE
  );

  EsUtilities::menuItemBitmapSet(
    m_pmnuTab.get(),
    esmdiID_DOC_CLOSE_ALL,
    appmnuimgsze,
    wxART_OTHER,
    esART_CLOSE_ALL
  );

  Bind(
    wxEVT_MENU,
    &EsMdiManagerView::onCloseRequest,
    this,
    esmdiID_DOC_CLOSE
  );
  Bind(
    wxEVT_MENU,
    &EsMdiManagerView::onCloseAllRequest,
    this,
    esmdiID_DOC_CLOSE_ALL
  );

  // Update strings once upon creation
  ctxmnuI18nUpdate();

  return m_pmnuTab;
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::documentViewAddToNotebook(const EsMdiManagerView::DocViewPtr& view, const EsDocumentIntf::Ptr& doc, bool activate /*= true*/)
{
  ES_ASSERT(view);
  ES_ASSERT(doc);

  wxWindowUpdateLocker lock(this);
  
  wxBitmap img = documentTabIconGet( doc );
  const EsVariant& key = doc->keyGet();

  m_views[key] = view;

  AddPage(
    view,
    wxEmptyString,
    activate,
    img
  );

  documentTabUpdate(
    view, 
    doc
  );

  currentDocViewChangeCheck();
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::documentViewParamsHandle(const EsVariant& key, const EsAssocContainerIntf::Ptr& params)
{
  ES_DEBUG_TRACE(
    esT("EsMdiManagerView::documentViewParamsHandle key='%s', params:\n%s"),
    key.trace(),
    EsVariant::dump(params)
  );

  if( !params || params->isEmpty() )
    return;

  doHandleDocumentViewParameters(
    documentViewByKeyGet(key),
    params
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::documentViewClose(const EsMdiManagerView::DocViewPtr& view)
{
  ES_DEBUG_TRACE(esT("EsMdiManagerView::documentViewClose"));
  ES_ASSERT(view);

  const EsVariant& key = keyByDocumentViewGet(view);
  ES_ASSERT(!key.isEmpty());

  int idx = GetPageIndex(view);
  ES_ASSERT(wxNOT_FOUND < idx);

  // Detach old handlers && custom menu items for an active view
  if(m_viewActive == view)
    onActiveViewClosing();

  DeletePage(idx);

  m_views.erase(key);

  if( view )
    view->Destroy();

  currentDocViewChangeCheck();

  if(0 == GetPageCount())
    onLastViewClosed();
}
//--------------------------------------------------------------------------------

EsDocumentIntf::Ptr EsMdiManagerView::documentFind(const EsVariant& key) const
{
  ES_ASSERT( m_docmgr );

  EsVariant vdoc = m_docmgr->documentFind(key);
  if(vdoc.isEmpty())
    return nullptr;

  return vdoc.asObject();
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::documentOpenOrActivate(const EsVariant& key, const EsVariant& params, bool preferActivate /*= false*/)
{
  DocViewPtr view = documentViewByKeyGet(key);

  if(preferActivate && view)
  {
    if(m_viewActive != view)
      SetSelection(
        GetPageIndex(view)
      );

    return;
  }

  const EsPath& path = EsPath::createFromFilePath( key.asString() );

  // Get document type by selected file extension
  const EsString& type = m_docmgr->documentTypeFindByFileExt(
    path.fileExtGet()
  );

  if(type.empty())
    EsException::Throw(
      _("Could not find registered document type for file extension '%s'. File type unknown or is not supported."),
      path.fileExtGet()
    );

  EsVariant aparams( EsVariant::VAR_VARIANT_COLLECTION );

  // Key must be the first in params collection, by design
  aparams.addToVariantCollection(
    path.pathGet()
  );

  // Stuff other params after the key 
  if( !params.isEmpty() && aparams[0] != params )
    aparams += params;

  m_docmgr->documentOpen(
    type,
    aparams
  );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// WX events
// 
void EsMdiManagerView::onDocumentTabClose(wxAuiNotebookEvent& evt)
{
  ES_DEBUG_TRACE(esT("EsMdiManagerView::onDocumentTabClose"));

  evt.Veto(); // Perform actual close from ES event handler

  wxWindow* view = GetPage(
    evt.GetSelection()
  );
  ES_ASSERT(view);

  const EsVariant::Array& keys = {
    keyByDocumentViewGet(view)
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentsCloseRequest),
    keys
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentTabChanged(wxAuiNotebookEvent& evt)
{
  ES_DEBUG_TRACE(esT("EsMdiManagerView::onDocumentTabChanged"));

  int pageIdx = evt.GetSelection();
  ES_ASSERT(-1 < pageIdx);

  wxWindow* view = GetPage(pageIdx);
  ES_ASSERT(view);

  const EsVariant& key = keyByDocumentViewGet(view);
  ES_ASSERT(!key.isEmpty());

  currentDocViewChangeCheck();
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentTabRclicked(wxAuiNotebookEvent& evt)
{
  m_rclickedIdx = evt.GetSelection();
  ES_ASSERT(wxNOT_FOUND != m_rclickedIdx);
  
  wxPoint pos = wxGetMousePosition();
  pos = ScreenToClient(pos);

  PopupMenu(
    ctxmenuTabGet().get(),
    pos
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onCloseRequest(wxCommandEvent& ES_UNUSED(evt))
{
  ES_ASSERT(wxNOT_FOUND != m_rclickedIdx);

  wxWindow* view = GetPage(
    m_rclickedIdx
  );
  ES_ASSERT(view);

  const EsVariant::Array& keys = {
    keyByDocumentViewGet(view)
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_MDI,
    as_<ulong>(EsMdiEvt::DocumentsCloseRequest),
    keys
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onCloseAllRequest(wxCommandEvent& ES_UNUSED(evt))
{
  ES_ASSERT(m_docmgr);
  m_docmgr->documentsCloseAllRequest();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// ES events
//

void EsMdiManagerView::onI18nLocaleChanged(const EsString& ES_UNUSED(lang))
{
  ctxmnuI18nUpdate();

  ES_ASSERT(m_docmgr);
  const EsVariant& keys = m_docmgr->allDocumentKeysGet();

  for(ulong idx = 0; idx < keys.countGet(); ++idx)
  {
    // Force refresh on document views tabs
    EsDocumentIntf::Ptr doc = documentFind(keys[idx]);
    if(!doc)
      continue;

    DocViewPtr view = documentViewByKeyGet(keys[idx]);
    if(!view)
      continue;

    documentTabUpdate(
      view,
      doc
    );
  }
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentCreated(const EsVariant& key, const EsVariant& params)
{
  ES_DEBUG_TRACE(
    esT("EsMdiManagerView::onDocumentCreated(key=%s, params=%s)"),
    key.trace(),
    params.trace()
  );

  ES_ASSERT(!key.isEmpty());

  EsDocumentIntf::Ptr doc = m_docmgr->documentFind(key).asExistingObject();
  ES_ASSERT(doc);

  wxWindow* view = documentViewCreate( doc );
  ES_ASSERT(view);

  ES_ASSERT( this == view->GetParent() );

  documentViewAddToNotebook(
    view,
    doc
  );

  onDocumentViewCreated(
    view,
    doc
  );

  documentViewParamsHandle(
    key,
    params.isObject() ? 
      params.asObject() : 
      nullptr
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentOpened(const EsVariant& key, const EsVariant& params)
{
  ES_DEBUG_TRACE(
    esT("EsMdiManagerView::onDocumentOpened(key=%s, params=%s)"),
    key.trace(), 
    params.trace()
  );

  ES_ASSERT(!key.isEmpty());

  EsDocumentIntf::Ptr doc = m_docmgr->documentFind(key).asExistingObject();
  ES_ASSERT(doc);

  auto cit = m_views.find(key);
  if(cit != m_views.end())
    documentTabUpdate(
      (*cit).second,
      doc
    );
  else
    documentViewCreate(
      doc
    );

  documentViewParamsHandle(
    key,
    params.isObject() ?
      params.asObject() :
      nullptr
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentClosed(const EsVariant& key, const EsString& type)
{
  ES_DEBUG_TRACE(
    esT("EsMdiManagerView::onDocumentClosed(key=%s, type=%s)"),
    key.trace(), 
    type
  );

  DocViewPtr view = documentViewByKeyGet(key);
  if(view)
    documentViewClose(view);
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentChanged(const EsVariant& key, const EsString& subj, void* sender)
{
  ES_DEBUG_TRACE(
    esT("EsMdiManagerView::onDocumentChanged(key=%s, subj=%s, sender=%p)"),
    key.trace(),
    subj,
    EsVariant(
      sender,
      EsVariant::ACCEPT_POINTER
    )
  );

  EsDocumentIntf::Ptr doc = documentFind(key);
  if(!doc)
    return;

  DocViewPtr view = documentViewByKeyGet(key);
  if( !view )
    return;

  documentTabUpdate(
    view,
    doc
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentStateChanged(const EsVariant& key)
{
  ES_DEBUG_TRACE(
    esT("EsMdiManagerView::onDocumentStateChanged(key=%s)"),
    key.trace()
  );

  EsDocumentIntf::Ptr doc = documentFind(key);
  if(!doc)
    return;

  DocViewPtr view = documentViewByKeyGet(key);

  documentTabUpdate(
    view,
    doc
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentKeyChanged(const EsVariant& keyOld, const EsVariant& keyNew)
{
  ES_DEBUG_TRACE(
    esT("EsMdiManagerView::onDocumentKeyChanged(old=%s, new=%s)"),
    keyOld.trace(), 
    keyNew.trace()
  );

  DocViewPtr view = documentViewByKeyGet(keyOld);
  if(!view)
    return;

  m_views.erase(keyOld);
  m_views[keyNew] = view;

  EsDocumentIntf::Ptr doc = documentFind(keyNew);
  ES_ASSERT(doc);

  documentTabUpdate(
    view,
    doc
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentActivating(const EsVariant& key)
{
  ES_DEBUG_TRACE(
    esT("EsMdiManagerView::onDocumentActivating(key=%s)"),
    key.trace()
  );

  wxWindow* view = documentViewByKeyGet(key);
  if( !view )
    return;

  int idx = GetPageIndex(view);
  if( wxNOT_FOUND == idx )
    return;

  Unbind(
    wxEVT_AUINOTEBOOK_PAGE_CHANGED, 
    &EsMdiManagerView::onDocumentTabChanged, 
    this
  );

  try
  {
    SetSelection(idx);
    currentDocViewChangeCheck();
  }
  catch(...)
  {}

  Bind(
    wxEVT_AUINOTEBOOK_PAGE_CHANGED, 
    &EsMdiManagerView::onDocumentTabChanged, 
    this
  );
}
//--------------------------------------------------------------------------------

void EsMdiManagerView::onDocumentsCloseRequest(const EsVariant& keys)
{
  documentsClose(keys);
}
//--------------------------------------------------------------------------------
