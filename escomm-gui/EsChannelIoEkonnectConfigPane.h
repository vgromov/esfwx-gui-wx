#ifndef _ekonnect_io_channel_config_pane_h_
#define _ekonnect_io_channel_config_pane_h_

#ifdef ES_COMM_USE_CHANNEL_EKONNECT

class ESCOMM_GUI_CLASS ES_INTF_IMPL_DERIVED(EsChannelIoEkonnectConfigPane, EsReflectedClassConfigPane)

protected:
  class ConfigPaneWnd : public PaneWnd
  {
  protected:
    ConfigPaneWnd(EsChannelIoEkonnectConfigPane& pane, wxWindow* parent);

  public:
    virtual ~ConfigPaneWnd();

  protected:
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
    
    friend class EsChannelIoEkonnectConfigPane;
  };

public:
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);
  virtual ~EsChannelIoEkonnectConfigPane();

  /// Necessary EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

protected:
  /// Base class implementations
  virtual PaneWnd* doPaneWndCreate(wxWindow* parent) ES_OVERRIDE;
  virtual EsReflectedClassDataSource* doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta) ES_OVERRIDE;

ES_INTF_IMPL_END

#endif //< ES_COMM_USE_CHANNEL_EKONNECT

#endif //< _ekonnect_io_channel_config_pane_h_
