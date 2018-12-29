#ifndef _ekonnect_io_channel_config_pane_h_
#define _ekonnect_io_channel_config_pane_h_

class ESCOMM_GUI_CLASS EsChannelIoEkonnectConfigPane : EsReflectedClassConfigPane
{
public:
	EsChannelIoEkonnectConfigPane(wxWindow* parent);

protected:
	// selectors population helpers
	void populateDevices();

	// reset button click handler
	void onResetToDefaults(wxCommandEvent& evt);

  // rescan button handler
	void onRescan(wxCommandEvent& evt);	

protected:
	wxButton* m_btnResetToDefaults;
	wxNotebook* m_settings;
	wxPanel* m_pnlStd;
	wxStaticText* m_lblDevice;
	wxComboBox* m_edDeviceName;
	wxBitmapButton* m_btnRescan;
	wxStaticText* m_lblBaud;
	wxComboBox* m_edBaud;
	wxStaticText* m_lblByteSize;
	wxComboBox* m_edByteSize;
	wxStaticText* m_lblParity;
	wxComboBox* m_edParity;
	wxStaticText* m_lblStopBits;
	wxComboBox* m_edStopBits;
	wxPanel* m_pnlAdvanced;
	wxStaticText* m_lblRxTmo;
	wxSpinCtrl* m_edRxTmo;
	wxStaticText* m_lblTxTmo;
	wxSpinCtrl* m_edTxTmo;
	wxStaticText* m_lblRxBuffer;
	wxSpinCtrl* m_edRxBuff;
	wxStaticText* m_lblTxBuffer;
	wxSpinCtrl* m_edTxBuff;
	wxCheckBox* m_chkResetOnRxTmo;
	wxCheckBox* m_chkUseRS232;
};

#endif // _ekonnect_io_channel_config_pane_h_
