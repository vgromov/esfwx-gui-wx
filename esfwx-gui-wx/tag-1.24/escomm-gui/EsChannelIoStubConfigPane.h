#ifndef _stub_io_channel_config_pane_h_
#define _stub_io_channel_config_pane_h_

class EsChannelIoStubConfigPane : public EsReflectedClassConfigPane
{
public:
  virtual ~EsChannelIoStubConfigPane();

  /// Necessary EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);

protected:
  virtual PaneWnd* doPaneWndCreate(wxWindow* parent) ES_OVERRIDE;
  virtual EsReflectedClassDataSource* doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta) ES_OVERRIDE;
};

#endif // _stub_io_channel_config_pane_h_