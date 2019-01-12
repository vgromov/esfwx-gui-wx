#ifndef _stub_io_channel_config_pane_h_
#define _stub_io_channel_config_pane_h_

#ifdef ES_COMM_USE_CHANNEL_STUB

class ESCOMM_GUI_CLASS ES_INTF_IMPL_DERIVED( EsChannelIoStubConfigPane, EsReflectedClassConfigPane )

public:
  virtual ~EsChannelIoStubConfigPane();

  /// Necessary EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);

protected:
  virtual PaneWnd* doPaneWndCreate(wxWindow* parent) ES_OVERRIDE;
  virtual EsReflectedClassDataSource* doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta) ES_OVERRIDE;

ES_INTF_IMPL_END

#endif //< ES_COMM_USE_CHANNEL_STUB

#endif //< _stub_io_channel_config_pane_h_
