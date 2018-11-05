#include "escore_gui_pch.h"
#pragma hdrstop

#include <wx/containr.h>
#include <wx/dialog.h>
#include <wx/msgdlg.h>

#include "EsDlgBase.h"
//---------------------------------------------------------------------------

EsDlgBase::EvtHandler::EvtHandler(EsDlgBase& owner) :
  EsEventSubscriber(ES_EVTC_I18N),
  m_owner(owner)
{
  activeSet(false);
}
//---------------------------------------------------------------------------

void EsDlgBase::EvtHandler::notify(const EsEventIntf::Ptr& evt)
{
  if (ES_EVTC_I18N == evt->categoryGet() && as_<ulong>(EsEvtI18n::LocaleChanged) == evt->idGet())
    m_owner.onI18nLocaleChanged( evt->payloadGet().asString() );
  else
    m_owner.onNotify(evt);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsDlgBase::EsDlgBase(wxWindow* Owner, const wxString& title /*= wxEmptyString*/, long style /*= wxDEFAULT_DIALOG_STYLE*/) :
wxDialog(
  Owner, 
  wxID_ANY, 
  wxEmptyString, 
  wxDefaultPosition,
  wxDefaultSize,
  style
),
m_handler(*this),
m_title(title),
m_internals(nullptr),
m_contents(nullptr),
m_btns(nullptr),
m_contentsFinalized(false)
{
  m_internals = new wxBoxSizer(wxVERTICAL);
  ES_ASSERT(m_internals);

  m_contents = new wxBoxSizer(wxVERTICAL);
  ES_ASSERT(m_contents);

  m_btns = new wxStdDialogButtonSizer;
  ES_ASSERT(m_btns);
}
//---------------------------------------------------------------------------

EsDlgBase::~EsDlgBase()
{
  if( !m_contentsFinalized )
  {
    ES_DELETE(m_btns);
    ES_DELETE(m_contents);
    ES_DELETE(m_internals);
  }
}
//---------------------------------------------------------------------------

void EsDlgBase::onNotify(const EsEventIntf::Ptr& evt)
{}
//--------------------------------------------------------------------------------

void EsDlgBase::onI18nLocaleChanged(const EsString& ES_UNUSED(loc))
{
  SetTitle(
    EsStringI18n::translate(
      m_title.t_str()
    ).c_str()
  );
}
//---------------------------------------------------------------------------

void EsDlgBase::contentsFinalize()
{
  if( m_contentsFinalized )
    return;

  m_btns->Realize();
  m_internals->Add(
    m_contents,
    wxSizerFlags(1).Expand().Border()
  );
  m_internals->Add(
    m_btns,
    wxSizerFlags().Expand().Border()
  );
  
  const wxSize& minSize = GetMinSize();
  m_internals->SetMinSize(minSize);

  SetSizerAndFit(m_internals);

  m_contentsFinalized = true;
}
//--------------------------------------------------------------------------------

void EsDlgBase::showModal(std::function<void(const EsDlgBase& Sender, int modalResult)> onClose)
{
  onI18nLocaleChanged(
    EsStringI18n::instanceGet().currentLocaleGet()
  );

  // Finalize sizer contents
  contentsFinalize();
  
  m_handler.activeSet(true);

  int mr = ShowModal();

  m_handler.activeSet(false);

  if( onClose )
    onClose(
      *this, 
      mr
  );
}
//---------------------------------------------------------------------------
