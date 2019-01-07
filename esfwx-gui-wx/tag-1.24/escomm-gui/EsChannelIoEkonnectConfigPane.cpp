#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoEkonnectConfigPane.h"

// Resources TODO: move to the application art provider
#include "res/rescan.xpm"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

class EsEkonnectIoDevicePropertyLink : public EsComboBoxPropertyLink
{
protected:
	EsEkonnectIoDevicePropertyLink(
    EsChannelIoEkonnectConfigPane& pane, 
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
	static EsReflectedClassPropertyLink::Ptr create(EsChannelIoEkonnectConfigPane& pane, const EsString& prop, wxComboBox* cbx, wxStaticText* lbl)
	{
		EsReflectedClassPropertyLink::Ptr result(
      new EsEkonnectIoDevicePropertyLink(
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

    const EsVariant::Array& devices = EsChannelIoEkonnect::enumerate(
      EsVariant(false)
    ).asVariantCollection();

    for(auto const& device : devices)
      m_cbx->AppendString(
        device[0].asString().c_str()
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
		      sze = m_pane.paneGet()->GetClientSize();
		      sze.x += delta;
		      m_pane.paneGet()->SetClientSize(sze);
		      m_pane.paneGet()->Layout();
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
    ES_ASSERT(m_cbx);
    int idx = m_cbx->FindString(
      val.asString().c_str()
    );

		m_cbx->SetSelection(idx);
	}
	
	virtual EsVariant doControlValGet() const ES_OVERRIDE
	{
		int sel = m_cbx->GetSelection();
		if( wxNOT_FOUND != sel )
      return EsVariant(
        m_cbx->GetString(sel).wc_str(),
        EsVariant::ACCEPT_STRING
      );

		return EsString::null();
  }

protected:
  EsChannelIoEkonnectConfigPane& m_pane;
};
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChannelIoEkonnectConfigPane::
ConfigPaneWnd::ConfigPaneWnd(EsChannelIoEkonnectConfigPane& pane, wxWindow* parent) :
PaneWnd(
  pane,
  parent
),
m_btnResetToDefaults(nullptr),
m_settings(nullptr),
m_pnlStd(nullptr),
m_lblDevice(nullptr),
m_edDeviceName(nullptr),
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
m_lblRxTmo(nullptr),
m_edRxTmo(nullptr),
m_lblTxTmo(nullptr),
m_edTxTmo(nullptr),
m_lblRxBuffer(nullptr),
m_edRxBuff(nullptr),
m_lblTxBuffer(nullptr),
m_edTxBuff(nullptr),
m_chkResetOnRxTmo(nullptr),
m_chkUseRS232(nullptr)
{
	m_btnResetToDefaults = new wxButton( this, wxID_ANY, _("Reset to defaults") );
	m_layContents->Add( m_btnResetToDefaults, wxSizerFlags().Border() );
	
	m_settings = new wxNotebook( this, wxID_ANY );
	m_pnlStd = new wxPanel( m_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* controlsStd;
	controlsStd = new wxFlexGridSizer( 0, 2, 0, 0 );
	controlsStd->AddGrowableCol( 1 );
	controlsStd->SetFlexibleDirection( wxHORIZONTAL );
	controlsStd->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxSizerFlags lblFlags;
	lblFlags.Border().CenterVertical().Left();

  wxSizerFlags edFlags(1);
  edFlags.Border().Expand();

	m_lblDevice = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString );
	m_lblDevice->Wrap( -1 );
	controlsStd->Add( m_lblDevice, lblFlags );
	
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	m_edDeviceName = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	box->Add(m_edDeviceName, edFlags);
	box->AddSpacer(5);

	m_btnRescan = new wxBitmapButton( m_pnlStd, wxID_ANY, wxBitmap(rescan_xpm, wxBITMAP_TYPE_XPM));
	m_btnRescan->SetToolTip( _("Rescan EKONNECT devices") );
	box->Add(m_btnRescan, wxSizerFlags().CenterVertical());	
	controlsStd->Add( box, edFlags );
	
	m_lblBaud = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString );
	m_lblBaud->Wrap( -1 );
	controlsStd->Add( m_lblBaud, lblFlags );
	
	m_edBaud = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edBaud, edFlags );
	
	m_lblByteSize = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString );
	m_lblByteSize->Wrap( -1 );
	controlsStd->Add( m_lblByteSize, lblFlags );
	
	m_edByteSize = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edByteSize, edFlags );
	
	m_lblParity = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString );
	m_lblParity->Wrap( -1 );
	controlsStd->Add( m_lblParity, lblFlags );
	
	m_edParity = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edParity, edFlags );
	
	m_lblStopBits = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString );
	m_lblStopBits->Wrap( -1 );
	controlsStd->Add( m_lblStopBits, lblFlags );
	
	m_edStopBits = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edStopBits, edFlags );
	
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
	
	m_lblRxTmo = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString );
	m_lblRxTmo->Wrap( -1 );
	ctlsGrid->Add( m_lblRxTmo, lblFlags );
	
	m_edRxTmo = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edRxTmo, edFlags );
	
	m_lblTxTmo = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString );
	m_lblTxTmo->Wrap( -1 );
	ctlsGrid->Add( m_lblTxTmo, lblFlags );
	
	m_edTxTmo = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edTxTmo, edFlags );
	
	m_lblRxBuffer = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString );
	m_lblRxBuffer->Wrap( -1 );
	ctlsGrid->Add( m_lblRxBuffer, lblFlags );
	
	m_edRxBuff = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edRxBuff, edFlags );
	
	m_lblTxBuffer = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString );
	m_lblTxBuffer->Wrap( -1 );
	ctlsGrid->Add( m_lblTxBuffer, lblFlags );
	
	m_edTxBuff = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edTxBuff, edFlags );
	
	controlsAdvanced->Add( ctlsGrid, wxSizerFlags().Border().Expand() );
	
	m_chkResetOnRxTmo = new wxCheckBox( m_pnlAdvanced, wxID_ANY, wxEmptyString );

  wxSizerFlags edFlagsNoProportion = edFlags;
  edFlagsNoProportion.Proportion(0);

	controlsAdvanced->Add( m_chkResetOnRxTmo, edFlagsNoProportion );
	
	m_chkUseRS232 = new wxCheckBox( m_pnlAdvanced, wxID_ANY, wxEmptyString );
	controlsAdvanced->Add( m_chkUseRS232, edFlagsNoProportion );
	
	m_pnlAdvanced->SetSizer( controlsAdvanced );
	m_pnlAdvanced->Layout();
	controlsAdvanced->Fit( m_pnlAdvanced );
	m_settings->AddPage( m_pnlAdvanced, _("Advanced"), false );
	
	m_layContents->Add( m_settings, edFlags );

	// Connect Events
	m_btnResetToDefaults->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &ConfigPaneWnd::onResetToDefaults, this );
	m_btnRescan->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &ConfigPaneWnd::onRescan, this );
}
//--------------------------------------------------------------------------------

EsChannelIoEkonnectConfigPane::
ConfigPaneWnd::~ConfigPaneWnd()
{
  ES_DEBUG_TRACE(esT("EsChannelIoEkonnectConfigPane::~ConfigPaneWnd"));

	// Disconnect Events
	m_btnResetToDefaults->Unbind( 
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this 
  );
	m_btnRescan->Unbind( 
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onRescan, 
    this 
  );
}
//--------------------------------------------------------------------------------

void EsChannelIoEkonnectConfigPane::
ConfigPaneWnd::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
	m_intf.controlsResetToDefault();
}
//--------------------------------------------------------------------------------

void EsChannelIoEkonnectConfigPane::
ConfigPaneWnd::onRescan(wxCommandEvent& WXUNUSED(evt))
{
	EsEkonnectIoDevicePropertyLink* link = m_intf.dataSourceAccess().linkFind<EsEkonnectIoDevicePropertyLink>(
    esT("device")
  );
  ES_ASSERT(link);

  link->itemsPopulate();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsString EsChannelIoEkonnectConfigPane::typeNameGet() const ES_NOTHROW
{
  return esT("EsChannelIoEkonnectConfigPane");
}
//--------------------------------------------------------------------------------

EsReflectedClassConfigPane::PaneWnd* EsChannelIoEkonnectConfigPane::doPaneWndCreate(wxWindow* parent)
{
  PaneWnd* wnd = new ConfigPaneWnd(
    *this, 
    parent
  );
  ES_ASSERT(wnd);

  return wnd;
}
//--------------------------------------------------------------------------------

EsReflectedClassDataSource* EsChannelIoEkonnectConfigPane::doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta)
{
  ConfigPaneWnd* pane = wxDynamicCast( m_pane.get(), ConfigPaneWnd );
  ES_ASSERT(pane);

  std::unique_ptr<EsReflectedClassContainerDataSource> src = ES_MAKE_UNIQUE(
    EsReflectedClassContainerDataSource,
    m_pane,
    className,
    nullptr
  );
  ES_ASSERT(src);

	// set-up control links
	src->link(
    EsEkonnectIoDevicePropertyLink::create(
      *this,
      esT("device"), 
      pane->m_edDeviceName, 
      pane->m_lblDevice
    )
  );
	src->link(
    EsComboBoxPropertyLink::create(
      esT("baud"), 
      pane->m_edBaud, 
      pane->m_lblBaud
    )
  );
	src->link(
    EsComboBoxPropertyLink::create(
      esT("bits"), 
      pane->m_edByteSize, 
      pane->m_lblByteSize
    )
  );	
  src->link(
    EsComboBoxPropertyLink::create(
      esT("parity"), 
      pane->m_edParity, 
      pane->m_lblParity
    )
  );
	src->link(
    EsComboBoxPropertyLink::create(
      esT("stopBits"), 
      pane->m_edStopBits, 
      pane->m_lblStopBits
    )
  );
	src->link(
    EsSpinCtlPropertyLink::create(
      esT("rxTmo"), 
      pane->m_edRxTmo, 
      pane->m_lblRxTmo
    )
  );
	src->link(
    EsSpinCtlPropertyLink::create(
      esT("txTmo"), 
      pane->m_edTxTmo, 
      pane->m_lblTxTmo
    )
  );
	src->link(
    EsSpinCtlPropertyLink::create(
      esT("rxBuffLen"), 
      pane->m_edRxBuff, 
      pane->m_lblRxBuffer
    )
  );
	src->link(
    EsSpinCtlPropertyLink::create(
      esT("txBuffLen"), 
      pane->m_edTxBuff, 
      pane->m_lblTxBuffer
    )
  );
	src->link(
    EsCheckBoxPropertyLink::create(
      esT("resetOnRxTmo"), 
      pane->m_chkResetOnRxTmo
    )
  );
	src->link(
    EsCheckBoxPropertyLink::create(
      esT("useRS232"), 
      pane->m_chkUseRS232
    )
  );

  return src.release();
}
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoEkonnectConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoEkonnectConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoEkonnectConfigPane );
  ES_ASSERT(ptr);

  ptr->init(
    parent,
    EsChannelIoEkonnect::classNameGetStatic(),
    nullptr
  );

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

EsChannelIoEkonnectConfigPane::~EsChannelIoEkonnectConfigPane()
{
  ES_DEBUG_TRACE(esT("EsChannelIoEkonnectConfigPane::~EsChannelIoEkonnectConfigPane"));
}
//--------------------------------------------------------------------------------
