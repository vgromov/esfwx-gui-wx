#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoBluetoothLeConfigPane.h"
//--------------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE
//--------------------------------------------------------------------------------

EsChannelIoBluetoothLeConfigPane::
ConfigPaneWnd::ConfigPaneWnd(EsChannelIoBluetoothLeConfigPane& pane, wxWindow* parent) :
PaneWnd(
  pane,
  parent
),
m_reset(nullptr),
m_edAddr(nullptr),
m_lblAddr(nullptr),
m_edSubscribe(nullptr)
{
  m_reset = new wxButton(this, wxID_ANY, _("Reset to defaults"));
  ES_ASSERT(m_reset);

	m_layContents->Add(
    m_reset, 
    wxSizerFlags().Border()
  );

	wxFlexGridSizer* ctls = new wxFlexGridSizer(3, 2, 0, 0);
	wxASSERT(ctls);
	ctls->AddGrowableCol(1, 1);
	wxSizerFlags lblFlags;
	lblFlags.Border().CenterVertical().Left();

  m_edAddr = new wxTextCtrl(this, wxID_ANY);
  ES_ASSERT(m_edAddr);

  m_lblAddr = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblAddr);

	ctls->Add(m_lblAddr, lblFlags);
	ctls->Add(m_edAddr, wxSizerFlags(1).Border().Expand());
	
  m_edSubscribe = new wxCheckBox(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_edSubscribe);
	ctls->Add(m_edSubscribe, wxSizerFlags(1).Border().Expand());

  m_layContents->Add(ctls, wxSizerFlags(1).Border(wxALL, 0).Expand());

	m_reset->Bind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );
}
//--------------------------------------------------------------------------------

EsChannelIoBluetoothLeConfigPane::
ConfigPaneWnd::~ConfigPaneWnd()
{
	m_reset->Unbind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );

  ES_DEBUG_TRACE(
    esT("EsChannelIoBluetoothLeConfigPane::~ConfigPaneWnd")
  );
}
//--------------------------------------------------------------------------------

void EsChannelIoBluetoothLeConfigPane::
ConfigPaneWnd::onResetToDefaults(wxCommandEvent& evt)
{
	m_intf.controlsResetToDefault();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoBluetoothLeConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoBluetoothLeConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoBluetoothLeConfigPane );
  ES_ASSERT(ptr);

  ptr->init(
    parent,
    EsChannelIoBluetoothLE::classNameGetStatic(),
    nullptr
  );

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

EsChannelIoBluetoothLeConfigPane::~EsChannelIoBluetoothLeConfigPane()
{
  ES_DEBUG_TRACE(esT("EsChannelIoBluetoothLeConfigPane::~EsChannelIoBluetoothLeConfigPane"));
}
//--------------------------------------------------------------------------------

EsString EsChannelIoBluetoothLeConfigPane::typeNameGet() const ES_NOTHROW
{
  return classNameGetStatic();
}
//--------------------------------------------------------------------------------

EsReflectedClassConfigPane::PaneWnd* EsChannelIoBluetoothLeConfigPane::doPaneWndCreate(wxWindow* parent)
{
  PaneWnd* wnd = new ConfigPaneWnd(
    *this, 
    parent
  );
  ES_ASSERT(wnd);

  return wnd;
}
//--------------------------------------------------------------------------------

EsReflectedClassDataSource* EsChannelIoBluetoothLeConfigPane::doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta)
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

	// initialize property links
	src->link(
    EsTextCtlPropertyLink::create(
      esT("deviceAddress"), 
      pane->m_edAddr, 
      pane->m_lblAddr
    )
  );
	src->link(
    EsCheckBoxPropertyLink::create(
      esT("subscribeToNotifications"), 
      pane->m_edSubscribe
    )
  );

  return src.release();
}
//--------------------------------------------------------------------------------

#endif //< ES_COMM_USE_CHANNEL_BLUETOOTH_LE
