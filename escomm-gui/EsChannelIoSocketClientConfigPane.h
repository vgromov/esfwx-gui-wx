#ifndef _socket_client_io_channel_config_pane_
#define _socket_client_io_channel_config_pane_

#include <wx/spinctrl.h>

class EsChannelIoSocketClientConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct EsChannelIoSocketClientConfigPane :public EsBaseIntf,
																						public EsReflectedObjectConfigPaneIntf,
																						public EsReflectedClassIntf
	{
		EsChannelIoSocketClientConfigPane( EsChannelIoSocketClientConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE(EsChannelIoSocketClientConfigPane)
		ES_DECL_ATTR_HANDLING_STD
		
		ES_INTF_MAP_BEGIN( EsChannelIoSocketClientConfigPane )
			ES_INTF_SUPPORTS( EsChannelIoSocketClientConfigPane, EsReflectedObjectConfigPaneIntf )
			ES_INTF_SUPPORTS( EsChannelIoSocketClientConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();	
			m_this.Destroy();
		}
		
		// reflected constructor
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			EsChannelIoSocketClientConfigPaneWnd* pane = new EsChannelIoSocketClientConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}		
	
		// EsReflectedObjectConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getObjectName)() const;
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		ES_DECL_INTF_METHOD(void, updateControls)( const EsReflectedClassIntf::Ptr& chnl ) { m_this.m_links.updateControls(chnl); }
		ES_DECL_INTF_METHOD(void, updateObject)( const EsReflectedClassIntf::Ptr& chnl ) const { m_this.m_links.updateObject(chnl); }		
		
		// data members
		EsChannelIoSocketClientConfigPaneWnd& m_this;
	};
	friend struct EsChannelIoSocketClientConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	EsChannelIoSocketClientConfigPaneWnd(wxWindow* parent);

protected:
	virtual ~EsChannelIoSocketClientConfigPaneWnd();

	// set default values
	void setDefaults();
	// reset button click handler
	void onResetToDefaults(wxCommandEvent& evt);

protected:
	// interface implementor instance
	EsChannelIoSocketClientConfigPane m_intf;
	// controls
	wxButton* m_reset;
	wxTextCtrl* m_edAddr;
	wxStaticText* m_lblAddr;
	wxSpinCtrl* m_edPort;
	wxStaticText* m_lblPort;
	wxSpinCtrl* m_edTmo;
	wxStaticText* m_lblTmo;
	// property links
	EsPropertyLinks m_links;
};

#endif // _socket_client_io_channel_config_pane_
