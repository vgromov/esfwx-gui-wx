#ifndef _es_channel_io_config_pane_h_
#define _es_channel_io_config_pane_h_

/// Embeddable IO channel configuration GUI pane with IO channel type selection
///
class ESCOMM_GUI_CLASS EsChannelIoConfigPane : public wxPanel
{
public:
	/// Optional parameters - cfg, path, key may point to the config entry,
	/// which is used when specific channel is selected, to update 
	/// property values from persistent storage profiles saved for specific channel name
  ///
	EsChannelIoConfigPane(wxWindow* parent, wxConfigBase* cfg = nullptr, const EsString& path = EsString::null());
	virtual ~EsChannelIoConfigPane();

	/// Set|Get IO channel object
  ///
	void channelSet(const EsReflectedClassIntf::Ptr& chnl);
	EsReflectedClassIntf::Ptr channelGet() const;

protected:
	// WX event handling
	void onChannelSelected( wxCommandEvent& evt );
	
  // helpers
	void currentPaneReset(const EsString& chnlTypeName);
	void currentPaneCreate(const EsString& chnlTypeName);
	void channelSelect(const EsString& chnlTypeName);
	void topLevelFit();

protected:
	EsString m_path;
	EsStringArray m_channelNames;
	wxConfigBase* m_cfg;
	wxBoxSizer* m_contents;
	wxComboBox* m_channels;
	
	EsReflectedClassIntf::Ptr m_channel;
	EsReflectedObjectConfigPaneIntf::Ptr m_paneChannel;	
};

#endif // _es_channel_io_config_pane_h_
