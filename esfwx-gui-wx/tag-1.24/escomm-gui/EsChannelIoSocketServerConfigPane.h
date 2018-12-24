#ifndef _Socket_Server_Io_Channel_Config_Pane_h_
#define _Socket_Server_Io_Channel_Config_Pane_h_

#include <wx/spinctrl.h>

class EsChannelIoSocketServerConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct EsChannelIoSocketServerConfigPane :public EsBaseIntf,
																						public EsReflectedObjectConfigPaneIntf,
																						public EsReflectedClassIntf
	{
		EsChannelIoSocketServerConfigPane( EsChannelIoSocketServerConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE( EsChannelIoSocketServerConfigPane )
		ES_DECL_ATTR_HANDLING_STD
		
		ES_INTF_MAP_BEGIN( EsChannelIoSocketServerConfigPane )
			ES_INTF_SUPPORTS( EsChannelIoSocketServerConfigPane, EsReflectedObjectConfigPaneIntf )
			ES_INTF_SUPPORTS( EsChannelIoSocketServerConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();
			m_this.Destroy();
		}
		
		// reflected constructor
				// class services
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			EsChannelIoSocketServerConfigPaneWnd* pane = new EsChannelIoSocketServerConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}

		// EsReflectedObjectConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getObjectName)() const;
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		ES_DECL_INTF_METHOD(void, updateControls)( const EsReflectedClassIntf::Ptr& chnl ) { m_this.m_links.updateControls(chnl); }
		ES_DECL_INTF_METHOD(void, updateObject)( const EsReflectedClassIntf::Ptr& chnl ) const { m_this.m_links.updateObject(chnl); }		
		
		// data members
		EsChannelIoSocketServerConfigPaneWnd& m_this;
	};
	friend struct EsChannelIoSocketServerConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	EsChannelIoSocketServerConfigPaneWnd(wxWindow* parent);

protected:
	virtual ~EsChannelIoSocketServerConfigPaneWnd();

	// reset button click handler
	void onResetToDefaults(wxCommandEvent& evt);

protected:
	// interface implementor instance
	EsChannelIoSocketServerConfigPane m_intf;
	// controls
	wxButton* m_reset;
	wxStaticText* m_lblPort;
	wxSpinCtrl* m_edPort;
	wxStaticText* m_lblTmo;
	wxSpinCtrl* m_edTmo;	
	// property links
	EsPropertyLinks m_links;
};

#endif // _Socket_Server_Io_Channel_Config_Pane_h_
