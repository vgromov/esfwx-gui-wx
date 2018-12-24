#ifndef _es_protocol_config_pane_h_
#define _es_protocol_config_pane_h_

// protocol parameters configuration GUI
//

class EKOSF_COMMGUI_CLASS EsProtocolConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct EsProtocolConfigPane :	public EsBaseIntf,
																public EsReflectedObjectConfigPaneIntf,
																public EsReflectedClassIntf
	{
		EsProtocolConfigPane( EsProtocolConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE( EsProtocolConfigPane )
		ES_DECL_ATTR_HANDLING_STD
		
		ES_INTF_MAP_BEGIN( EsProtocolConfigPane )
			ES_INTF_SUPPORTS( EsProtocolConfigPane, EsReflectedObjectConfigPaneIntf )
			ES_INTF_SUPPORTS( EsProtocolConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();
			m_this.Destroy();
		}
		
		// reflected ctor
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			EsProtocolConfigPaneWnd* pane = new EsProtocolConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}

		// EsReflectedObjectConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getObjectName)() const { return EsChannelIoEkonnect::getClassInfoStatic().nameGet(); }
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		ES_DECL_INTF_METHOD(void, updateControls)( const EsReflectedClassIntf::Ptr& chnl ) { m_this.m_links.updateControls(chnl); }
		ES_DECL_INTF_METHOD(void, updateObject)( const EsReflectedClassIntf::Ptr& chnl ) const { m_this.m_links.updateObject(chnl); }
		
		// data members
		EsProtocolConfigPaneWnd& m_this;
	};
	friend struct EsProtocolConfigPane;

protected:
	EsProtocolConfigPaneWnd(wxWindow* parent);

	// event handlers
	//
	void onResetToDefaults(wxCommandEvent& evt);
	
protected:
	// data members	
	//
	wxStaticText* m_lblPacketTmo;
	wxSpinCtrl* m_edPacketTmo;	
	wxStaticText* m_lblPacketRetries;
	wxSpinCtrl* m_edPacketRetries;
	wxStaticText* m_lblSlaveAddr;
	wxSpinCtrl* m_edSlaveAddr;

	EsProtocolConfigPane m_intf;
	// reflected property linkage
	EsPropertyLinks m_links;
};

#endif // _es_protocol_config_pane_h_
