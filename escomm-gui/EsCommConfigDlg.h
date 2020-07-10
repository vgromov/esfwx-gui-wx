#ifndef _comm_config_dlg_h_
#define _comm_config_dlg_h_
 
class ESCOMM_GUI_CLASS EsCommConfigDlg :	public wxDialog
{
public:
	EsCommConfigDlg(wxWindow* parent, bool showProtocolPane = false, wxConfigBase* cfg = nullptr, const EsString& path = EsString::null());
	
		// set|get channel object
	inline void channelSet(const EsChannelIoIntf::Ptr& chnl) { m_paneChannel->channelSet(chnl); } 
	inline EsChannelIoIntf::Ptr channelGet() const { return m_paneChannel->channelGet(); }	

	// protocol config access
	//
	EsReflectedObjectConfigPaneIntf::Ptr protocolConfigPaneGet() const ES_NOTHROW { return m_paneProtocol; }
	
protected:
	EsReflectedObjectConfigPaneIntf::Ptr m_paneProtocol;
	EsChannelIoConfigPane* m_paneChannel;
	wxButton* m_btnOK;
	wxButton* m_btnCancel;
};

#endif // _comm_config_dlg_h_
