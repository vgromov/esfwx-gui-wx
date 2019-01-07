#ifndef _Socket_Server_Io_Channel_Config_Pane_h_
#define _Socket_Server_Io_Channel_Config_Pane_h_

class EsChannelIoSocketServerConfigPane : public  EsReflectedClassConfigPane
{
protected:
  class ConfigPaneWnd : public PaneWnd
  {
  protected:
    ConfigPaneWnd(EsChannelIoSocketServerConfigPane& pane, wxWindow* parent);

  public:
    virtual ~ConfigPaneWnd();

  protected:
	  // reset button click handler
	  void onResetToDefaults(wxCommandEvent& evt);

  protected:
	  // controls
	  wxButton* m_reset;
	  wxStaticText* m_lblPort;
	  wxSpinCtrl* m_edPort;
	  wxStaticText* m_lblTmo;
	  wxSpinCtrl* m_edTmo;	

    friend class EsChannelIoSocketServerConfigPane;
  };

public:
  static EsReflectedObjectConfigPaneIntf::Ptr create(wxWindow* parent);
  
  virtual ~EsChannelIoSocketServerConfigPane();

  /// Necessary EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE;

protected:
  /// Base class implementations
  virtual PaneWnd* doPaneWndCreate(wxWindow* parent) ES_OVERRIDE;
  virtual EsReflectedClassDataSource* doDataSourceCreate(const EsString& className, const EsMetaclassIntf::Ptr& meta) ES_OVERRIDE;
};

#endif // _Socket_Server_Io_Channel_Config_Pane_h_
