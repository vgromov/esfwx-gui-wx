#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoBluetoothClientConfigPane.h"
//--------------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH
//--------------------------------------------------------------------------------

EsChannelIoBluetoothClientConfigPane::
ConfigPaneWnd::ConfigPaneWnd(EsChannelIoBluetoothClientConfigPane& pane, wxWindow* parent) :
PaneWnd(
  pane,
  parent
),
m_reset(nullptr),
m_edAddr(nullptr),
m_lblAddr(nullptr),
m_edName(nullptr),
m_lblName(nullptr),
m_edSvcClass(nullptr),
m_lblSvcClass(nullptr),
m_edSvcName(nullptr),
m_lblSvcName(nullptr)
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
	
  m_edName = new wxTextCtrl(this, wxID_ANY);
  ES_ASSERT(m_edName);

  m_lblName = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblName);
  
  ctls->Add(m_lblName, lblFlags);
	ctls->Add(m_edName, wxSizerFlags(1).Border().Expand());

  m_edSvcClass = new wxTextCtrl(this, wxID_ANY);
  ES_ASSERT(m_edSvcClass);

  m_lblSvcClass = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblSvcClass);

	ctls->Add(m_lblSvcClass, lblFlags);
	ctls->Add(m_edSvcClass, wxSizerFlags(1).Border().Expand());

  m_edSvcName = new wxTextCtrl(this, wxID_ANY);
  ES_ASSERT(m_edSvcName);

  m_lblSvcName = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblSvcName);

	ctls->Add(m_lblSvcName, lblFlags);
	ctls->Add(m_edSvcName, wxSizerFlags(1).Border().Expand());

  m_layContents->Add(ctls, wxSizerFlags(1).Border(wxALL, 0).Expand());

	m_reset->Bind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );
}
//--------------------------------------------------------------------------------

EsChannelIoBluetoothClientConfigPane::
ConfigPaneWnd::~ConfigPaneWnd()
{
	m_reset->Unbind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );

  ES_DEBUG_TRACE(
    esT("EsChannelIoBluetoothClientConfigPane::~ConfigPaneWnd")
  );
}
//--------------------------------------------------------------------------------

void EsChannelIoBluetoothClientConfigPane::
ConfigPaneWnd::onResetToDefaults(wxCommandEvent& evt)
{
	m_intf.controlsResetToDefault();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoBluetoothClientConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoBluetoothClientConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoBluetoothClientConfigPane );
  ES_ASSERT(ptr);

  ptr->init(
    parent,
    EsChannelIoBluetoothClient::classNameGetStatic(),
    nullptr
  );

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

EsChannelIoBluetoothClientConfigPane::~EsChannelIoBluetoothClientConfigPane()
{
  ES_DEBUG_TRACE(esT("EsChannelIoBluetoothClientConfigPane::~EsChannelIoBluetoothClientConfigPane"));
}
//--------------------------------------------------------------------------------

EsString EsChannelIoBluetoothClientConfigPane::typeNameGet() const ES_NOTHROW
{
  return classNameGetStatic();
}
//--------------------------------------------------------------------------------

EsReflectedClassConfigPane::PaneWnd* EsChannelIoBluetoothClientConfigPane::doPaneWndCreate(wxWindow* parent)
{
  PaneWnd* wnd = new ConfigPaneWnd(
    *this, 
    parent
  );
  ES_ASSERT(wnd);

  return wnd;
}
//--------------------------------------------------------------------------------

EsReflectedClassDataSource* EsChannelIoBluetoothClientConfigPane::doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta)
{
  ConfigPaneWnd* pane = dynamic_cast<ConfigPaneWnd*>( m_pane.get() );
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
    EsTextCtlPropertyLink::create(
      esT("deviceName"), 
      pane->m_edName, 
      pane->m_lblName
    )
  );
	src->link(
    EsTextCtlPropertyLink::create(
      esT("serviceClass"), 
      pane->m_edSvcClass, 
      pane->m_lblSvcClass
    )
  );
	src->link(
    EsTextCtlPropertyLink::create(
      esT("serviceName"), 
      pane->m_edSvcName, 
      pane->m_lblSvcName
    )
  );

  return src.release();
}
//--------------------------------------------------------------------------------

#endif //< ES_COMM_USE_CHANNEL_BLUETOOTH
