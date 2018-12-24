#include "stdafx.h"
#pragma hdrstop

#include "EsChannelIoStubConfigPane.h"

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(EsChannelIoStubConfigPaneWnd::EsChannelIoStubConfigPane, EsChannelIoStubConfigPane, _i("Stub channel configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(EsChannelIoStubConfigPaneWnd::EsChannelIoStubConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, _i("EsChannelIoStubConfigPane constructor."))
ES_DECL_CLASS_INFO_END

EsChannelIoStubConfigPaneWnd::EsChannelIoStubConfigPane::~EsChannelIoStubConfigPane() {}

EsString EsChannelIoStubConfigPaneWnd::EsChannelIoStubConfigPane::getObjectName() const 
{ 
	return EsChannelIoStub::getClassInfoStatic().nameGet(); 
}

// configuration pane itself
//
EsChannelIoStubConfigPaneWnd::EsChannelIoStubConfigPaneWnd(wxWindow* parent) :
wxPanel(parent),
m_intf(*this)
{
	wxBoxSizer* contents;
	contents = new wxBoxSizer( wxVERTICAL );
	contents->Add( new wxStaticText(this, wxID_ANY, _("Stub Io channel does not have any adjustable settings")),
		wxSizerFlags(1).Align(wxALIGN_CENTER) );
	
//	m_btnResetToDefaults = new wxButton( this, wxID_ANY, _("Reset to defaults"), wxDefaultPosition, wxDefaultSize, 0 );
//	contents->Add( m_btnResetToDefaults, 0, wxALL, 5 );
	
	SetSizer( contents );
	Layout();	
}

// reset button click handler
//void EsChannelIoStubConfigPaneWnd::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
//{
//	m_intf.setChannelConfigString(EsString::s_null);	
//}