#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsChannelIoStubConfigPane.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChannelIoStubConfigPane::EsChannelIoStubConfigPane(wxWindow* parent) :
EsReflectedClassConfigPane(
  parent,
  EsChannelIoStub::classNameGetStatic()
)
{
	m_layContents->Add( 
    new wxStaticText(
      this, 
      wxID_ANY, 
      _("Stub Io channel does not have any adjustable settings")
    ),
		wxSizerFlags(1).Align(wxALIGN_CENTER)
  );
	
	Layout();	
}
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoStubConfigPane::create(wxWindow* parent)
{
  std::unique_ptr<EsChannelIoStubConfigPane> ptr = ES_MAKE_UNIQUE( EsChannelIoStubConfigPane, parent );
  ES_ASSERT(ptr);

  return ptr.release()->intfGet();
}
//--------------------------------------------------------------------------------