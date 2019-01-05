#ifndef _es_io_config_pane_factory_h_
#define _es_io_config_pane_factory_h_

// channel io config panes factory singleton
class ESCOMM_GUI_CLASS EsIoConfigPaneFactory
{
protected:
  typedef std::function<EsReflectedObjectConfigPaneIntf::Ptr(wxWindow*)> PaneCreatorT;
	typedef std::map<EsString, PaneCreatorT> RegistryT;

public:
	static EsReflectedObjectConfigPaneIntf::Ptr create(const EsString& ioTypeName, wxWindow* parent);

protected:
  template <typename IoClassT, typename IoPaneClassT>
  void paneRegister()
  {
    const EsString& name = IoClassT::classNameGetStatic();
    ES_ASSERT(m_registry.end() == m_registry.find(name));
    m_registry[name] = &IoPaneClassT::create;
  }

protected:
	RegistryT m_registry;

private:
	EsIoConfigPaneFactory();
	static const EsIoConfigPaneFactory& instanceGet();

	EsIoConfigPaneFactory(const EsIoConfigPaneFactory&) ES_REMOVEDECL;
	EsIoConfigPaneFactory& operator=(const EsIoConfigPaneFactory&) ES_REMOVEDECL;
};

#endif // _es_io_config_pane_factory_h_
