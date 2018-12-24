#ifndef _stub_io_channel_config_pane_h_
#define _stub_io_channel_config_pane_h_

class EsChannelIoStubConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct EsChannelIoStubConfigPane :public EsBaseIntf,
																		public EsReflectedObjectConfigPaneIntf,
																		public EsReflectedClassIntf																	 
	{
		EsChannelIoStubConfigPane( EsChannelIoStubConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE(EsChannelIoStubConfigPane)
		ES_DECL_ATTR_HANDLING_STD
		
		// interface mapping
		ES_INTF_MAP_BEGIN( EsChannelIoStubConfigPane )
			ES_INTF_SUPPORTS( EsChannelIoStubConfigPane, EsReflectedObjectConfigPaneIntf )
			ES_INTF_SUPPORTS( EsChannelIoStubConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();
			m_this.Destroy();
		}
	
		// reflected class services
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			EsChannelIoStubConfigPaneWnd* pane = new EsChannelIoStubConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}
	
		// EsReflectedObjectConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getObjectName)() const;
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		ES_DECL_INTF_METHOD(void, updateControls)( const EsReflectedClassIntf::Ptr& chnl ) {}
		ES_DECL_INTF_METHOD(void, updateObject)( const EsReflectedClassIntf::Ptr& chnl ) const {}
		
		// data members
		EsChannelIoStubConfigPaneWnd& m_this;
	};
	friend struct EsChannelIoStubConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	EsChannelIoStubConfigPaneWnd(wxWindow* parent);

protected:
	// reset button click handler
	//void onResetToDefaults(wxCommandEvent& evt);

protected:
	//wxButton* m_btnResetToDefaults;
	EsChannelIoStubConfigPane m_intf;
};

#endif // _stub_io_channel_config_pane_h_