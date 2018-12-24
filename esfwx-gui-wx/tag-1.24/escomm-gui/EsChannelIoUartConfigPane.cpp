#include "stdafx.h"
#pragma hdrstop

#include <wx/spinctrl.h>
#include "EsChannelIoUartConfigPane.h"
#include "res/rescan.xpm"

const wxChar* c_sysPortPrefix = wxT("\\\\.\\");

// custom property link for com port enumeration
//
// combo box link
class EsUartIoPortPropertyLink : public EsPropertyLink
{
protected:
	EsUartIoPortPropertyLink(const EsPropertyInfo& propInfo,
		wxComboBox* cbx, wxStaticText* lbl, const EsCommPortInfos& portInfos) :
	EsPropertyLink(propInfo),
	m_cbx(0),
	m_lbl(0),
	m_portInfos(portInfos)
	{
		init(cbx, lbl);
	}

public:
	static EsPropertyLink::Ptr create(const EsPropertyInfo& propInfo,
		wxComboBox* cbx, wxStaticText* lbl, const EsCommPortInfos& portInfos)
	{
		EsPropertyLink::Ptr result(new EsUartIoPortPropertyLink(propInfo, cbx, lbl, portInfos));
		result->resetControlToDefault();
		return result;
	}
	
	virtual void updateControl(const EsReflectedClassIntf::Ptr& obj)
	{
		wxASSERT(obj);
		wxASSERT(obj->is(m_propInfo.getClassInfo().nameGet()));
		
		EsString port = m_propInfo.get(obj.request<EsBaseIntf>().get()).asString();
		const EsCommPortInfo& infoByName = m_portInfos.findPortInfoByPortName( port );
		if( infoByName.isOk() )
			m_cbx->SetValue( infoByName.m_strFriendlyName.c_str() );
		else
		{
			const EsCommPortInfo& infoByDev = m_portInfos.findPortInfoByDevPath( port );
			if( infoByDev.isOk() )
				m_cbx->SetValue( infoByDev.m_strFriendlyName.c_str() );
			else
				m_cbx->SetSelection(-1);
		}
	}
	
	virtual void updateProperty(const EsReflectedClassIntf::Ptr& obj) const
	{
		wxASSERT(obj);
		wxASSERT(obj->is(m_propInfo.getClassInfo().nameGet()));
		int sel = m_cbx->GetSelection();
		if( -1 < sel )
			m_propInfo.set(obj.request<EsBaseIntf>().get(), m_portInfos[sel].m_strDevPath);	
	}
	
	virtual void resetControlToDefault()
	{
		// set default port
		if( !m_portInfos.empty() )
		{
			wxString simpleName;
			wxString portName = m_propInfo.defaultGet().asString();
			if( !portName.StartsWith(c_sysPortPrefix, &simpleName) )
				simpleName = portName;
			const EsCommPortInfo& info = m_portInfos.findPortInfoByPortName(simpleName);
			if( info.isOk() )
				m_cbx->SetValue(info.m_strFriendlyName.c_str());
			else
				m_cbx->SetValue(wxEmptyString);
		}
	}

protected:
	void init(wxComboBox* cbx, wxStaticText* lbl)
	{
		m_lbl = lbl;
		if( m_lbl )
			m_lbl->SetLabel( m_propInfo.labelGet() + wxT(":") );

		wxASSERT(cbx);
		m_cbx = cbx;
	}

protected:
	wxComboBox* m_cbx;
	wxStaticText* m_lbl;
	const EsCommPortInfos& m_portInfos;
};

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(EsChannelIoUartConfigPaneWnd::EsChannelIoUartConfigPane, EsChannelIoUartConfigPane, _i("UART channel configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(EsChannelIoUartConfigPaneWnd::EsChannelIoUartConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, _i("EsChannelIoUartConfigPane constructor."))
ES_DECL_CLASS_INFO_END

EsChannelIoUartConfigPaneWnd::EsChannelIoUartConfigPane::~EsChannelIoUartConfigPane() {}

EsString EsChannelIoUartConfigPaneWnd::EsChannelIoUartConfigPane::getObjectName() const 
{ 
	return EsRpcMaster::getClassInfoStatic().nameGet(); 
}

// configuration pane itself
//
EsChannelIoUartConfigPaneWnd::EsChannelIoUartConfigPaneWnd(wxWindow* parent) :
wxPanel(parent),
m_intf(*this)
{
	wxBoxSizer* contents;
	contents = new wxBoxSizer( wxVERTICAL );
	
	m_btnResetToDefaults = new wxButton( this, wxID_ANY, _("Reset to defaults"), wxDefaultPosition, wxDefaultSize, 0 );
	contents->Add( m_btnResetToDefaults, 0, wxALL, 5 );
	
	m_settings = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_pnlStd = new wxPanel( m_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* controlsStd;
	controlsStd = new wxFlexGridSizer( 0, 2, 0, 0 );
	controlsStd->AddGrowableCol( 1 );
	controlsStd->SetFlexibleDirection( wxHORIZONTAL );
	controlsStd->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxSizerFlags lblFlags;
	lblFlags.Border().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

	m_lblPort = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblPort->Wrap( -1 );
	controlsStd->Add( m_lblPort, lblFlags);
	
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	m_edPortName = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	box->Add(m_edPortName, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL);
	box->AddSpacer(5);
	m_btnRescan = new wxBitmapButton( m_pnlStd, wxID_ANY, wxBitmap(rescan_xpm, wxBITMAP_TYPE_XPM));
	m_btnRescan->SetToolTip( _("Rescan serial ports") );
	box->Add(m_btnRescan, 0, wxALIGN_CENTER_VERTICAL);
	controlsStd->Add( box, 1, wxALL|wxEXPAND, 5 );
	
	m_lblBaud = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblBaud->Wrap( -1 );
	controlsStd->Add( m_lblBaud, lblFlags );
	
	m_edBaud = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edBaud, 1, wxALL|wxEXPAND, 5 );
	
	m_lblByteSize = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblByteSize->Wrap( -1 );
	controlsStd->Add( m_lblByteSize, lblFlags );
	
	m_edByteSize = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edByteSize, 1, wxALL|wxEXPAND, 5 );
	
	m_lblParity = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblParity->Wrap( -1 );
	controlsStd->Add( m_lblParity, lblFlags );
	
	m_edParity = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edParity, 1, wxALL|wxEXPAND, 5 );
	
	m_lblStopBits = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblStopBits->Wrap( -1 );
	controlsStd->Add( m_lblStopBits, lblFlags );
	
	m_edStopBits = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edStopBits, 1, wxALL|wxEXPAND, 5 );
	
	m_pnlStd->SetSizer( controlsStd );
	m_pnlStd->Layout();
	controlsStd->Fit( m_pnlStd );
	m_settings->AddPage( m_pnlStd, _("Generic"), true );
	m_pnlAdvanced = new wxPanel( m_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* controlsAdvanced;
	controlsAdvanced = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* ctlsGrid;
	ctlsGrid = new wxFlexGridSizer( 0, 2, 0, 0 );
	ctlsGrid->AddGrowableCol( 1 );
	ctlsGrid->SetFlexibleDirection( wxHORIZONTAL );
	ctlsGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lblRxBuffer = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblRxBuffer->Wrap( -1 );
	ctlsGrid->Add( m_lblRxBuffer, lblFlags );
	
	m_edRxBuff = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edRxBuff, 1, wxALL|wxEXPAND, 5 );
	
	m_lblTxBuffer = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblTxBuffer->Wrap( -1 );
	ctlsGrid->Add( m_lblTxBuffer, lblFlags );
	
	m_edTxBuff = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edTxBuff, 1, wxALL|wxEXPAND, 5 );
	
	controlsAdvanced->Add( ctlsGrid, 0, wxEXPAND, 5 );
	
	m_chkResetOnRxTmo = new wxCheckBox( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	controlsAdvanced->Add( m_chkResetOnRxTmo, 0, wxALL|wxEXPAND, 5 );
	
	m_pnlAdvanced->SetSizer( controlsAdvanced );
	m_pnlAdvanced->Layout();
	controlsAdvanced->Fit( m_pnlAdvanced );
	m_settings->AddPage( m_pnlAdvanced, _("Advanced"), false );
	
	contents->Add( m_settings, 1, wxEXPAND | wxALL, 5 );
	SetSizer( contents );
	Layout();	
	
	// enumerate ports
	populatePorts();
	// set-up property links
	m_links
		.linkAdd(EsUartIoPortPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoUart, port), m_edPortName, m_lblPort, m_ports))
		.linkAdd(EsComboBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoUart, baud), m_edBaud, m_lblBaud))
		.linkAdd(EsComboBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoUart, bits), m_edByteSize, m_lblByteSize))
		.linkAdd(EsComboBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoUart, parity), m_edParity, m_lblParity))
		.linkAdd(EsComboBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoUart, stopBits), m_edStopBits, m_lblStopBits))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoUart, rxBuffLen), m_edRxBuff, m_lblRxBuffer))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoUart, txBuffLen), m_edTxBuff, m_lblTxBuffer))
		.linkAdd(EsCheckBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoUart, resetOnRxTmo), m_chkResetOnRxTmo));

	// adjust width to fit longest string inside port names list
	wxSize sze = m_edPortName->GetClientSize();
	sze.x -= wxSystemSettings::GetMetric( wxSYS_HTHUMB_X ) + 2 * wxSystemSettings::GetMetric( wxSYS_EDGE_X );
	EsString::Array strs = EsUtilities::fromArrayString(m_edPortName->GetStrings());
	int delta = wxMax( EsUtilities::longestStringWidthGet(strs, *m_edPortName, GetFont()), sze.x) - sze.x;
	if( 0 < delta )
	{
		sze = GetClientSize();
		sze.x += delta;
		SetClientSize(sze);
		Layout();
	}
	
	// Connect Events
	m_btnResetToDefaults->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoUartConfigPaneWnd::onResetToDefaults, this );
	m_btnRescan->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoUartConfigPaneWnd::onRescan, this );
}

// populate port selector
void EsChannelIoUartConfigPaneWnd::populatePorts()
{
	m_edPortName->Clear();
	// enumerate communication ports on this machine, ignoring busy ones
	m_ports.enumerate(true);
	// fill-in port selector with user-friendly port names, ignoring modems
	for(size_t idx = 0; idx < m_ports.size(); ++idx)
	{
		if( !m_ports[idx].m_bModem )
			m_edPortName->AppendString( m_ports[idx].m_strFriendlyName.c_str() );
	}
}

// reset button click handler
void EsChannelIoUartConfigPaneWnd::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
	m_links.resetControlsToDefault();	
}

void EsChannelIoUartConfigPaneWnd::onRescan(wxCommandEvent& WXUNUSED(evt))
{
	populatePorts();
}
