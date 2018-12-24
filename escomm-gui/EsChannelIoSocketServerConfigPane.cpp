#include "stdafx.h"
#pragma hdrstop

#include "EsChannelIoSocketServerConfigPane.h"

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(EsChannelIoSocketServerConfigPaneWnd::EsChannelIoSocketServerConfigPane, EsChannelIoSocketServerConfigPane, _i("Client socket channel configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(EsChannelIoSocketServerConfigPaneWnd::EsChannelIoSocketServerConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, _i("EsChannelIoSocketServerConfigPane constructor."))
ES_DECL_CLASS_INFO_END

EsChannelIoSocketServerConfigPaneWnd::EsChannelIoSocketServerConfigPane::~EsChannelIoSocketServerConfigPane() {}

EsString EsChannelIoSocketServerConfigPaneWnd::EsChannelIoSocketServerConfigPane::getObjectName() const 
{ 
	return EsChannelIoSocketServer::getClassInfoStatic().nameGet(); 
}

// configuration pane itself
//
EsChannelIoSocketServerConfigPaneWnd::EsChannelIoSocketServerConfigPaneWnd(wxWindow* parent) :
wxPanel(parent),
m_intf(*this),
m_reset(new wxButton(this, wxID_ANY, _("Reset to defaults"))),
m_lblPort(new wxStaticText(this, wxID_ANY, wxEmptyString)),
m_edPort(new wxSpinCtrl(this, wxID_ANY)),
m_lblTmo(new wxStaticText(this, wxID_ANY, wxEmptyString)),
m_edTmo(new wxSpinCtrl(this, wxID_ANY))
{
	wxBoxSizer* contents = new wxBoxSizer(wxVERTICAL);
	wxASSERT(contents);
	contents->Add(m_reset, 0, wxALL, 5);

	wxFlexGridSizer* ctls = new wxFlexGridSizer(2, 2, 0, 0);
	wxASSERT(ctls);
	ctls->AddGrowableCol(1, 1);
	wxSizerFlags lblFlags;
	lblFlags.Border().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

	ctls->Add(m_lblPort, lblFlags);
	ctls->Add(m_edPort, 1, wxALL|wxEXPAND, 5);
	ctls->Add(m_lblTmo, lblFlags);
	ctls->Add(m_edTmo, 1, wxALL|wxEXPAND, 5);
	contents->Add(ctls, 1, wxALL|wxEXPAND, 0);	

	SetSizer( contents );
	Layout();
	
	// initialize property links
	m_links
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoSocketServer, port), m_edPort, m_lblPort))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoSocketServer, operationTimeout), m_edTmo, m_lblTmo));

	m_reset->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoSocketServerConfigPaneWnd::onResetToDefaults, this);
}

EsChannelIoSocketServerConfigPaneWnd::~EsChannelIoSocketServerConfigPaneWnd()
{
	m_reset->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoSocketServerConfigPaneWnd::onResetToDefaults, this);
}

// reset button click handler
void EsChannelIoSocketServerConfigPaneWnd::onResetToDefaults(wxCommandEvent& evt)
{
	m_links.resetControlsToDefault();
}
