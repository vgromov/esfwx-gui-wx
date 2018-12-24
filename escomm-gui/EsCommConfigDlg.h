#ifndef _comm_config_dlg_h_
#define _comm_config_dlg_h_
 
class EKOSF_COMMGUI_CLASS EsCommConfigDlg :	public wxDialog
{
public:
	EsCommConfigDlg(wxWindow* parent, bool showProtocolPane = false,
		wxConfigBase* cfg = 0, const EsString& path = EsString::s_null);
	
		// set|get channel object
	inline void channelSet(const EsChannelIoIntf::Ptr& chnl) { m_cfgPane->channelSet(chnl); } 
	inline EsChannelIoIntf::Ptr channelGet() const { return m_cfgPane->channelGet(); }	

	// protocol config access
	//
	EsReflectedObjectConfigPaneIntf::Ptr protocolConfigPaneGet() const { return m_protocolPane; }
	
protected:
	wxButton* m_btnOK;
	wxButton* m_btnCancel;
	EsCommConfigPane* m_cfgPane;
	EsReflectedObjectConfigPaneIntf::Ptr m_protocolPane;
};

#endif // _comm_config_dlg_h_
