#ifndef _es_protocol_ecoe_config_pane_h_
#define _es_protocol_ecoe_config_pane_h_

/// ECO-E Protocol parameters configuration GUI
///
class ESCOMM_GUI_CLASS EsProtocolEcoeConfigPane : public EsReflectedClassConfigPane
{
protected:
  class ConfigPaneWnd : public PaneWnd
  {
  protected:
    ConfigPaneWnd(EsProtocolEcoeConfigPane& pane, wxWindow* parent);

  public:
    virtual ~ConfigPaneWnd();

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

    friend class EsProtocolEcoeConfigPane;
  };

public:
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);
  virtual ~EsProtocolEcoeConfigPane();

  /// Necessary EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

protected:
  /// Base class implementations
  virtual PaneWnd* doPaneWndCreate(wxWindow* parent) ES_OVERRIDE;
  virtual EsReflectedClassDataSource* doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta) ES_OVERRIDE;
};

#endif // _es_protocol_ecoe_config_pane_h_
