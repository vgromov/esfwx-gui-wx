#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoSocketClientConfigPane.h"

EsChannelIoSocketClientConfigPane::EsChannelIoSocketClientConfigPane(wxWindow* parent) :
EsReflectedClassConfigPane(
  parent, 
  EsChannelIoSocketClient::classNameGetStatic() 
),
m_intf(*this),
m_reset(new wxButton(this, wxID_ANY, _("Reset to defaults"))),
m_edAddr(new wxTextCtrl(this, wxID_ANY)),
m_lblAddr(new wxStaticText(this, wxID_ANY, wxEmptyString)),
m_edPort(new wxSpinCtrl(this, wxID_ANY)),
m_lblPort(new wxStaticText(this, wxID_ANY, wxEmptyString)),
m_edTmo(new wxSpinCtrl(this, wxID_ANY)),
m_lblTmo(new wxStaticText(this, wxID_ANY, wxEmptyString))
{
	wxBoxSizer* contents = new wxBoxSizer(wxVERTICAL);
	wxASSERT(contents);
	contents->Add(m_reset, 0, wxALL, 5);

	wxFlexGridSizer* ctls = new wxFlexGridSizer(3, 2, 0, 0);
	wxASSERT(ctls);
	ctls->AddGrowableCol(1, 1);
	wxSizerFlags lblFlags;
	lblFlags.Border().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

	ctls->Add(m_lblAddr, lblFlags);
	ctls->Add(m_edAddr, 1, wxALL|wxEXPAND, 5);
	ctls->Add(m_lblPort, lblFlags);
	ctls->Add(m_edPort, 1, wxALL|wxEXPAND, 5);
	ctls->Add(m_lblTmo, lblFlags);
	ctls->Add(m_edTmo, 1, wxALL|wxEXPAND, 5);
	contents->Add(ctls, 1, wxALL|wxEXPAND, 0);

	SetSizer( contents );
	Layout();

	// initialize property links
	m_links
		.linkAdd(EsTextCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoSocketClient, target), m_edAddr, m_lblAddr))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoSocketClient, targetPort), m_edPort, m_lblPort))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoSocketClient, operationTimeout), m_edTmo, m_lblTmo));

	m_reset->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoSocketClientConfigPane::onResetToDefaults, this);
}

EsChannelIoSocketClientConfigPane::~EsChannelIoSocketClientConfigPane()
{
	m_reset->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoSocketClientConfigPane::onResetToDefaults, this);
}

// reset button click handler
void EsChannelIoSocketClientConfigPane::onResetToDefaults(wxCommandEvent& evt)
{
	m_links.resetControlsToDefault();
}
