#include "stdafx.h"
#pragma hdrstop

#include <wx/spinctrl.h>
#include "EsProtocolConfigPane.h"

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(EsProtocolConfigPaneWnd::EsProtocolConfigPane, EsProtocolConfigPane, wxT("Ekosf protocol configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(EsProtocolConfigPaneWnd::EsProtocolConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, wxT("EsProtocolConfigPane constructor"))
ES_DECL_CLASS_INFO_END

EsProtocolConfigPaneWnd::EsProtocolConfigPane::~EsProtocolConfigPane() {}

EsProtocolConfigPaneWnd::EsProtocolConfigPaneWnd(wxWindow* parent) : 
wxPanel(parent),
m_intf(*this),
m_lblPacketTmo(new wxStaticText(this, wxID_ANY, wxEmptyString)),
m_edPacketTmo(new wxSpinCtrl(this, wxID_ANY)),
m_lblPacketRetries(new wxStaticText(this, wxID_ANY, wxEmptyString)),
m_edPacketRetries(new wxSpinCtrl(this, wxID_ANY)),
m_lblSlaveAddr(new wxStaticText(this, wxID_ANY, wxEmptyString)),
m_edSlaveAddr(new wxSpinCtrl(this, wxID_ANY))
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	wxASSERT(sizer);
	
	wxFlexGridSizer* ctls = new wxFlexGridSizer(2);
	wxASSERT(ctls);	
	ctls->AddGrowableCol( 1 );
	ctls->SetFlexibleDirection( wxHORIZONTAL );
	ctls->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );	
	
	wxButton* reset = new wxButton( this, wxID_ANY, _("Reset to defaults") );
	wxASSERT(reset);
	sizer->Add(reset, wxSizerFlags().Border());
	
	// packet tmo
	wxSizerFlags lblFlags;
	lblFlags.Border().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

	ctls->Add(m_lblPacketTmo, lblFlags);
	ctls->Add(m_edPacketTmo, wxSizerFlags().Expand().Border());
	// packet retries
	ctls->Add(m_lblPacketRetries, lblFlags);
	ctls->Add(m_edPacketRetries, wxSizerFlags().Expand().Border());
	// slave address
	ctls->Add(m_lblSlaveAddr, lblFlags);
	ctls->Add(m_edSlaveAddr, wxSizerFlags().Expand().Border());
	// finalize sizer
	sizer->Add(ctls, wxSizerFlags().Expand().Border(wxALL, 0));
	SetSizer(sizer);
	
	// initialize property links
	m_links
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetTimeout), m_edPacketTmo, m_lblPacketTmo))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetRetries), m_edPacketRetries, m_lblPacketRetries))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, clientAddr), m_edSlaveAddr, m_lblSlaveAddr));

	// connect events
	reset->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &EsProtocolConfigPaneWnd::onResetToDefaults, this);
}

void EsProtocolConfigPaneWnd::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
	m_links.resetControlsToDefault();
}