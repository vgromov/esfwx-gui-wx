#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoSocketClientConfigPane.h"
//--------------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_IO_SOCKET
//--------------------------------------------------------------------------------

EsChannelIoSocketClientConfigPane::
ConfigPaneWnd::ConfigPaneWnd(EsChannelIoSocketClientConfigPane& pane, wxWindow* parent) :
PaneWnd(
  pane,
  parent
),
m_reset(nullptr),
m_edAddr(nullptr),
m_lblAddr(nullptr),
m_edPort(nullptr),
m_lblPort(nullptr),
m_edTmo(nullptr),
m_lblTmo(nullptr)
{
  m_reset = new wxButton(this, wxID_ANY, _("Reset to defaults").c_str());
  ES_ASSERT(m_reset);

	m_layContents->Add(
    m_reset, 
    wxSizerFlags().Border()
  );

	wxFlexGridSizer* ctls = new wxFlexGridSizer(3, 2, 0, 0);
	wxASSERT(ctls);
	ctls->AddGrowableCol(1, 1);
	wxSizerFlags lblFlags;
	lblFlags.Border().CenterVertical().Left();

  m_edAddr = new wxTextCtrl(this, wxID_ANY);
  ES_ASSERT(m_edAddr);

  m_lblAddr = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblAddr);

	ctls->Add(m_lblAddr, lblFlags);
	ctls->Add(m_edAddr, wxSizerFlags(1).Border().Expand());
	
  m_edPort = new wxSpinCtrl(this, wxID_ANY);
  ES_ASSERT(m_edPort);

  m_lblPort = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblPort);
  
  ctls->Add(m_lblPort, lblFlags);
	ctls->Add(m_edPort, wxSizerFlags(1).Border().Expand());

  m_edTmo = new wxSpinCtrl(this, wxID_ANY);
  ES_ASSERT(m_edTmo);

  m_lblTmo = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblTmo);

	ctls->Add(m_lblTmo, lblFlags);
	ctls->Add(m_edTmo, wxSizerFlags(1).Border().Expand());

  m_layContents->Add(ctls, wxSizerFlags(1).Border(wxALL, 0).Expand());

	m_reset->Bind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );
}
//--------------------------------------------------------------------------------

EsChannelIoSocketClientConfigPane::
ConfigPaneWnd::~ConfigPaneWnd()
{
	m_reset->Unbind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );

  ES_DEBUG_TRACE(
    esT("EsChannelIoSocketClientConfigPane::~ConfigPaneWnd")
  );
}
//--------------------------------------------------------------------------------

void EsChannelIoSocketClientConfigPane::
ConfigPaneWnd::onResetToDefaults(wxCommandEvent& evt)
{
	m_intf.controlsResetToDefault();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoSocketClientConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoSocketClientConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoSocketClientConfigPane );
  ES_ASSERT(ptr);

  ptr->init(
    parent,
    EsChannelIoSocketClient::classNameGetStatic(),
    nullptr
  );

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

EsChannelIoSocketClientConfigPane::~EsChannelIoSocketClientConfigPane()
{
  ES_DEBUG_TRACE(esT("EsChannelIoSocketClientConfigPane::~EsChannelIoSocketClientConfigPane"));
}
//--------------------------------------------------------------------------------

EsString EsChannelIoSocketClientConfigPane::typeNameGet() const ES_NOTHROW
{
  return classNameGetStatic();
}
//--------------------------------------------------------------------------------

EsReflectedClassConfigPane::PaneWnd* EsChannelIoSocketClientConfigPane::doPaneWndCreate(wxWindow* parent)
{
  PaneWnd* wnd = new ConfigPaneWnd(
    *this, 
    parent
  );
  ES_ASSERT(wnd);

  return wnd;
}
//--------------------------------------------------------------------------------

EsReflectedClassDataSource* EsChannelIoSocketClientConfigPane::doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta)
{
  ConfigPaneWnd* pane = dynamic_cast<ConfigPaneWnd*>( m_pane.get() );
  ES_ASSERT(pane);

  std::unique_ptr<EsReflectedClassContainerDataSource> src = ES_MAKE_UNIQUE(
    EsReflectedClassContainerDataSource,
    m_pane,
    className,
    nullptr
  );
  ES_ASSERT(src);

	// initialize property links
	src->link(
    EsTextCtlPropertyLink::create(
      esT("target"), 
      pane->m_edAddr, 
      pane->m_lblAddr
    )
  );
	src->link(
    EsSpinCtlPropertyLink::create(
      esT("targetPort"), 
      pane->m_edPort, 
      pane->m_lblPort
    )
  );
	src->link(
    EsSpinCtlPropertyLink::create(
      esT("operationTimeout"), 
      pane->m_edTmo, 
      pane->m_lblTmo
    )
  );

  return src.release();
}
//--------------------------------------------------------------------------------

#endif //< ES_COMM_USE_CHANNEL_IO_SOCKET
