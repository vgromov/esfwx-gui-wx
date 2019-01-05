#ifndef _stub_io_channel_config_pane_h_
#define _stub_io_channel_config_pane_h_

class EsChannelIoStubConfigPane : public EsReflectedClassConfigPane
{
protected:
	// create pane window via reflected constructor "NEW" only
	EsChannelIoStubConfigPane(wxWindow* parent);

public:
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);
};

#endif // _stub_io_channel_config_pane_h_