#ifndef _es_channel_io_bluetooth_le_config_pane_
#define _es_channel_io_bluetooth_le_config_pane_

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE

class ESCOMM_GUI_CLASS ES_INTF_IMPL_DERIVED(EsChannelIoBluetoothLeConfigPane, EsReflectedClassConfigPane)

protected:
  class ConfigPaneWnd : public PaneWnd 
  {
  protected:
    ConfigPaneWnd(EsChannelIoBluetoothLeConfigPane& pane, wxWindow* parent);

  public:
    virtual ~ConfigPaneWnd();

  protected:
	  // Reset button click handler
	  void onResetToDefaults(wxCommandEvent& evt);

  protected:
	  wxButton* m_reset;
	  wxTextCtrl* m_edAddr;
	  wxStaticText* m_lblAddr;
	  wxCheckBox* m_edSubscribe;

    friend class EsChannelIoBluetoothLeConfigPane;
  };

public:
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);
	virtual ~EsChannelIoBluetoothLeConfigPane();

  /// Necessary EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

protected:
  /// Base class implementations
  virtual PaneWnd* doPaneWndCreate(wxWindow* parent) ES_OVERRIDE;
  virtual EsReflectedClassDataSource* doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta) ES_OVERRIDE;

ES_INTF_IMPL_END

#endif //< ES_COMM_USE_CHANNEL_BLUETOOTH_LE

#endif //< _es_channel_io_bluetooth_le_config_pane_
