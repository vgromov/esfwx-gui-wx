#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsProtocolEcoeConfigPane.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsProtocolEcoeConfigPane::EsProtocolEcoeConfigPane(wxWindow* parent) : 
EsReflectedClassConfigPane(
  parent,
  EsRpcMaster::classNameGetStatic()
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
	
	// initialize property links
	m_src.link(
    EsSpinCtlPropertyLink::create(
      esT("packetTimeout"), 
      m_edPacketTmo, 
      m_lblPacketTmo
     )
  );
	m_src.link(
    EsSpinCtlPropertyLink::create(
      esT("packetRetries"), 
      m_edPacketRetries, 
      m_lblPacketRetries
    )
  );
	m_src.link(
    EsSpinCtlPropertyLink::create(
      esT("clientAddr"), 
      m_edSlaveAddr, 
      m_lblSlaveAddr
    )
  );

	// connect events
	m_reset->Bind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &EsProtocolEcoeConfigPane::onResetToDefaults, 
    this
  );
}
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsProtocolEcoeConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsProtocolEcoeConfigPane> ptr = ES_MAKE_UNIQUE( EsProtocolEcoeConfigPane, parent );
  ES_ASSERT(ptr);

  return ptr.release()->intfGet();
}
//--------------------------------------------------------------------------------

EsProtocolEcoeConfigPane::~EsProtocolEcoeConfigPane()
{
	m_reset->Unbind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &EsProtocolEcoeConfigPane::onResetToDefaults, 
    this
  );
}
//--------------------------------------------------------------------------------

void EsProtocolEcoeConfigPane::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
	m_src.resetControlsToDefault();
}
//--------------------------------------------------------------------------------
