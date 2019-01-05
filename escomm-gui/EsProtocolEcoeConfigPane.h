#ifndef _es_protocol_ecoe_config_pane_h_
#define _es_protocol_ecoe_config_pane_h_

/// ECO-E Protocol parameters configuration GUI
///
class ESCOMM_GUI_CLASS EsProtocolEcoeConfigPane : public EsReflectedClassConfigPane
{
protected:
	EsProtocolEcoeConfigPane(wxWindow* parent);

public:
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);
  virtual ~EsProtocolEcoeConfigPane();

protected:
	// WX event handlers
	//
	void onResetToDefaults(wxCommandEvent& evt);
	
protected:
  wxButton* m_reset;
	wxStaticText* m_lblPacketTmo;
	wxSpinCtrl* m_edPacketTmo;	
	wxStaticText* m_lblPacketRetries;
	wxSpinCtrl* m_edPacketRetries;
	wxStaticText* m_lblSlaveAddr;
	wxSpinCtrl* m_edSlaveAddr;
};

#endif // _es_protocol_ecoe_config_pane_h_
