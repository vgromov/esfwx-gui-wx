#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoSocketClientConfigPane.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChannelIoSocketClientConfigPane::EsChannelIoSocketClientConfigPane(wxWindow* parent) :
EsReflectedClassConfigPane(
  parent, 
  EsChannelIoSocketClient::classNameGetStatic() 
),
m_reset(nullptr),
m_edAddr(nullptr),
m_lblAddr(nullptr),
m_edPort(nullptr),
m_lblPort(nullptr),
m_edTmo(nullptr),
m_lblTmo(nullptr)
{
  m_reset = new wxButton(this, wxID_ANY, _("Reset to defaults"));
  ES_ASSERT(m_reset);

	m_layContents->Add(m_reset, 0, wxALL, 5);

	wxFlexGridSizer* ctls = new wxFlexGridSizer(3, 2, 0, 0);
	wxASSERT(ctls);
	ctls->AddGrowableCol(1, 1);
	wxSizerFlags lblFlags;
	lblFlags.Border().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

  m_edAddr = new wxTextCtrl(this, wxID_ANY);
  ES_ASSERT(m_edAddr);

  m_lblAddr = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblAddr);

	ctls->Add(m_lblAddr, lblFlags);
	ctls->Add(m_edAddr, 1, wxALL|wxEXPAND, 5);
	
  m_edPort = new wxSpinCtrl(this, wxID_ANY);
  ES_ASSERT(m_edPort);

  m_lblPort = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblPort);
  
  ctls->Add(m_lblPort, lblFlags);
	ctls->Add(m_edPort, 1, wxALL|wxEXPAND, 5);

  m_edTmo = new wxSpinCtrl(this, wxID_ANY);
  ES_ASSERT(m_edTmo);

  m_lblTmo = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblTmo);

	ctls->Add(m_lblTmo, lblFlags);
	ctls->Add(m_edTmo, 1, wxALL|wxEXPAND, 5);

  m_layContents->Add(ctls, 1, wxALL|wxEXPAND, 0);

	Layout();

	// initialize property links
	m_src.link(
    EsTextCtlPropertyLink::create(
      esT("target"), 
      m_edAddr, 
      m_lblAddr
    )
  );
	m_src.link(
    EsSpinCtlPropertyLink::create(
      esT("targetPort"), 
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

	m_reset->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoSocketClientConfigPane::onResetToDefaults, this);
}
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoSocketClientConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoSocketClientConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoSocketClientConfigPane, parent );
  ES_ASSERT(ptr);

  return ptr.release()->intfGet();
}
//--------------------------------------------------------------------------------

EsChannelIoSocketClientConfigPane::~EsChannelIoSocketClientConfigPane()
{
  ES_DEBUG_TRACE(esT("EsChannelIoSocketClientConfigPane::~EsChannelIoSocketClientConfigPane"));

	m_reset->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoSocketClientConfigPane::onResetToDefaults, this);
}
//--------------------------------------------------------------------------------

void EsChannelIoSocketClientConfigPane::onResetToDefaults(wxCommandEvent& evt)
{
	m_src.resetControlsToDefault();
}
//--------------------------------------------------------------------------------
