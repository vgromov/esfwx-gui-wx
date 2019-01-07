#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoStubConfigPane.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsString EsChannelIoStubConfigPane::typeNameGet() const ES_NOTHROW
{
  return esT("EsChannelIoStubConfigPane");
}
//--------------------------------------------------------------------------------

EsReflectedClassConfigPane::PaneWnd* EsChannelIoStubConfigPane::doPaneWndCreate(wxWindow* parent)
{
  PaneWnd* wnd = new PaneWnd(*this, parent);
  ES_ASSERT(wnd);

	wnd->contentsGet()->Add( 
    new wxStaticText(
      wnd, 
      wxID_ANY, 
      _("Stub Io channel does not have any adjustable settings"),
      wxDefaultPosition,
      wxSize(
        300,
        60
      ),
      wxALIGN_CENTRE_HORIZONTAL |
      wxST_NO_AUTORESIZE
    ),
		wxSizerFlags(1).Border().Expand().FixedMinSize()
  );

  return wnd;
}
//--------------------------------------------------------------------------------

EsReflectedClassDataSource* EsChannelIoStubConfigPane::doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& ES_UNUSED(meta))
{
  std::unique_ptr<EsReflectedClassContainerDataSource> src = ES_MAKE_UNIQUE(
    EsReflectedClassContainerDataSource,
    m_pane,
    className,
    nullptr
  );
  ES_ASSERT(src);

  return src.release();
}
//--------------------------------------------------------------------------------

EsChannelIoStubConfigPane::~EsChannelIoStubConfigPane()
{
  ES_DEBUG_TRACE(esT("EsChannelIoStubConfigPane::~EsChannelIoStubConfigPane"));
}
//--------------------------------------------------------------------------------
  
EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoStubConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoStubConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoStubConfigPane );
  ES_ASSERT(ptr);

  ptr->init(
    parent,
    EsChannelIoStub::classNameGetStatic(),
    nullptr
  );

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------