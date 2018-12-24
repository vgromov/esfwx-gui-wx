#include "stdafx.h"
#pragma hdrstop

#include "EsChannelIoConfigPaneFactory.h"

EsChannelIoConfigPaneFactory::EsChannelIoConfigPaneFactory()
{
#define REGISTER_PANE_FOR_CHANNEL(ChannelType) m_registry.itemAdd( wxT(#ChannelType), wxT(#ChannelType) wxT("ConfigPane") )
	
	REGISTER_PANE_FOR_CHANNEL(EsChannelIoStub);
	REGISTER_PANE_FOR_CHANNEL(EsChannelIoUart);
	REGISTER_PANE_FOR_CHANNEL(EsChannelIoEkonnect);
	REGISTER_PANE_FOR_CHANNEL(EsChannelIoSocketClient);
	REGISTER_PANE_FOR_CHANNEL(EsChannelIoSocketServer);
}

const EsChannelIoConfigPaneFactory& EsChannelIoConfigPaneFactory::instanceGet()
{
	static const EsChannelIoConfigPaneFactory sc_inst;
	return sc_inst;
}

EsReflectedObjectConfigPaneIntf::Ptr EsChannelIoConfigPaneFactory::create(const EsString& channelTypeName, wxWindow* parent) const
{
	EsReflectedObjectConfigPaneIntf::Ptr result;
	const EsString& paneClassName = m_registry.valueGet(channelTypeName).asString();
	const EsClassInfo* info = EsClassInfo::getClassInfo(paneClassName);
	if(info)
	{
		// explicit variant construction required for wxObject*
		EsVariant wxv(reinterpret_cast<wxObject*>(parent));
		result = info->classCall(c_reflectedCtrName, wxv).asExistingObject();
	}

	return result;
}
