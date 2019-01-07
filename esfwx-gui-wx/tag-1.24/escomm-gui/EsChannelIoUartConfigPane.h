#ifndef _uart_io_channel_config_pane_h_
#define _uart_io_channel_config_pane_h_

class EsChannelIoUartConfigPane : public EsReflectedClassConfigPane
{
protected:
  class ConfigPaneWnd : public PaneWnd
  {
  protected:
    ConfigPaneWnd(EsChannelIoUartConfigPane& pane, wxWindow* parent);

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

    friend class EsChannelIoUartConfigPane;
  };

public:
	virtual ~EsChannelIoUartConfigPane();
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);

  /// Necessary EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

protected:
  /// Base class implementations
  virtual PaneWnd* doPaneWndCreate(wxWindow* parent) ES_OVERRIDE;
  virtual EsReflectedClassDataSource* doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta) ES_OVERRIDE;
};

#endif // _uart_io_channel_config_pane_h_