#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoUartConfigPane.h"

// TODO: move resource to the art provider
#include "res/rescan.xpm"

class EsUartIoPortPropertyLink : public EsComboBoxPropertyLink
{
protected:
	EsUartIoPortPropertyLink(
	  EsChannelIoUartConfigPane& pane,    
    const EsString& prop,
		wxComboBox* cbx, 
    wxStaticText* lbl
  ) :
	EsComboBoxPropertyLink(
    prop,
    cbx,
    lbl,
    false
  ),
  m_pane(pane)
	{}

public:
	static EsReflectedClassPropertyLink::Ptr create(	  
    EsChannelIoUartConfigPane& pane,    
    const EsString& prop,
		wxComboBox* cbx, 
    wxStaticText* lbl
  )
	{
		EsReflectedClassPropertyLink::Ptr result(
      new EsUartIoPortPropertyLink(
        pane,
        prop, 
        cbx, 
        lbl
      )
    );

		return result;
	}
	
  virtual void itemsPopulate() ES_OVERRIDE
  {
    ES_ASSERT(m_cbx);
    wxWindowUpdateLocker lock(m_cbx);
    
    int sel = m_cbx->GetSelection(); //< Save selection
    m_cbx->Clear();

    m_devices = EsChannelIoUart::enumerate(
      EsVariant(false), //< Do not include busy ports
      EsVariant(false)  //< Do not restrict ourselves to USB-only devices
    ).asVariantCollection();

    for(auto const& device : m_devices)
      m_cbx->AppendString(
        device[2].asString().c_str() //< Device human-readable string
      );
   
    if( wxNOT_FOUND != sel )
    {
      if( 0 != m_cbx->GetCount() )
      {
        if( sel >= static_cast<int>(m_cbx->GetCount()) )
          sel = m_cbx->GetCount()-1;

	      // adjust width to fit longest string inside port names list
	      wxSize sze = m_cbx->GetClientSize();
	      sze.x -= wxSystemSettings::GetMetric( wxSYS_HTHUMB_X ) + 2 * wxSystemSettings::GetMetric( wxSYS_EDGE_X );
	      int delta = wxMax( 
          EsUtilities::longestStringWidthGet(
            m_cbx->GetStrings(), 
            *m_cbx, 
            m_cbx->GetFont()
          ), 
          sze.x
        ) - sze.x;
	      
        if( 0 < delta )
	      {
		      sze = m_pane.GetClientSize();
		      sze.x += delta;
		      m_pane.SetClientSize(sze);
		      m_pane.Layout();
	      }
      }
      else
        sel = wxNOT_FOUND;
    }

    m_cbx->SetSelection(sel); //< Restore selection, if any
  }

protected:
	virtual void doControlValSet(const EsVariant& val) ES_OVERRIDE
	{
    int sel = wxNOT_FOUND;
    for(size_t idx = 0; idx < m_devices.size(); ++idx)
    {
      const EsVariant& device = m_devices[idx];
      if( device[0] == val )
      {
        sel = idx;
        break;
      }
    }

    ES_ASSERT(m_cbx);
		m_cbx->SetSelection(sel);
	}
	
	virtual EsVariant doControlValGet() const ES_OVERRIDE
	{
    ES_ASSERT(m_cbx);
    int sel = m_cbx->GetSelection();

    if( sel != wxNOT_FOUND && sel < static_cast<int>(m_devices.size()) )
      return m_devices[sel][0];

		return EsString::null();
  }

protected:
  EsChannelIoUartConfigPane& m_pane;
  EsVariant::Array m_devices;
};
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChannelIoUartConfigPane::EsChannelIoUartConfigPane(wxWindow* parent) :
EsReflectedClassConfigPane(
  parent,
  EsChannelIoEkonnect::classNameGetStatic()
),
m_btnResetToDefaults(nullptr),
m_settings(nullptr),
m_pnlStd(nullptr),
m_lblPort(nullptr),
m_edPortName(nullptr),
m_btnRescan(nullptr),
m_lblBaud(nullptr),
m_edBaud(nullptr),
m_lblByteSize(nullptr),
m_edByteSize(nullptr),
m_lblParity(nullptr),
m_edParity(nullptr),
m_lblStopBits(nullptr),
m_edStopBits(nullptr),
m_pnlAdvanced(nullptr),
m_lblRxBuffer(nullptr),
m_edRxBuff(nullptr),
m_lblTxBuffer(nullptr),
m_edTxBuff(nullptr),
m_chkResetOnRxTmo(nullptr)
{
	m_btnResetToDefaults = new wxButton( this, wxID_ANY, _("Reset to defaults"), wxDefaultPosition, wxDefaultSize, 0 );
	m_layContents->Add( m_btnResetToDefaults, 0, wxALL, 5 );
	
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
	
	m_layContents->Add( m_settings, 1, wxEXPAND | wxALL, 5 );
	Layout();	
	
	// set-up property links
	m_src.link(
    EsUartIoPortPropertyLink::create(
      *this,
      esT("port"), 
      m_edPortName, 
      m_lblPort      
    )
  );
	m_src.link(
    EsComboBoxPropertyLink::create(
      esT("baud"), 
      m_edBaud, 
      m_lblBaud
    )
  );
	m_src.link(
    EsComboBoxPropertyLink::create(
      esT("bits"), 
      m_edByteSize, 
      m_lblByteSize
    )
  );
	m_src.link(
    EsComboBoxPropertyLink::create(
      esT("parity"), 
      m_edParity, 
      m_lblParity
    )
  );
	m_src.link(
    EsComboBoxPropertyLink::create(
      esT("stopBits"), 
      m_edStopBits, 
      m_lblStopBits
    )
  );
	m_src.link(
    EsSpinCtlPropertyLink::create(
      esT("rxBuffLen"), 
      m_edRxBuff, 
      m_lblRxBuffer
    )
  );
	m_src.link(
    EsSpinCtlPropertyLink::create(
      esT("txBuffLen"), 
      m_edTxBuff, 
      m_lblTxBuffer
    )
  );
	m_src.link(
    EsCheckBoxPropertyLink::create(
      esT("resetOnRxTmo"), 
      m_chkResetOnRxTmo
    )
  );

	// Connect Events
	m_btnResetToDefaults->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoUartConfigPane::onResetToDefaults, this );
	m_btnRescan->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoUartConfigPane::onRescan, this );
}
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoUartConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoUartConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoUartConfigPane, parent );
  ES_ASSERT(ptr);

  return ptr.release()->intfGet();
}
//--------------------------------------------------------------------------------

EsChannelIoUartConfigPane::~EsChannelIoUartConfigPane()
{
  ES_DEBUG_TRACE(
    esT("EsChannelIoUartConfigPane::~EsChannelIoUartConfigPane")
  );

	// Disconnect Events
	m_btnResetToDefaults->Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoUartConfigPane::onResetToDefaults, this );
	m_btnRescan->Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &EsChannelIoUartConfigPane::onRescan, this );
}
//--------------------------------------------------------------------------------

void EsChannelIoUartConfigPane::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
	m_src.resetControlsToDefault();	
}
//--------------------------------------------------------------------------------

void EsChannelIoUartConfigPane::onRescan(wxCommandEvent& WXUNUSED(evt))
{
	EsUartIoPortPropertyLink* link = m_src.linkFind<EsUartIoPortPropertyLink>(esT("port"));
  ES_ASSERT(link);

  link->itemsPopulate();
}
//--------------------------------------------------------------------------------
