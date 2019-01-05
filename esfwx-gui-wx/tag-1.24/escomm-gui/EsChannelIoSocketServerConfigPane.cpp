#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoSocketServerConfigPane.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChannelIoSocketServerConfigPane::EsChannelIoSocketServerConfigPane(wxWindow* parent) :
EsReflectedClassConfigPane(
  parent,
  EsChannelIoSocketServer::classNameGetStatic()
),
m_reset(nullptr),
m_lblPort(nullptr),
m_edPort(nullptr),
m_lblTmo(nullptr),
m_edTmo(nullptr)
{
  m_reset = new wxButton(this, wxID_ANY, _("Reset to defaults"));
  ES_ASSERT(m_reset);
	m_layContents->Add(m_reset, 0, wxALL, 5);

	wxFlexGridSizer* ctls = new wxFlexGridSizer(2, 2, 0, 0);
	wxASSERT(ctls);
	ctls->AddGrowableCol(1, 1);
	wxSizerFlags lblFlags;
	lblFlags.Border().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

  m_lblPort = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblPort);
  
  m_edPort = new wxSpinCtrl(this, wxID_ANY);
  ES_ASSERT(m_edPort);

	ctls->Add(m_lblPort, lblFlags);
	ctls->Add(m_edPort, 1, wxALL|wxEXPAND, 5);

  m_lblTmo = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblTmo);

  m_edTmo = new wxSpinCtrl(this, wxID_ANY);
  ES_ASSERT(m_edTmo);

	ctls->Add(m_lblTmo, lblFlags);
	ctls->Add(m_edTmo, 1, wxALL|wxEXPAND, 5);

	m_layContents->Add(ctls, 1, wxALL|wxEXPAND, 0);	

	Layout();
	
	// initialize property links
	m_src.link(
    EsSpinCtlPropertyLink::create(
      esT("port"), 
      m_edPort, 
      m_lblPort
    )
  );
	m_src.link(
    EsSpinCtlPropertyLink::create(
      esT("operationTimeout"), 
      m_edTmo, 
      m_lblTmo
    )
  );

	m_reset->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoSocketServerConfigPane::onResetToDefaults, this);
}
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoSocketServerConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoSocketServerConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoSocketServerConfigPane, parent );
  ES_ASSERT(ptr);

  return ptr.release()->intfGet();
}
//--------------------------------------------------------------------------------

EsChannelIoSocketServerConfigPane::~EsChannelIoSocketServerConfigPane()
{
  ES_DEBUG_TRACE(esT("EsChannelIoSocketServerConfigPane::~EsChannelIoSocketServerConfigPane"));

	m_reset->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoSocketServerConfigPane::onResetToDefaults, this);
}
//--------------------------------------------------------------------------------

void EsChannelIoSocketServerConfigPane::onResetToDefaults(wxCommandEvent& evt)
{
	m_src.resetControlsToDefault();
}
//--------------------------------------------------------------------------------
