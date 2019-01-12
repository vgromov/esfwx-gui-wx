#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsProtocolEcoeConfigPane.h"
//--------------------------------------------------------------------------------

#ifdef ES_COMM_USE_RPC
//--------------------------------------------------------------------------------

EsProtocolEcoeConfigPane::
ConfigPaneWnd::ConfigPaneWnd(EsProtocolEcoeConfigPane& pane, wxWindow* parent) :
PaneWnd(
  pane, 
  parent
),
m_reset(nullptr),
m_lblPacketTmo(nullptr),
m_edPacketTmo(nullptr),
m_lblPacketRetries(nullptr),
m_edPacketRetries(nullptr),
m_lblSlaveAddr(nullptr),
m_edSlaveAddr(nullptr)
{
	wxFlexGridSizer* ctls = new wxFlexGridSizer(2);
	wxASSERT(ctls);	
	ctls->AddGrowableCol( 1 );
	ctls->SetFlexibleDirection( wxHORIZONTAL );
	ctls->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );	
	
	m_reset = new wxButton( 
    this, 
    wxID_ANY, 
    _("Reset to defaults") 
  );
	wxASSERT(m_reset);
	m_layContents->Add(
    m_reset, 
    wxSizerFlags().Border()
  );
	
	// packet tmo
	wxSizerFlags lblFlags;
	lblFlags.Border().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

  m_lblPacketTmo = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblPacketTmo);

  m_edPacketTmo = new wxSpinCtrl(this, wxID_ANY);
  ES_ASSERT(m_edPacketTmo);
  
	ctls->Add(
    m_lblPacketTmo, 
    lblFlags
  );
	ctls->Add(
    m_edPacketTmo, 
    wxSizerFlags().Expand().Border()
  );
	
  // packet retries
	m_lblPacketRetries = new wxStaticText(
    this, 
    wxID_ANY, 
    wxEmptyString
  );
  ES_ASSERT(m_lblPacketRetries);

  m_edPacketRetries = new wxSpinCtrl(
    this, 
    wxID_ANY
  );
  ES_ASSERT(m_edPacketRetries);

  ctls->Add(
    m_lblPacketRetries, 
    lblFlags
  );
	ctls->Add(
    m_edPacketRetries, 
    wxSizerFlags().Expand().Border()
  );
	
  // slave address
  m_lblSlaveAddr = new wxStaticText(
    this, 
    wxID_ANY, 
    wxEmptyString
  );
  ES_ASSERT(m_lblSlaveAddr);

  m_edSlaveAddr = new wxSpinCtrl(
    this, 
    wxID_ANY
  );
  ES_ASSERT(m_edSlaveAddr);

	ctls->Add(
    m_lblSlaveAddr, 
    lblFlags
  );
	ctls->Add(
    m_edSlaveAddr, 
    wxSizerFlags().Expand().Border()
  );
	
  // finalize sizer
	m_layContents->Add(
    ctls, 
    wxSizerFlags().Expand().Border(wxALL, 0)
  );

	// connect events
	m_reset->Bind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );
}
//--------------------------------------------------------------------------------

EsProtocolEcoeConfigPane::
ConfigPaneWnd::~ConfigPaneWnd()
{
  ES_DEBUG_TRACE(esT("EsProtocolEcoeConfigPane::~ConfigPaneWnd"));

	m_reset->Unbind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );
}
//--------------------------------------------------------------------------------

void EsProtocolEcoeConfigPane::
ConfigPaneWnd::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
  if( !m_orphan )
    m_intf.dataSourceAccess().resetControlsToDefault();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsProtocolEcoeConfigPane::~EsProtocolEcoeConfigPane()
{
  ES_DEBUG_TRACE(
    esT("EsProtocolEcoeConfigPane::~EsProtocolEcoeConfigPane")
  );
}
//--------------------------------------------------------------------------------

EsString EsProtocolEcoeConfigPane::typeNameGet() const ES_NOTHROW
{
  return classNameGetStatic();
}
//--------------------------------------------------------------------------------

EsReflectedClassConfigPane::PaneWnd* EsProtocolEcoeConfigPane::doPaneWndCreate(wxWindow* parent)
{
  ConfigPaneWnd* wnd = new ConfigPaneWnd(*this, parent);
  ES_ASSERT(wnd);

  return wnd;
}
//--------------------------------------------------------------------------------

EsReflectedClassDataSource* EsProtocolEcoeConfigPane::doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& ES_UNUSED(meta))
{
  ConfigPaneWnd* pane = wxDynamicCast( m_pane.get(), ConfigPaneWnd );
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
    EsSpinCtlPropertyLink::create(
      esT("packetTimeout"), 
      pane->m_edPacketTmo, 
      pane->m_lblPacketTmo
     )
  );
	src->link(
    EsSpinCtlPropertyLink::create(
      esT("packetRetries"), 
      pane->m_edPacketRetries, 
      pane->m_lblPacketRetries
    )
  );
	src->link(
    EsSpinCtlPropertyLink::create(
      esT("clientAddr"), 
      pane->m_edSlaveAddr, 
      pane->m_lblSlaveAddr
    )
  );

  return src.release();
}
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsProtocolEcoeConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsProtocolEcoeConfigPane> ptr = ES_MAKE_UNIQUE( EsProtocolEcoeConfigPane );
  ES_ASSERT(ptr);

  ptr->init(
    parent,
    EsRpcMaster::classNameGetStatic(),
    nullptr
  );

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

#endif //< ES_COMM_USE_RPC
