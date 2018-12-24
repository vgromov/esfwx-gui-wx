#ifndef _uart_io_channel_config_pane_h_
#define _uart_io_channel_config_pane_h_

class EsChannelIoUartConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct EsChannelIoUartConfigPane :public EsBaseIntf,
																		public EsReflectedObjectConfigPaneIntf,
																		public EsReflectedClassIntf																	 
	{
		EsChannelIoUartConfigPane( EsChannelIoUartConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE(EsChannelIoUartConfigPane)
		ES_DECL_ATTR_HANDLING_STD
		
		// interface mapping
		ES_INTF_MAP_BEGIN( EsChannelIoUartConfigPane )
			ES_INTF_SUPPORTS( EsChannelIoUartConfigPane, EsReflectedObjectConfigPaneIntf )
			ES_INTF_SUPPORTS( EsChannelIoUartConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();
			m_this.Destroy();
		}
	
		// reflected class services
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			EsChannelIoUartConfigPaneWnd* pane = new EsChannelIoUartConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}
	
		// EsReflectedObjectConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getObjectName)() const;
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		ES_DECL_INTF_METHOD(void, updateControls)( const EsReflectedClassIntf::Ptr& prot ) { m_this.m_links.updateControls(prot); }
		ES_DECL_INTF_METHOD(void, updateObject)( const EsReflectedClassIntf::Ptr& prot ) const { m_this.m_links.updateObject(prot); }
		
		// data members
		EsChannelIoUartConfigPaneWnd& m_this;
	};
	friend struct EsChannelIoUartConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	EsChannelIoUartConfigPaneWnd(wxWindow* parent);

protected:
	// populate ports selector
	void populatePorts();
	
	// reset button click handler
	void onResetToDefaults(wxCommandEvent& evt);
	// rescan button handler
	void onRescan(wxCommandEvent& evt);

protected:
	wxButton* m_btnResetToDefaults;
	wxNotebook* m_settings;
	wxPanel* m_pnlStd;
	wxStaticText* m_lblPort;
	wxComboBox* m_edPortName;
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
	wxStaticText* m_lblRxBuffer;
	wxSpinCtrl* m_edRxBuff;
	wxStaticText* m_lblTxBuffer;
	wxSpinCtrl* m_edTxBuff;
	wxCheckBox* m_chkResetOnRxTmo;

	EsCommPortInfos m_ports;
	EsPropertyLinks m_links;
	EsChannelIoUartConfigPane m_intf;
};

#endif // _uart_io_channel_config_pane_h_