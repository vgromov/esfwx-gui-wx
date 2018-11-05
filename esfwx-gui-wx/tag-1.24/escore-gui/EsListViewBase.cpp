#include "escore_gui_pch.h"
#pragma hdrstop

#include <wx/listbase.h>
#include <wx/listctrl.h>

#include "EsListViewBase.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsListViewStyle& EsListViewStyle::attrSet(int key, const wxColour &colText, const wxColour &colBack, const wxFont &font /*= wxNullFont*/)
{
  m_attrs[key] = wxListItemAttr(
    colText,
    colBack,
    font
  );

  return *this;
}
//--------------------------------------------------------------------------------

wxListItemAttr* EsListViewStyle::attrGet(int key) const
{
  auto cit = m_attrs.find(key);
  if(cit != m_attrs.end())
    return const_cast<wxListItemAttr*>(&(*cit).second); //< wxListCtrl expects non const

  return nullptr;
}
//--------------------------------------------------------------------------------

void EsListViewStyle::attrDelete(int key)
{
  auto it = m_attrs.find(key);
  if(it != m_attrs.end())
    m_attrs.erase(it);
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsListViewModel::Ptr EsListViewModel::create()
{
  EsListViewModel::Ptr obj(new EsListViewModel);
  ES_ASSERT(obj);

  return obj;
}
//--------------------------------------------------------------------------------

EsListViewModel& EsListViewModel::fieldAdd(const wxString& name, ulong minWidthHint /*= 0*/, const wxString& hint /*= wxEmptyString*/)
{
  m_fldNames.push_back(name);
  m_fldMinWidthHints.push_back(minWidthHint);
  m_fldHints.push_back(hint);

  return *this;
}
//--------------------------------------------------------------------------------

wxString EsListViewModel::fieldTextGet(ulong recIdx, ulong idx) const
{
  const EsVariant& data = fieldDataGet(recIdx, idx);

  if(data.isString())
    return data.doInterpretAsString().c_str();
  else
    return data.asString().c_str();
}
//--------------------------------------------------------------------------------

const EsVariant& EsListViewModel::fieldDataGet(ulong recIdx, ulong idx) const
{
  return recordGet(recIdx)[idx];
}
//--------------------------------------------------------------------------------

EsVariant EsListViewModel::recordsFind(const EsVariant& fldIndexes, const EsVariant& fldValues) const
{
  if(m_data.empty())
    return EsVariant::null();

  if(fldIndexes.isEmpty() || fldValues.isEmpty())
    return EsVariant::null();

  const EsVariant::Array& idxs = fldIndexes.asVariantCollection();
  const EsVariant::Array& vals = fldValues.asVariantCollection();

  if(idxs.size() != vals.size())
    EsException::Throw(
      _("EsListViewModel::recordsFind: Both arguments must be of the same size")
    );

  EsVariant::Array result;
  for(size_t recidx = 0; recidx < m_data.size(); ++recidx)
  {
    const EsVariant::Array& rec = m_data[recidx];
    bool recFound = true;

    for(size_t filterIdx = 0; filterIdx < idxs.size(); ++filterIdx)
    {
      ulong fldIdx = idxs[filterIdx].asULong();

      const EsVariant& fldVal = rec[fldIdx];
      const EsVariant& fltrVal = vals[filterIdx];

      if(fldVal != fltrVal)
      {
        recFound = false;
        break;
      }
    }

    if(recFound)
      result.push_back(recidx);
  }

  if(result.empty())
    return EsVariant::null();

  return result;
}
//--------------------------------------------------------------------------------

const EsVariant::Array& EsListViewModel::recordGet(ulong recIdx) const
{
  return m_data[recIdx];
}
//--------------------------------------------------------------------------------

void EsListViewModel::recordSet(ulong recIdx, const EsVariant::Array& record)
{
  ES_ASSERT(fieldCountGet() == record.size());
  m_data[recIdx] = record;
}
//--------------------------------------------------------------------------------

ulong EsListViewModel::recordAppend(const EsVariant::Array& record)
{
  ES_ASSERT(fieldCountGet() == record.size());
  m_data.push_back( record );

  return m_data.size()-1;
}
//--------------------------------------------------------------------------------

void EsListViewModel::recordDelete(ulong recIdx)
{
  m_data.erase(
    m_data.begin()+recIdx
  );
}
//--------------------------------------------------------------------------------

void EsListViewModel::clear()
{
  m_data.clear();
}
//--------------------------------------------------------------------------------

wxString EsListViewModel::asString() const
{
  if(isEmpty())
    return wxEmptyString;

  return asString(
    0,
    fieldCountGet()-1,
    0,
    recordCountGet()-1
  );
}
//--------------------------------------------------------------------------------

wxString EsListViewModel::asString(ulong recIdx) const
{
  return asString(
    0,
    fieldCountGet()-1,
    recIdx,
    recIdx
  );
}
//--------------------------------------------------------------------------------

wxString EsListViewModel::asString(ulong fldStart, ulong fldEnd, ulong recStart, ulong recEnd) const
{
  ES_ASSERT(!isEmpty());
  ES_ASSERT(fldStart <= fldEnd);
  ES_ASSERT(recStart <= recEnd);
  ES_ASSERT(fldStart < fieldCountGet());
  ES_ASSERT(fldEnd < fieldCountGet());
  ES_ASSERT(recStart < recordCountGet());
  ES_ASSERT(recEnd < recordCountGet());
  
  wxString result;

  for(ulong recIdx = recStart; recIdx <= recEnd; ++recIdx)
  {
    wxString line;
    for(ulong fldIdx = fldStart; fldIdx <= fldEnd; ++fldIdx)
    {
      if(!result.empty())
        line += esT("||");

      line += fieldTextGet(
        recIdx,
        fldIdx
      );
    }

    result += line + esT("\n\r");
  }

  return result;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsListViewBase::EsListViewBase(wxWindow* parent, const EsListViewModel::Ptr& model, wxWindowID id /*= wxID_ANY*/) :
wxListCtrl(
  parent, 
  id, 
  wxDefaultPosition,
  wxDefaultSize,
  wxLC_REPORT|wxLC_VIRTUAL|wxLC_VRULES|wxLC_HRULES
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
}
//--------------------------------------------------------------------------------

EsListViewBase::EsListViewBase(wxWindow* parent, wxWindowID id /*= wxID_ANY*/) :
wxListCtrl(
  parent,
  id,
  wxDefaultPosition,
  wxDefaultSize,
  wxLC_REPORT|wxLC_VIRTUAL|wxLC_VRULES|wxLC_HRULES
),
m_ptb(nullptr),
m_needRefresh(false)
{
  Bind(
    wxEVT_CONTEXT_MENU,
    &EsListViewBase::onCtxMenu,
    this
  );
}
//--------------------------------------------------------------------------------

EsListViewBase::~EsListViewBase()
{
  Unbind(
    wxEVT_CONTEXT_MENU,
    &EsListViewBase::onCtxMenu,
    this
  );
}
//--------------------------------------------------------------------------------

void EsListViewBase::modelSet(const EsListViewModel::Ptr& model)
{
  wxWindowUpdateLocker lock(this);

  DeleteAllColumns();
  SetItemCount(0);

  m_model = model;
  if( m_model )
    fromModelInit();
}
//--------------------------------------------------------------------------------

void EsListViewBase::fromModelInit()
{
  ES_ASSERT(m_model);

  // Set-up columns
  for(ulong idx = 0; idx < m_model->fieldCountGet(); ++idx)
  {
    long col = InsertColumn(
      idx,
      EsStringI18n::translate(
        m_model->fieldNameGet(idx).t_str()
      ).c_str()
    );

    int minWidth = m_model->fieldMinWidthGet(idx);
    if(0 < minWidth)
      SetColumnWidth(
        col,
        minWidth
      );
  }

  // Force list refresh
  refresh(true);
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
    ulong size = m_model->recordCountGet();
    if(GetItemCount() != size)
    {
      SetItemCount(size);
      if(size)
        EnsureVisible(size-1);
    }

    Refresh();
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
    wxListItem col;
    if(GetColumn(idx, col))
    {
      col.SetText(
        EsStringI18n::translate(
          m_model->fieldNameGet(idx).t_str()
        ).c_str()
      );
    }
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

wxMenuPtr EsListViewBase::ctxMenuGet()
{
  if(m_pmnu)
    return m_pmnu;

  m_pmnu = std::make_shared<wxMenu>();
  ES_ASSERT(m_pmnu);

  ctxMenuInit();

  evtHandlersSet(
    this,
    true
  );

  return m_pmnu;
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
