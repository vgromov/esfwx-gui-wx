#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoSocketServerConfigPane.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChannelIoSocketServerConfigPane::
ConfigPaneWnd::ConfigPaneWnd(EsChannelIoSocketServerConfigPane& pane, wxWindow* parent) :
PaneWnd(
  pane,
  parent
),
m_reset(nullptr),
m_lblPort(nullptr),
m_edPort(nullptr),
m_lblTmo(nullptr),
m_edTmo(nullptr)
{
 m_reset = new wxButton(this, wxID_ANY, _("Reset to defaults"));
  ES_ASSERT(m_reset);
	m_layContents->Add(
    m_reset, 
    wxSizerFlags().Border()
  );

	wxFlexGridSizer* ctls = new wxFlexGridSizer(2, 2, 0, 0);
	wxASSERT(ctls);
	ctls->AddGrowableCol(1, 1);
	wxSizerFlags lblFlags;
	lblFlags.Border().CenterVertical().Left();

  m_lblPort = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblPort);
  
  m_edPort = new wxSpinCtrl(this, wxID_ANY);
  ES_ASSERT(m_edPort);

	ctls->Add(m_lblPort, lblFlags);
	ctls->Add(m_edPort, wxSizerFlags(1).Border().Expand());

  m_lblTmo = new wxStaticText(this, wxID_ANY, wxEmptyString);
  ES_ASSERT(m_lblTmo);

  m_edTmo = new wxSpinCtrl(this, wxID_ANY);
  ES_ASSERT(m_edTmo);

	ctls->Add(m_lblTmo, lblFlags);
	ctls->Add(m_edTmo, wxSizerFlags(1).Border().Expand());

	m_layContents->Add(ctls, wxSizerFlags(1).Border(wxALL, 0).Expand());

	m_reset->Bind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );
}
//--------------------------------------------------------------------------------

EsChannelIoSocketServerConfigPane::
ConfigPaneWnd::~ConfigPaneWnd()
{
	m_reset->Unbind(
    wxEVT_COMMAND_BUTTON_CLICKED, 
    &ConfigPaneWnd::onResetToDefaults, 
    this
  );

  ES_DEBUG_TRACE(
    esT("EsChannelIoSocketServerConfigPane::~ConfigPaneWnd")
  );
}
//--------------------------------------------------------------------------------

void EsChannelIoSocketServerConfigPane::
ConfigPaneWnd::onResetToDefaults(wxCommandEvent& evt)
{
	m_intf.controlsResetToDefault();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoSocketServerConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoSocketServerConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoSocketServerConfigPane );
  ES_ASSERT(ptr);

  ptr->init(
    parent,
    EsChannelIoSocketServer::classNameGetStatic(),
    nullptr
  );

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

EsChannelIoSocketServerConfigPane::~EsChannelIoSocketServerConfigPane()
{
  ES_DEBUG_TRACE(esT("EsChannelIoSocketServerConfigPane::~EsChannelIoSocketServerConfigPane"));
}
//--------------------------------------------------------------------------------

EsString EsChannelIoSocketServerConfigPane::typeNameGet() const ES_NOTHROW
{
  return esT("EsChannelIoSocketServerConfigPane");
}
//--------------------------------------------------------------------------------

EsReflectedClassConfigPane::PaneWnd* EsChannelIoSocketServerConfigPane::doPaneWndCreate(wxWindow* parent)
{
  PaneWnd* wnd = new ConfigPaneWnd(
    *this, 
    parent
  );
  ES_ASSERT(wnd);

  return wnd;
}
//--------------------------------------------------------------------------------

EsReflectedClassDataSource* EsChannelIoSocketServerConfigPane::doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta)
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
    EsSpinCtlPropertyLink::create(
      esT("port"), 
      pane->m_edPort, 
      pane->m_lblPort
    )
  );
	src->link(
    EsSpinCtlPropertyLink::create(
      esT("operationTimeout"), 
      pane->m_edTmo, 
      pane->m_lblTmo
    )
  );

  return src.release();
}
//--------------------------------------------------------------------------------
