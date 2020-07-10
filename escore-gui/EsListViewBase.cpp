#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsListViewBase.h"
//--------------------------------------------------------------------------------

EsListViewBase::EsListViewBase(wxWindow* parent, const EsListViewModelBase::Ptr& model, wxWindowID id /*= wxID_ANY*/, long style /*= EsListViewBase::styleDefault*/) :
wxDataViewCtrl(
  parent, 
  id, 
  wxDefaultPosition,
  wxDefaultSize,
  style
),
m_ptb(nullptr),
m_needRefresh(false)
{
  modelSet(model);

  Bind(
    wxEVT_CONTEXT_MENU, 
    &EsListViewBase::onCtxMenu,
    this
  );
  Bind(
    wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,
    &EsListViewBase::onCtxMenuItem,
    this
  );
}
//--------------------------------------------------------------------------------

EsListViewBase::EsListViewBase(wxWindow* parent, wxWindowID id, long style /*= EsListViewBase::styleDefault*/) :
wxDataViewCtrl(
  parent,
  id,
  wxDefaultPosition,
  wxDefaultSize,
  style
),
m_ptb(nullptr),
m_needRefresh(false)
{
  Bind(
    wxEVT_CONTEXT_MENU,
    &EsListViewBase::onCtxMenu,
    this
  );
  Bind(
    wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,
    &EsListViewBase::onCtxMenuItem,
    this
  );
}
//--------------------------------------------------------------------------------

EsListViewBase::~EsListViewBase()
{
  modelSet(
    EsListViewModelBase::Ptr()
  );
}
//--------------------------------------------------------------------------------

void EsListViewBase::modelSet(const EsListViewModelBase::Ptr& model)
{
  wxWindowUpdateLocker lock(this);

  ClearColumns();

  m_model = model;

  AssociateModel(
    model.get()
  );

  if( m_model )
    fromModelInit();
}
//--------------------------------------------------------------------------------

void EsListViewBase::fromModelInit()
{
  ES_ASSERT(m_model);

  // TODO: Set-up indicator

  // Set-up columns
  for(ulong idx = 0; idx < m_model->fieldCountGet(); ++idx)
    modelFieldColumnAppend(idx);

  // Force list refresh
  refresh(true);
}
//--------------------------------------------------------------------------------

void EsListViewBase::doRefresh()
{
  ES_ASSERT(m_model);

  wxDataViewItem sel = GetSelection();
  m_model->refresh();

  if( sel.IsOk() )
  {
    EnsureVisible( 
      sel
    );
  }
}
//--------------------------------------------------------------------------------

void EsListViewBase::modelFieldColumnAppend(ulong fldIdx)
{
  const wxString& type = m_model->fieldTypeGet(fldIdx);
  const wxString& name = m_model->fieldNameGet(fldIdx);
  const wxString& hint = m_model->fieldHintGet(fldIdx);
  int minWidth = m_model->fieldMinWidthGet(fldIdx);
    
  wxDataViewColumn* col = doCreateColumnFromField(
    fldIdx,
    type,
    name,
    hint,
    minWidth
  );
  ES_ASSERT(col);

  AppendColumn( col );
}
//--------------------------------------------------------------------------------

wxDataViewColumn* EsListViewBase::doCreateColumnFromField(ulong fldIdx, const wxString& type, const wxString& name, const wxString& hint, int minWidth) const
{
  // Create column with text renderer by default
  // TODO: allow selecting renderer for column based on some criterion
  // 
  std::unique_ptr<EsMultilineTextRenderer> rendptr = ES_MAKE_UNIQUE(
    EsMultilineTextRenderer,
    type
  );
  ES_ASSERT( rendptr );

  std::unique_ptr<wxDataViewColumn> colptr = ES_MAKE_UNIQUE(
    wxDataViewColumn,
    EsStringI18n::translate(
      name.t_str()
    ).c_str(),
    rendptr.release(),
    fldIdx,
    minWidth,
    wxALIGN_LEFT,
    wxDATAVIEW_COL_RESIZABLE
  );
  ES_ASSERT(colptr);

  colptr->SetMinWidth(minWidth);

  return colptr.release();
}
//--------------------------------------------------------------------------------

void EsListViewBase::clear()
{
  if(!m_model)
    return;

  m_model->clear();
  m_needRefresh = true;
  refresh();
}
//--------------------------------------------------------------------------------

void EsListViewBase::refresh(bool force /*= false*/)
{
  if(!m_model)
    return;

  if(m_needRefresh || force)
  {
    doRefresh();

    m_needRefresh = false;
  }
}
//--------------------------------------------------------------------------------

void EsListViewBase::onI18nLocaleChanged(const EsString& loc)
{
  if( !m_model )
    return;

  wxWindowUpdateLocker lock(this);
  for(ulong idx = 0; idx < m_model->fieldCountGet(); ++idx)
  {
    wxDataViewColumn* col = GetColumn(idx);
    if( !col )
      continue;

    col->SetTitle(
      EsStringI18n::translate(
        m_model->fieldNameGet(idx).t_str()
      ).as<wxString>()
    );
  }
}
//--------------------------------------------------------------------------------

void EsListViewBase::onCtxMenu(wxContextMenuEvent& ES_UNUSED(evt))
{
  ctxMenuGet();
  ES_ASSERT(m_pmnu);

  PopupMenu(
    m_pmnu.get()
  );
}
//--------------------------------------------------------------------------------

void EsListViewBase::onCtxMenuItem(wxDataViewEvent& ES_UNUSED(evt))
{
  // TODO:
  ctxMenuGet();
  ES_ASSERT(m_pmnu);

  PopupMenu(
    m_pmnu.get()
  );
}
//--------------------------------------------------------------------------------

wxMenuSharedPtr EsListViewBase::ctxMenuGet()
{
  if(m_pmnu)
    return m_pmnu;

  m_pmnu = ES_MAKE_SHARED( wxMenu );
  ES_ASSERT(m_pmnu);

  ctxMenuInit();

  evtHandlersSet(
    this,
    true
  );

  return m_pmnu;
}
//--------------------------------------------------------------------------------

void EsListViewBase::ctxMenuSet(const wxMenuSharedPtr& pmnu)
{
  ES_DEBUG_TRACE(
    esT("EsListViewBase::ctxMenuSet")
  );

  m_pmnu = pmnu;
}
//--------------------------------------------------------------------------------

wxToolBar* EsListViewBase::toolBarGet(wxWindow* parent)
{
  if(m_ptb)
    return m_ptb;

  m_ptb = new wxToolBar(
    parent,
    wxID_ANY,
    wxDefaultPosition,
    wxDefaultSize,
    wxTB_FLAT|wxTB_TOP|wxTB_HORZ_TEXT
  );
  ES_ASSERT(m_ptb);

  m_ptb->SetToolBitmapSize(
    EsUtilities::appSizePropertyGet(ES_APP_OPT_TOOLBAR_IMG_WH)
  );

  toolBarInit();
  
  m_ptb->AddStretchableSpace();
  m_ptb->Realize();

  evtHandlersSet(
    m_ptb, 
    true
  );
    
  return m_ptb;
}
//--------------------------------------------------------------------------------
