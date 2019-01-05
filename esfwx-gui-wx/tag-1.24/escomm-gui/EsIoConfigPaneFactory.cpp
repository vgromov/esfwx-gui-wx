#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsIoConfigPaneFactory.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsIoConfigPaneFactory::EsIoConfigPaneFactory()
{
  paneRegister<EsChannelIoStub, EsChannelIoStubConfigPane>();
  paneRegister<EsChannelIoUart, EsChannelIoUartConfigPane>();
	paneRegister<EsChannelIoEkonnect, EsChannelIoEkonnectConfigPane>();
	paneRegister<EsChannelIoSocketClient, EsChannelIoSocketClientConfigPane>();
	paneRegister<EsChannelIoSocketServer, EsChannelIoSocketServerConfigPane>();
  paneRegister<EsRpcMaster, EsProtocolEcoeConfigPane>();
}
//--------------------------------------------------------------------------------

const EsIoConfigPaneFactory& EsIoConfigPaneFactory::instanceGet()
{
	static const EsIoConfigPaneFactory sc_inst;
	return sc_inst;
}
//--------------------------------------------------------------------------------

EsReflectedObjectConfigPaneIntf::Ptr EsIoConfigPaneFactory::create(const EsString& ioTypeName, wxWindow* parent)
{
  const EsIoConfigPaneFactory& This = instanceGet();

	auto cit = This.m_registry.find(ioTypeName);
  if( cit == This.m_registry.end() )
    EsException::Throw(
      esT("IO Configuration pane class is not registered for %s"),
      ioTypeName
    );

	return (*cit).second(parent);
}
//--------------------------------------------------------------------------------
