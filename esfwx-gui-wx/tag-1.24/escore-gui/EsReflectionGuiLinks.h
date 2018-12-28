#ifndef _es_reflection_gui_links_h_
#define _es_reflection_gui_links_h_

/// Object configuration pane interface
///
ES_DECL_INTF_BEGIN(8CC19CF9, D0B24b4c, 8D361982, 26E276BA, EsReflectedObjectConfigPaneIntf)
  ES_DECL_INTF_METHOD(EsString, objectClassNameGet)() const = 0;
  /// Access pane owner form
  ES_DECL_INTF_METHOD(wxWindow*, ownerGet)() = 0;
  /// Update pane controls from linked Reflected object source
  ES_DECL_INTF_METHOD(void, controlsUpdateFromObject)(const EsReflectedClassIntf::Ptr& obj) = 0;
  /// Update Reflected object properties from linked controls
  ES_DECL_INTF_METHOD(void, objectUpdateFromControls)(const EsReflectedClassIntf::Ptr& obj) const = 0;
ES_DECL_INTF_END

/// Reflectd property link classes FWD decls
///
class EsReflectedClassDataSource;
class EsReflectedClassPropertyLink;

///// Reflected property GUI link base class
/////
typedef std::function<void (EsReflectedClassPropertyLink& link)> EsOnSrcObjectSet;
typedef std::function<void (EsReflectedClassPropertyLink& link)> EsOnCtlStateChanged;
typedef std::function<void (EsReflectedClassPropertyLink& sender, const EsVariant& val, EsVariant& suggestedVal)> EsOnCtlValInvalid;

class ESCORE_GUI_CLASS EsReflectedClassPropertyLink
{
protected:
  EsReflectedClassPropertyLink(wxControl* ref, const EsString& propName);

public:
  virtual ~EsReflectedClassPropertyLink();

  bool sourceIsEmpty() const;
  virtual bool isUnidirectional() const = 0;
  void sourceSet(EsReflectedClassDataSource* src);
  void controlUpdateFromObject(const EsReflectedClassIntf::Ptr& obj);
  void objectUpdateFromControl(const EsReflectedClassIntf::Ptr& obj);
  void resetControlToDefault();

  const EsString& propertyNameGet() const;
  EsVariant attributeGet(const EsString& attr) const;
  EsVariant restrictionGet() const;
  EsVariant lookupGet() const;
  EsVariant defaultGet() const;

  /// Linked property attributes access
  ///
  wxString labelGet() const;
  wxString descriptionGet() const;
  
  /// Link visibility control
  bool visibleGet() const;
  void visibleSet(bool visible);

  /// Link enabled state control
  bool enabledGet() const;
  void enabledSet(bool enabled);

  void onSrcObjectSet_set(EsOnSrcObjectSet fn);
  void onCtlStateChanged_set(EsOnCtlStateChanged fn);
  void onCtlValueInvalid_set(EsOnCtlValInvalid fn);

  virtual void i18nStringsUpdate(const EsString& loc) = 0;

protected:
  virtual void initialize() = 0;
  virtual void doControlValSet(const EsVariant& val) = 0;
  virtual EsVariant doControlValGet() const = 0;
  virtual bool doValidateControlVal(const EsVariant& val, EsVariant& suggestedVal) const; //< Optional in-place validator. Default implementation does nothing
  virtual void doControlValueRollback(const EsVariant& val);

protected:
  EsOnSrcObjectSet m_onSrcSet;
  EsOnCtlStateChanged m_onCtlChanged;
  EsOnCtlValInvalid m_onCtlValueInvalid;
  wxWeakRef< wxControl > m_ref;
  EsReflectedClassDataSource* m_src;
  EsString m_propName;

private:
  EsReflectedClassPropertyLink() ES_REMOVEDECL;
  EsReflectedClassPropertyLink(const EsReflectedClassPropertyLink&) ES_REMOVEDECL;
  EsReflectedClassPropertyLink& operator=(const EsReflectedClassPropertyLink&) ES_REMOVEDECL;
};

/// Property links collection for group access to reflected class properties
///
typedef std::set< EsReflectedClassPropertyLink* > EsReflectedClassPropertyLinksT;

class ESCORE_GUI_CLASS EsReflectedClassDataSource
{
public:
  EsReflectedClassDataSource(wxWindow* ref, const EsString& className, const EsMetaclassIntf::Ptr& meta = nullptr);
  virtual ~EsReflectedClassDataSource();

  const EsString& objectClassNameGet() const;

  EsMetaclassIntf::Ptr metaGet() const;
  void metaSet(const EsMetaclassIntf::Ptr& meta);

  inline bool isReadOnly() const ES_NOTHROW { return m_readOnly; }
  
  void link(EsReflectedClassPropertyLink* link);

  void controlsUpdateFromObject(const EsReflectedClassIntf::Ptr& obj);
  void objectUpdateFromControls(const EsReflectedClassIntf::Ptr& obj) const;
  void resetControlsToDefault();

  virtual void i18nStringsUpdate(const EsString& loc);

protected:
  void linkAdd(EsReflectedClassPropertyLink* link);
  void linkRemove(EsReflectedClassPropertyLink* link);
  void classNameCheck(const EsReflectedClassIntf::Ptr& obj) const;

protected:
  wxWeakRef< wxWindow > m_ref;
  EsReflectedClassPropertyLinksT m_links;
  EsMetaclassIntf::Ptr m_meta;
  EsString m_className;
  bool m_readOnly;

private:
  EsReflectedClassDataSource(const EsReflectedClassDataSource&) ES_REMOVEDECL;
  EsReflectedClassDataSource& operator=(const EsReflectedClassDataSource&) ES_REMOVEDECL;

  friend class EsReflectedClassPropertyLink;
};

/// Property links collection for group access to reflected class properties.
/// Reflected class instance is held and owned by the source itself.
///
class ESCORE_GUI_CLASS EsReflectedClassContainerDataSource : public EsReflectedClassDataSource
{
public:
  EsReflectedClassContainerDataSource(wxWindow* owner, const EsReflectedClassIntf::Ptr& obj);
  EsReflectedClassContainerDataSource(wxWindow* owner, const EsString& className, const EsMetaclassIntf::Ptr& meta);
  virtual ~EsReflectedClassContainerDataSource();

  /// Contained object access
  ///

  /// Assign object to container, update target property links, as appropriate.
  /// Only objects of type specified at construction time are allowed.
  /// If object of incompatible type is set, an exception is thrown.
  ///
  void objectSet(const EsReflectedClassIntf::Ptr& obj);

  /// Get currently set data object.
  /// If object is not null, data in object request update before an object is returned to caller
  ///
  EsReflectedClassIntf::Ptr objectGet() const;

  /// Properties
  ///

  /// Return true if data object is empty
  ///
  bool isEmpty() const ES_NOTHROW { return !m_obj; }

  /// Return true if data object is valid
  ///
  bool isValid() const;

protected:
  /// Reflected data object, held and managed by container
  ///
  EsReflectedClassIntf::Ptr m_obj;

private:
  EsReflectedClassContainerDataSource(const EsReflectedClassContainerDataSource&) ES_REMOVEDECL;
  EsReflectedClassContainerDataSource& operator=(const EsReflectedClassContainerDataSource&) ES_REMOVEDECL;
};

/// Combo box link
///
class ESCORE_GUI_CLASS EsComboBoxPropertyLink : public EsReflectedClassPropertyLink
{
public:
  EsComboBoxPropertyLink(const EsString& prop, wxComboBox* cbx, wxStaticText* lbl = nullptr, bool useLookup = false);

  virtual bool isUnidirectional() const ES_OVERRIDE;

  virtual void i18nStringsUpdate(const EsString& loc) ES_OVERRIDE;

protected:
  virtual void initialize();
  virtual void doControlValSet(const EsVariant& val);
  virtual EsVariant doControlValGet() const;

protected:
  wxWeakRef< wxComboBox > m_cbx;
  wxWeakRef< wxStaticText > m_lbl;
  bool m_useLookup;
};

/// spin control porperty link
///
class ESCORE_GUI_CLASS EsSpinCtlPropertyLink : public EsReflectedClassPropertyLink
{
public:
  EsSpinCtlPropertyLink(const EsString& propName, wxSpinCtrl* ctl, wxStaticText* lbl = nullptr);

  virtual bool isUnidirectional() const ES_OVERRIDE;

  virtual void i18nStringsUpdate(const EsString& loc) ES_OVERRIDE;

protected:
  virtual void initialize();
  virtual void doControlValSet(const EsVariant& val);
  virtual EsVariant doControlValGet() const;

protected:
  wxWeakRef< wxSpinCtrl > m_ctl;
  wxWeakRef< wxStaticText > m_lbl;
};

/// spin control double link
///
class ESCORE_GUI_CLASS EsSpinCtlDoublePropertyLink : public EsReflectedClassPropertyLink
{
public:
  EsSpinCtlDoublePropertyLink(const EsString& propName, wxSpinCtrlDouble* ctl, wxStaticText* lbl = nullptr);

  virtual bool isUnidirectional() const ES_OVERRIDE;

  virtual void i18nStringsUpdate(const EsString& loc) ES_OVERRIDE;

protected:
  virtual void initialize();
  virtual void doControlValSet(const EsVariant& val);
  virtual EsVariant doControlValGet() const;

protected:
  wxWeakRef< wxSpinCtrlDouble > m_ctl;
  wxWeakRef< wxStaticText > m_lbl;
};

/// text control link
///
class ESCORE_GUI_CLASS EsTextCtlPropertyLink : public EsReflectedClassPropertyLink
{
public:
  EsTextCtlPropertyLink(const EsString& propName, wxTextCtrl* ctl, wxStaticText* lbl = nullptr);

  virtual bool isUnidirectional() const ES_OVERRIDE;

  virtual void i18nStringsUpdate(const EsString& loc) ES_OVERRIDE;

protected:
  virtual void initialize();
  virtual void doControlValSet(const EsVariant& val);
  virtual EsVariant doControlValGet() const;

protected:
  wxWeakRef< wxTextCtrl > m_ctl;
  wxWeakRef< wxStaticText > m_lbl;
};

/// checkbox property link
///
class ESCORE_GUI_CLASS EsCheckBoxPropertyLink : public EsReflectedClassPropertyLink
{
public:
  EsCheckBoxPropertyLink(const EsString& propName, wxCheckBox* cbx);

  virtual bool isUnidirectional() const ES_OVERRIDE;
  
  virtual void i18nStringsUpdate(const EsString& loc) ES_OVERRIDE;

protected:
  virtual void initialize();
  virtual void doControlValSet(const EsVariant& val);
  virtual EsVariant doControlValGet() const;

protected:
  wxWeakRef< wxCheckBox > m_ctl;
};

#endif // _es_reflection_gui_links_h_
