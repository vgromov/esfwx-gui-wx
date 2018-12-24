#include <stdafx.h>
#pragma hdrstop

#include <wx/spinctrl.h>
#include "EsChannelIoEkonnectConfigPane.h"
#include "res/rescan.xpm"

// custom property link for com port enumeration
//
// combo box link
class EsEkonnectIoDevicePropertyLink : public EsPropertyLink
{
protected:
	EsEkonnectIoDevicePropertyLink(const EsPropertyInfo& propInfo,
		wxComboBox* cbx, wxStaticText* lbl, const EsFtdiDriver::DeviceList& devices) :
	EsPropertyLink(propInfo),
	m_cbx(0),
	m_lbl(0),
	m_devices(devices)
	{
		init(cbx, lbl);
	}

public:
	static EsPropertyLink::Ptr create(const EsPropertyInfo& propInfo,
		wxComboBox* cbx, wxStaticText* lbl, const EsFtdiDriver::DeviceList& devices)
	{
		EsPropertyLink::Ptr result(new EsEkonnectIoDevicePropertyLink(propInfo, cbx, lbl, devices));
		result->resetControlToDefault();
		return result;
	}
	
	virtual void updateControl(const EsReflectedClassIntf::Ptr& obj)
	{
		wxASSERT(obj);
		wxASSERT(obj->is(m_propInfo.getClassInfo().nameGet()));
		
		EsString devSerial = m_propInfo.get(obj.request<EsBaseIntf>().get()).asString();
		for(size_t idx = 0; idx < m_devices.size(); ++idx)
			if( m_devices[idx].getSerialNumberStr() == devSerial )
			{
				m_cbx->SetSelection( idx );
				return;
			}
		
		m_cbx->SetSelection(-1);
	}
	
	virtual void updateProperty(const EsReflectedClassIntf::Ptr& obj) const
	{
		wxASSERT(obj);
		wxASSERT(obj->is(m_propInfo.getClassInfo().nameGet()));
		int sel = m_cbx->GetSelection();
		if( -1 < sel )
			m_propInfo.set(obj.request<EsBaseIntf>().get(), m_devices[sel].getSerialNumberStr());	
	}
	
	virtual void resetControlToDefault()
	{
		// set default port
		if( !m_devices.empty() )
		{
			EsString devSerial = m_propInfo.defaultGet().asString();			
			for(size_t idx = 0; idx < m_devices.size(); ++idx)
				if( m_devices[idx].getSerialNumberStr() == devSerial )
				{
					m_cbx->SetSelection( idx );
					return;
				}

				m_cbx->SetSelection(-1);
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
	const EsFtdiDriver::DeviceList& m_devices;
};

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(EsChannelIoEkonnectConfigPaneWnd::EsChannelIoEkonnectConfigPane, EsChannelIoEkonnectConfigPane, _i("UART channel configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(EsChannelIoEkonnectConfigPaneWnd::EsChannelIoEkonnectConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, _i("EsChannelIoEkonnectConfigPane constructor."))
ES_DECL_CLASS_INFO_END

EsChannelIoEkonnectConfigPaneWnd::EsChannelIoEkonnectConfigPane::~EsChannelIoEkonnectConfigPane() {}

EsString EsChannelIoEkonnectConfigPaneWnd::EsChannelIoEkonnectConfigPane::getObjectName() const 
{ 
	return EsChannelIoEkonnect::getClassInfoStatic().nameGet(); 
}

// configuration pane itself
//
EsChannelIoEkonnectConfigPaneWnd::EsChannelIoEkonnectConfigPaneWnd(wxWindow* parent) :
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

	m_lblDevice = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblDevice->Wrap( -1 );
	controlsStd->Add( m_lblDevice, lblFlags );
	
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	m_edDeviceName = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	box->Add(m_edDeviceName, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL);
	box->AddSpacer(5);
	m_btnRescan = new wxBitmapButton( m_pnlStd, wxID_ANY, wxBitmap(rescan_xpm, wxBITMAP_TYPE_XPM));
	m_btnRescan->SetToolTip( _("Rescan EKONNECT devices") );
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
	
	m_lblRxTmo = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblRxTmo->Wrap( -1 );
	ctlsGrid->Add( m_lblRxTmo, lblFlags );
	
	m_edRxTmo = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edRxTmo, 1, wxALL|wxEXPAND, 5 );
	
	m_lblTxTmo = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblTxTmo->Wrap( -1 );
	ctlsGrid->Add( m_lblTxTmo, lblFlags );
	
	m_edTxTmo = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edTxTmo, 1, wxALL|wxEXPAND, 5 );
	
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
	
	m_chkUseRS232 = new wxCheckBox( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	controlsAdvanced->Add( m_chkUseRS232, 0, wxALL|wxEXPAND, 5 );
	
	m_pnlAdvanced->SetSizer( controlsAdvanced );
	m_pnlAdvanced->Layout();
	controlsAdvanced->Fit( m_pnlAdvanced );
	m_settings->AddPage( m_pnlAdvanced, _("Advanced"), false );
	
	contents->Add( m_settings, 1, wxEXPAND | wxALL, 5 );
	
	SetSizer( contents );
	Layout();
	
	// populate device list
	populateDevices();

	// set-up control links
	m_links
		.linkAdd(EsEkonnectIoDevicePropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, device), m_edDeviceName, m_lblDevice, m_devices))
		.linkAdd(EsComboBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, baud), m_edBaud, m_lblBaud))
		.linkAdd(EsComboBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, bits), m_edByteSize, m_lblByteSize))
		.linkAdd(EsComboBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, parity), m_edParity, m_lblParity))
		.linkAdd(EsComboBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, stopBits), m_edStopBits, m_lblStopBits))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, rxTmo), m_edRxTmo, m_lblRxTmo))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, txTmo), m_edTxTmo, m_lblTxTmo))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, rxBuffLen), m_edRxBuff, m_lblRxBuffer))
		.linkAdd(EsSpinCtlPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, txBuffLen), m_edTxBuff, m_lblTxBuffer))
		.linkAdd(EsCheckBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, resetOnRxTmo), m_chkResetOnRxTmo))
		.linkAdd(EsCheckBoxPropertyLink::create(ES_REFLECTED_PROP_INFO_GET(EsChannelIoEkonnect, useRS232), m_chkUseRS232));

	// adjust width to fit longest string inside port names list
	wxSize sze = m_edDeviceName->GetClientSize();
	sze.x -= wxSystemSettings::GetMetric( wxSYS_HTHUMB_X ) + 2 * wxSystemSettings::GetMetric( wxSYS_EDGE_X );
	EsString::Array strs = EsUtilities::fromArrayString(m_edDeviceName->GetStrings());
	int delta = wxMax( EsUtilities::longestStringWidthGet(strs, *m_edDeviceName, GetFont()), sze.x) - sze.x;
	if( 0 < delta )
	{
		sze = GetClientSize();
		sze.x += delta;
		SetClientSize(sze);
		Layout();
	}
	
	// Connect Events
	m_btnResetToDefaults->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoEkonnectConfigPaneWnd::onResetToDefaults, this );
	m_btnRescan->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoEkonnectConfigPaneWnd::onRescan, this );
}

// populate port selector
void EsChannelIoEkonnectConfigPaneWnd::populateDevices()
{
	m_edDeviceName->Clear();
	// enumerate Ekonnect communication devices on this machine
	m_devices = EsChannelIoEkonnect::getDeviceList();
	// fill-in device selector with user-friendly device names
	for(size_t idx = 0; idx < m_devices.size(); ++idx)
		m_edDeviceName->AppendString( m_devices[idx].getSerialNumberStr() );
}

// reset button click handler
void EsChannelIoEkonnectConfigPaneWnd::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
	m_links.resetControlsToDefault();
}

// rescan button click
void EsChannelIoEkonnectConfigPaneWnd::onRescan(wxCommandEvent& WXUNUSED(evt))
{
	populateDevices();
}
