#ifndef _ekonnect_io_channel_config_pane_h_
#define _ekonnect_io_channel_config_pane_h_

class EsChannelIoEkonnectConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct EsChannelIoEkonnectConfigPane :	public EsBaseIntf,
																				public EsReflectedObjectConfigPaneIntf,
																				public EsReflectedClassIntf
	{
		EsChannelIoEkonnectConfigPane( EsChannelIoEkonnectConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE( EsChannelIoEkonnectConfigPane )
		ES_DECL_ATTR_HANDLING_STD
		
		ES_INTF_MAP_BEGIN( EsChannelIoEkonnectConfigPane )
			ES_INTF_SUPPORTS( EsChannelIoEkonnectConfigPane, EsReflectedObjectConfigPaneIntf )
			ES_INTF_SUPPORTS( EsChannelIoEkonnectConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();
			m_this.Destroy();
		}
		
		// reflected ctor
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			EsChannelIoEkonnectConfigPaneWnd* pane = new EsChannelIoEkonnectConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}

		// EsReflectedObjectConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getObjectName)() const;
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		ES_DECL_INTF_METHOD(void, updateControls)( const EsReflectedClassIntf::Ptr& chnl ) { m_this.m_links.updateControls(chnl); }
		ES_DECL_INTF_METHOD(void, updateObject)( const EsReflectedClassIntf::Ptr& chnl ) const { m_this.m_links.updateObject(chnl); }
		
		// data members
		EsChannelIoEkonnectConfigPaneWnd& m_this;
	};
	friend struct EsChannelIoEkonnectConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	EsChannelIoEkonnectConfigPaneWnd(wxWindow* parent);

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
		
	// interface implementor instance
	EsChannelIoEkonnectConfigPane m_intf;
	// channel property links collection
	EsFtdiDriver::DeviceList m_devices;
	EsPropertyLinks m_links;
};

#endif // _ekonnect_io_channel_config_pane_h_
