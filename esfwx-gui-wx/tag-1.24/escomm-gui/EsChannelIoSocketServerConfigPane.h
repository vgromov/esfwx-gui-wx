#ifndef _Socket_Server_Io_Channel_Config_Pane_h_
#define _Socket_Server_Io_Channel_Config_Pane_h_

class EsChannelIoSocketServerConfigPane : public  EsReflectedClassConfigPane
{
protected:
	// create pane window via reflected constructor "NEW" only
	EsChannelIoSocketServerConfigPane(wxWindow* parent);

public:
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);
  virtual ~EsChannelIoSocketServerConfigPane();

protected:
	// reset button click handler
	void onResetToDefaults(wxCommandEvent& evt);

protected:
	// controls
	wxButton* m_reset;
	wxStaticText* m_lblPort;
	wxSpinCtrl* m_edPort;
	wxStaticText* m_lblTmo;
	wxSpinCtrl* m_edTmo;	
};

#endif // _Socket_Server_Io_Channel_Config_Pane_h_
