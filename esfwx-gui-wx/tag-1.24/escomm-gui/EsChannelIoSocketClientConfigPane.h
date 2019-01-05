#ifndef _socket_client_io_channel_config_pane_
#define _socket_client_io_channel_config_pane_

class EsChannelIoSocketClientConfigPane : public EsReflectedClassConfigPane
{
protected:
	// create pane window via reflected constructor "NEW" only
	EsChannelIoSocketClientConfigPane(wxWindow* parent);

public:
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);
	virtual ~EsChannelIoSocketClientConfigPane();

protected:
	// set default values
	void setDefaults();
	// reset button click handler
	void onResetToDefaults(wxCommandEvent& evt);

protected:
	// controls
	wxButton* m_reset;
	wxTextCtrl* m_edAddr;
	wxStaticText* m_lblAddr;
	wxSpinCtrl* m_edPort;
	wxStaticText* m_lblPort;
	wxSpinCtrl* m_edTmo;
	wxStaticText* m_lblTmo;
};

#endif // _socket_client_io_channel_config_pane_
