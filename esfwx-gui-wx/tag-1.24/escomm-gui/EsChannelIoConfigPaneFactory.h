#ifndef _es_channel_io_config_pane_factory_h_
#define _es_channel_io_config_pane_factory_h_

// channel io config panes factory singleton
class EsChannelIoConfigPaneFactory
{
public:
	EsReflectedObjectConfigPaneIntf::Ptr create(const EsString& channelTypeName, wxWindow* parent) const;
	static const EsChannelIoConfigPaneFactory& instanceGet();

protected:
	// use pane class name as a payload
	EsStringIndexedMap m_registry;

private:
	EsChannelIoConfigPaneFactory();
	EsChannelIoConfigPaneFactory(const EsChannelIoConfigPaneFactory&);
	EsChannelIoConfigPaneFactory& operator=(const EsChannelIoConfigPaneFactory&);
};

#endif // _es_channel_io_config_pane_factory_h_