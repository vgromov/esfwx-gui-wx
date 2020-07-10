#include "escomm_gui_pch.h"
#pragma hdrstop

#include "EsIoConfigPaneFactory.h"
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsIoConfigPaneFactory::EsIoConfigPaneFactory()
{
#ifdef ES_COMM_USE_CHANNEL_STUB
  paneRegister<EsChannelIoStub, EsChannelIoStubConfigPane>();
#endif
#ifdef ES_COMM_USE_CHANNEL_UART
  paneRegister<EsChannelIoUart, EsChannelIoUartConfigPane>();
#endif
#ifdef ES_COMM_USE_CHANNEL_EKONNECT
	paneRegister<EsChannelIoEkonnect, EsChannelIoEkonnectConfigPane>();
#endif
#ifdef ES_COMM_USE_CHANNEL_IO_SOCKET
	paneRegister<EsChannelIoSocketClient, EsChannelIoSocketClientConfigPane>();
	paneRegister<EsChannelIoSocketServer, EsChannelIoSocketServerConfigPane>();
#endif
#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH
	paneRegister<EsChannelIoBluetoothClient, EsChannelIoBluetoothClientConfigPane>();
#endif
#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE
	paneRegister<EsChannelIoBluetoothLE, EsChannelIoBluetoothLeConfigPane>();
#endif
#ifdef ES_COMM_USE_RPC
  paneRegister<EsRpcMaster, EsProtocolEcoeConfigPane>();
#endif
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
