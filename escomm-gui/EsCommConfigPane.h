#ifndef _comm_config_pane_h_
#define _comm_config_pane_h_

// embeddable channel configuration GUI pane
//
class EKOSF_COMMGUI_CLASS EsCommConfigPane : public wxPanel
{
public:
	// optional parameters - cfg, path, key may point to the config entry,
	// which is used when specific channel is selected, to update 
	// property values from persistent storage profiles saved for specific channel name
	EsCommConfigPane(wxWindow* parent, 
		wxConfigBase* cfg = 0, const EsString& path = EsString::s_null);
	virtual ~EsCommConfigPane();

	// set|get channel object
	void channelSet(const EsReflectedClassIntf::Ptr& chnl);
	EsReflectedClassIntf::Ptr channelGet() const;

protected:
	// event handling
	void onChannelSelected( wxCommandEvent& evt );
	// helpers
	void currentPaneReset(const EsString& chnlTypeName);
	void currentPaneCreate(const EsString& chnlTypeName);
	void channelSelect(const EsString& chnlTypeName);
	void topLevelFit();

protected:
	// data members
	wxConfigBase* m_cfg;
	EsString m_path;
	EsStringArray m_channelNames;
	wxBoxSizer* m_contents;
	wxComboBox* m_channels;
	
	EsReflectedClassIntf::Ptr m_curChannel;
	EsReflectedObjectConfigPaneIntf::Ptr m_pane;	
};

#endif // _comm_config_pane_h_
