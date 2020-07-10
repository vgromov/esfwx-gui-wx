#ifndef _socket_client_io_channel_config_pane_
#define _socket_client_io_channel_config_pane_

#ifdef ES_COMM_USE_CHANNEL_IO_SOCKET

class ESCOMM_GUI_CLASS ES_INTF_IMPL_DERIVED(EsChannelIoSocketClientConfigPane, EsReflectedClassConfigPane)

protected:
  class ConfigPaneWnd : public PaneWnd 
  {
  protected:
    ConfigPaneWnd(EsChannelIoSocketClientConfigPane& pane, wxWindow* parent);

  public:
    virtual ~ConfigPaneWnd();

  protected:
	  // Reset button click handler
	  void onResetToDefaults(wxCommandEvent& evt);

  protected:
	  wxButton* m_reset;
	  wxTextCtrl* m_edAddr;
	  wxStaticText* m_lblAddr;
	  wxSpinCtrl* m_edPort;
	  wxStaticText* m_lblPort;
	  wxSpinCtrl* m_edTmo;
	  wxStaticText* m_lblTmo;

    friend class EsChannelIoSocketClientConfigPane;
  };

public:
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);
	virtual ~EsChannelIoSocketClientConfigPane();

  /// Necessary EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

protected:
  /// Base class implementations
  virtual PaneWnd* doPaneWndCreate(wxWindow* parent) ES_OVERRIDE;
  virtual EsReflectedClassDataSource* doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta) ES_OVERRIDE;

ES_INTF_IMPL_END

#endif //< ES_COMM_USE_CHANNEL_IO_SOCKET

#endif //< _socket_client_io_channel_config_pane_
