#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsCoreGui.h"

#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsCoreGui.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

// common link base
//
__fastcall EsReflectedClassPropertyLink::EsReflectedClassPropertyLink(TComponent* Owner, const EsString& propName) :
  TComponent(Owner),
  m_i18nScope(this),
  m_onSrcSet(nullptr),
  m_onCtlChanged(nullptr),
  m_src(nullptr),
  m_propName(propName)
{
  ES_ASSERT(!m_propName.empty());
}
//---------------------------------------------------------------------------

__fastcall EsReflectedClassPropertyLink::~EsReflectedClassPropertyLink()
{
  sourceSet(nullptr);
}
//---------------------------------------------------------------------------

void __fastcall EsReflectedClassPropertyLink::onI18nStringsUpdate(TObject* sender)
{
  // Default implementation does nothing
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::i18nInit()
{
  m_i18nScope.onGuiStringsUpdate = onI18nStringsUpdate;
  m_i18nScope.managerSet(EsI18nManager::instanceGet());
}
//---------------------------------------------------------------------------

const EsString& EsReflectedClassPropertyLink::propertyNameGet() const
{
  return m_propName;
}
//---------------------------------------------------------------------------

bool EsReflectedClassPropertyLink::sourceIsEmpty() const
{
  return nullptr == m_src;
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::sourceSet(EsReflectedClassDataSource* src)
{
  if(m_src != src)
  {
    if(m_src)
    {
      m_src->linkRemove(this);
      m_src = nullptr;
    }

    m_src = src;

    if(m_onSrcSet)
      m_onSrcSet(this);

    if(m_src)
    {
      m_src->linkAdd(this);
      // (re-) initialize stuff upon each link to source
      initialize();
      // (re-) set control to default upon each link to source
      resetControlToDefault();
    }
  }
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::updateControl(const EsReflectedClassIntf::Ptr& obj)
{
  if(!m_src)
    return;

  if(!obj)
    return;

  ES_ASSERT(m_src->m_className == obj->typeNameGet());
  const EsVariant& var = obj->propertyGet(m_propName);
  doControlValSet(var);

  if(m_onCtlChanged)
    m_onCtlChanged(this);
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::doControlValueRollback(const EsVariant& val)
{
  doControlValSet(
    val
  );
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::updateProperty(const EsReflectedClassIntf::Ptr& obj)
{
  if(m_src && obj && !isUnidirectional())
  {
    ES_ASSERT(m_src->m_className == obj->typeNameGet());
    const EsVariant& var = doControlValGet();

    EsVariant suggested;
    if(!doValidateControlVal(var, suggested))
    {
      if(m_onCtlValueInvalid)
        m_onCtlValueInvalid(
          this,
          var,
          suggested
        );

      doControlValueRollback(suggested);
      return;
    }

    obj->propertySet(
      m_propName,
      var
    );
  }
}
//---------------------------------------------------------------------------

EsVariant EsReflectedClassPropertyLink::attributeGet(const EsString& attr) const
{
  if(m_src)
  {
    ES_ASSERT(m_src->m_meta);

    if(m_src->m_meta->propertyHasAttribute(m_src->m_className, m_propName, attr))
      return m_src->m_meta->propertyAttributeGet(m_src->m_className, m_propName, attr);
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

bool EsReflectedClassPropertyLink::doValidateControlVal(const EsVariant& ES_UNUSED(val), EsVariant& ES_UNUSED(suggestedVal)) const
{
  return true;
}
//---------------------------------------------------------------------------

EsVariant EsReflectedClassPropertyLink::defaultGet() const
{
  return attributeGet(esT("default"));
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::resetControlToDefault()
{
  if(!m_src)
    return;

  const EsVariant& var = defaultGet();
  if(!var.isEmpty())
  {
    doControlValSet(var);

    if(m_onCtlChanged)
      m_onCtlChanged(this);
  }
}
//---------------------------------------------------------------------------

EsVariant EsReflectedClassPropertyLink::restrictionGet() const
{
  return attributeGet(esT("restriction"));
}
//---------------------------------------------------------------------------

EsVariant EsReflectedClassPropertyLink::lookupGet() const
{
  return attributeGet(esT("lookup"));
}
//---------------------------------------------------------------------------

UnicodeString __fastcall EsReflectedClassPropertyLink::labelGet() const
{
  UnicodeString result;

  if(m_src)
  {
    ES_ASSERT(m_src->m_meta);
    result = m_src->m_meta->propertyLabelGet(
      m_src->m_className,
      m_propName
    ).c_str();
  }

  return result;
}
//---------------------------------------------------------------------------

UnicodeString __fastcall EsReflectedClassPropertyLink::descriptionGet() const
{
  UnicodeString result;

  if(m_src)
  {
    ES_ASSERT(m_src->m_meta);
    result = m_src->m_meta->propertyDescriptionGet(
      m_src->m_className,
      m_propName
    ).c_str();
  }

  return result;
}
//---------------------------------------------------------------------------

bool __fastcall EsReflectedClassPropertyLink::visibleGet() const
{
  TControl* ctl = dynamic_cast<TControl*>(Owner);
  if(ctl)
    return ctl->Visible;

  return false;
}
//---------------------------------------------------------------------------

void __fastcall EsReflectedClassPropertyLink::visibleSet(bool visible)
{
  TControl* ctl = dynamic_cast<TControl*>(Owner);
  if(ctl)
    ctl->Visible = visible;
}
//---------------------------------------------------------------------------

bool __fastcall EsReflectedClassPropertyLink::enabledGet() const
{
  TControl* ctl = dynamic_cast<TControl*>(Owner);
  if(ctl)
    return ctl->Enabled;

  return true;
}
//---------------------------------------------------------------------------

void __fastcall EsReflectedClassPropertyLink::enabledSet(bool enabled)
{
  TControl* ctl = dynamic_cast<TControl*>(Owner);
  if(ctl)
    ctl->Enabled = enabled;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// links collection implementation
//
__fastcall EsReflectedClassDataSource::EsReflectedClassDataSource(TComponent* Owner,
  const EsString& className, const EsMetaclassIntf::Ptr& meta /*= EsMetaclassIntf::Ptr()*/) :
  TComponent(Owner),
  m_readOnly(false),
  m_className(className)
{
  ES_ASSERT(!m_className.empty());
  metaSet(meta);
}
//---------------------------------------------------------------------------

__fastcall EsReflectedClassDataSource::~EsReflectedClassDataSource()
{
  while(!m_links.empty())
  {
    EsReflectedClassPropertyLinksT::iterator it = m_links.begin();
    (*it)->sourceSet(nullptr);
  }
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::i18nInit()
{
  for(EsReflectedClassPropertyLinksT::iterator it = m_links.begin(); it != m_links.end(); ++it)
    (*it)->i18nInit();
}
//---------------------------------------------------------------------------

EsMetaclassIntf::Ptr EsReflectedClassDataSource::metaGet() const
{
  return m_meta;
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::metaSet(const EsMetaclassIntf::Ptr& meta)
{
  if(meta)
    m_meta = meta;
  else
    m_meta = EsMetaclass::get();

  if(!m_meta->hasClass(m_className))
    EsException::Throw(esT("Meta information for class '%s' could not be found"), m_className.c_str());
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::link(EsReflectedClassPropertyLink* link)
{
  ES_ASSERT(link);
  link->sourceSet(this);
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::linkAdd(EsReflectedClassPropertyLink* link)
{
  ES_ASSERT(link);
  m_links.insert(link);
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::linkRemove(EsReflectedClassPropertyLink* link)
{
  ES_ASSERT(link);
  m_links.erase(link);
}
//---------------------------------------------------------------------------

const EsString& EsReflectedClassDataSource::classNameGet() const
{
  return m_className;
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::classNameCheck(const EsReflectedClassIntf::Ptr& obj) const
{
  if(m_className != obj->typeNameGet())
    EsException::ThrowIncompatibleObjectTypes(m_className, obj->typeNameGet());
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::resetControlsToDefault()
{
  for(EsReflectedClassPropertyLinksT::iterator it = m_links.begin(); it != m_links.end(); ++it)
    (*it)->resetControlToDefault();
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::updateControls(const EsReflectedClassIntf::Ptr& obj)
{
  if(obj)
    classNameCheck(obj);

  for(EsReflectedClassPropertyLinksT::iterator it = m_links.begin(); it != m_links.end(); ++it)
    (*it)->updateControl(obj);
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::updateObject(const EsReflectedClassIntf::Ptr& obj) const
{
  if(m_readOnly)
    return;

  if(obj)
  {
    classNameCheck(obj);
    for(EsReflectedClassPropertyLinksT::const_iterator it = m_links.begin(); it != m_links.end(); ++it)
      (*it)->updateProperty(obj);
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Links collection with object container
__fastcall EsReflectedClassContainerDataSource::EsReflectedClassContainerDataSource(
  TComponent* Owner, const EsReflectedClassIntf::Ptr& obj) :
  EsReflectedClassDataSource(Owner, obj->typeNameGet(), obj->metaGet()),
  m_obj(obj)
{
}
//---------------------------------------------------------------------------

__fastcall EsReflectedClassContainerDataSource::EsReflectedClassContainerDataSource(
  TComponent* Owner, const EsString& className, const EsMetaclassIntf::Ptr& meta) :
  EsReflectedClassDataSource(Owner, className, meta)
{
  ES_ASSERT(meta);
}
//---------------------------------------------------------------------------

__fastcall EsReflectedClassContainerDataSource::~EsReflectedClassContainerDataSource()
{
  objectSet(0);
  m_meta.reset();
}
//---------------------------------------------------------------------------

/// Contained object access
void EsReflectedClassContainerDataSource::objectSet(const EsReflectedClassIntf::Ptr& obj)
{
  if(obj)
  {
    if(m_className != obj->typeNameGet())
      EsException::Throw(esT("Could not assign incompatible object to data source. Expected '%s', got '%s'."),
        m_className.c_str(), obj->typeNameGet().c_str());

    metaSet(obj->metaGet());
  }

  m_obj = obj;
  updateControls(m_obj);
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsReflectedClassContainerDataSource::objectGet() const
{
  if(m_obj)
    updateObject(m_obj);

  return m_obj;
}
//---------------------------------------------------------------------------

bool __fastcall EsReflectedClassContainerDataSource::emptyGet() const
{
  return 0 == m_obj;
}
//---------------------------------------------------------------------------

bool __fastcall EsReflectedClassContainerDataSource::validGet() const
{
  if(0 != m_obj)
  {
    if(m_obj->hasProperty(esT("valid")))
      return m_obj->propertyGet(esT("valid")).asBool();

    return true;
  }

  return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// Closure property link. The __closure is used to handle data updates to\from
/// linked reflected property.
///

__fastcall EsClosurePropertyLink::EsClosurePropertyLink(const EsString& prop, TComponent* owner,
  EsControlValSet setter, EsControlValGet getter) :
  EsReflectedClassPropertyLink(owner, prop),
  m_setter(setter),
  m_getter(getter)
{
  ES_ASSERT(owner);
}
//---------------------------------------------------------------------------

__fastcall EsClosurePropertyLink::~EsClosurePropertyLink()
{}
//---------------------------------------------------------------------------

void EsClosurePropertyLink::initialize()
{}
//---------------------------------------------------------------------------

bool EsClosurePropertyLink::isUnidirectional() const
{
  return 0 == m_getter;
}
//---------------------------------------------------------------------------

void EsClosurePropertyLink::doControlValSet(const EsVariant& val)
{
  if(!m_setter)
    EsException::Throw(esT("Value setter closure is not set-up"));

  m_setter(val);
}
//---------------------------------------------------------------------------

EsVariant EsClosurePropertyLink::doControlValGet() const
{
  if(!m_getter)
    EsException::Throw(esT("Value getter closure is not set-up"));

  return m_getter();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// RTTI property link
///
__fastcall EsRttiPropertyLink::EsRttiPropertyLink(const EsString& prop, TComponent* cmp, const EsString& rttiProp) :
  EsReflectedClassPropertyLink(cmp, prop),
  m_cmp(cmp),
  m_ptype(0)
{
  ES_ASSERT(m_cmp);

  TRttiContext rtti;
  TMetaClass* ctype = m_cmp->ClassType();
  TRttiType* type = rtti.GetType(ctype);
  ES_ASSERT(type);
  m_ptype = type->GetProperty(rttiProp.c_str());
  ES_ASSERT(m_ptype);
}
//---------------------------------------------------------------------------

__fastcall EsRttiPropertyLink::~EsRttiPropertyLink()
{}
//---------------------------------------------------------------------------

bool EsRttiPropertyLink::isUnidirectional() const
{
  return true;
}
//---------------------------------------------------------------------------

void EsRttiPropertyLink::initialize()
{}
//---------------------------------------------------------------------------

void EsRttiPropertyLink::doControlValSet(const EsVariant& val)
{
  ES_ASSERT(m_ptype);

  const TValue& tval = EsUtilities::toTValue(val);
  m_ptype->SetValue(m_cmp, tval);
}
//---------------------------------------------------------------------------

EsVariant EsRttiPropertyLink::doControlValGet() const
{
  ES_ASSERT(m_ptype);
  EsException::Throw(esT("EsRttiPropertyLink does not support property write"));
  return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// RTTI property link for indexed reflected properties
///
__fastcall EsRttiPropertyLinkIndexed::IndexLink::
IndexLink(EsRttiPropertyLinkIndexed* container, ulong index, TComponent* cmp, const EsString& rttiProp) :
  TComponent(cmp),
  m_container(0),
  m_cmp(cmp),
  m_ptype(0),
  m_index(index)
{
  ES_ASSERT(m_cmp);

  TRttiContext rtti;
  TRttiType* type = rtti.GetType(m_cmp->ClassType());
  ES_ASSERT(type);
  m_ptype = type->GetProperty(rttiProp.c_str());
  ES_ASSERT(m_ptype);

  containerSet(container);
}
//---------------------------------------------------------------------------

__fastcall EsRttiPropertyLinkIndexed::IndexLink::~IndexLink()
{
  containerSet(nullptr);
}
//---------------------------------------------------------------------------

ulong EsRttiPropertyLinkIndexed::IndexLink::indexGet() const
{
  return m_index;
}
//---------------------------------------------------------------------------

void EsRttiPropertyLinkIndexed::IndexLink::containerSet(EsRttiPropertyLinkIndexed* container)
{
  if(m_container != container)
  {
    if(m_container)
      m_container->linkRemove(this);

    m_container = container;
    if(m_container)
      m_container->linkAdd(this);
  }
}
//---------------------------------------------------------------------------

void EsRttiPropertyLinkIndexed::IndexLink::doControlValSet(const EsVariant& val)
{
  ES_ASSERT(m_ptype);
  if(val.isEmpty())
    m_ptype->SetValue(m_cmp, TValue::Empty);
  else
    m_ptype->SetValue(m_cmp, EsUtilities::toTValue(val.itemGet(m_index)));
}
//---------------------------------------------------------------------------

__fastcall EsRttiPropertyLinkIndexed::EsRttiPropertyLinkIndexed(const EsString& prop, TComponent* owner) :
  EsReflectedClassPropertyLink(owner, prop)
{
}
//---------------------------------------------------------------------------

__fastcall EsRttiPropertyLinkIndexed::~EsRttiPropertyLinkIndexed()
{
  while(!m_idxLinks.empty())
  {
    IndexedLinksT::iterator it = m_idxLinks.begin();
    (*it).second->containerSet(nullptr);
  }
}
//---------------------------------------------------------------------------

void EsRttiPropertyLinkIndexed::indexLinkAdd(ulong index, TComponent* cmp, const EsString& rttiProp)
{
#ifdef ES_DEBUG
  IndexLink* il =
#endif
    new IndexLink(this, index, cmp, rttiProp);

#ifdef ES_DEBUG
  ES_ASSERT(il);
#endif
}
//---------------------------------------------------------------------------

void EsRttiPropertyLinkIndexed::linkAdd(IndexLink* link)
{
  ES_ASSERT(link);
  IndexedLinksT::iterator it = m_idxLinks.find(link->indexGet());
  if(it != m_idxLinks.end())
    EsException::Throw(esT("Index '%d' link already exists in container"), link->indexGet());

  m_idxLinks[link->indexGet()] = link;
}
//---------------------------------------------------------------------------

void EsRttiPropertyLinkIndexed::linkRemove(IndexLink* link)
{
  ES_ASSERT(link);

  IndexedLinksT::iterator it = m_idxLinks.find(link->indexGet());
  if(it == m_idxLinks.end())
    EsException::Throw(esT("Index '%d' link does not exist in container"), link->indexGet());

  m_idxLinks.erase(it);
}
//---------------------------------------------------------------------------

bool EsRttiPropertyLinkIndexed::isUnidirectional() const
{
  return true;
}
//---------------------------------------------------------------------------

void EsRttiPropertyLinkIndexed::initialize()
{}
//---------------------------------------------------------------------------

void EsRttiPropertyLinkIndexed::doControlValSet(const EsVariant& val)
{
  for(IndexedLinksT::iterator it = m_idxLinks.begin(); it != m_idxLinks.end(); ++it)
    (*it).second->doControlValSet(val);
}
//---------------------------------------------------------------------------

EsVariant EsRttiPropertyLinkIndexed::doControlValGet() const
{
  EsException::Throw(esT("EsRttiPropertyLinkIndexed does not support property write"));
  return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// combo box link implementation
//
__fastcall EsComboBoxPropertyLink::EsComboBoxPropertyLink(const EsString& prop, TComboBox* cbx, TLabel* lbl, bool useLookup) :
  EsReflectedClassPropertyLink(cbx, prop),
  m_cbx(cbx),
  m_lbl(lbl),
  m_useLookup(useLookup)
{
  ES_ASSERT(cbx);
}
//---------------------------------------------------------------------------

__fastcall EsComboBoxPropertyLink::~EsComboBoxPropertyLink()
{}
//---------------------------------------------------------------------------

void __fastcall EsComboBoxPropertyLink::onI18nStringsUpdate(TObject* sender)
{
  ES_ASSERT(m_cbx);

  TNotifyEvent oldOnChange = m_cbx->OnChange;
  try
  {
    EsUpdateLocker<TComboBox> lock(m_cbx);

    m_cbx->OnChange = nullptr;

    int idx = m_cbx->ItemIndex;
    m_cbx->Clear();

    initialize();

    m_cbx->ItemIndex = idx;
  }
  __finally
  {
    m_cbx->OnChange = oldOnChange;
  }
}
//---------------------------------------------------------------------------

bool EsComboBoxPropertyLink::isUnidirectional() const
{
  return !m_cbx->Enabled;
}
//---------------------------------------------------------------------------

void EsComboBoxPropertyLink::initialize()
{
  ES_ASSERT(m_src);
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(label.c_str()) + esT(":");
    m_lbl->Text = txt.c_str();
  }

  ES_ASSERT(m_cbx);
  m_cbx->Hint = EsStringI18n::translate(description.c_str()).c_str();

  const EsVariant& restriction = m_useLookup ?
    lookupGet() :
    restrictionGet();

  if(restriction.isEmpty())
    return;

  if(m_useLookup)
  {
    const EsVariant::Array& va = restriction.asVariantCollection();
    EsUpdateLocker<TStrings> lock(m_cbx->Items);
    for(size_t idx = 0; idx < va.size(); ++idx)
    {
      const EsString& lbl = EsStringI18n::translate(va[idx].itemGet(0).asString());

      m_cbx->Items->Add(
        lbl.c_str()
      );
    }
    return;
  }

  if(restriction.isCollection())
    EsUtilities::toStringList(m_cbx->Items, restriction.asStringCollection());
  else if(restriction.isObject())
  {
    EsReflectedClassIntf::Ptr _enum = restriction.asExistingObject();
    ES_ASSERT(_enum);
    if(_enum->isKindOf(esT("EsEnumeration")))
      EsUtilities::toStringList(m_cbx->Items, _enum->propertyGet(esT("labels")).asStringCollection());
  }
}
//---------------------------------------------------------------------------

void EsComboBoxPropertyLink::doControlValSet(const EsVariant& val)
{
  const EsVariant& restriction = m_useLookup ?
    lookupGet() :
    restrictionGet();

  if(m_useLookup)
  {
    const EsVariant::Array& va = restriction.asVariantCollection();
    int itemIdx = 0; //< Fallback to lookup item at index 0
    for(size_t idx = 0; idx < va.size(); ++idx)
    {
      if(val == va[idx].itemGet(1))
      {
        itemIdx = idx;
        break;
      }
    }

    m_cbx->ItemIndex = itemIdx;
    return;
  }

  if(restriction.isObject())
  {
    EsReflectedClassIntf::Ptr _enum = restriction.asExistingObject();
    ES_ASSERT(_enum);
    if(_enum->isKindOf(esT("EsEnumeration")))
    {
      ulong idx = _enum->call(esT("valueIndexGet"), val).asULong();
      m_cbx->ItemIndex = idx;
      return;
    }
  }

  int idx = m_cbx->Items->IndexOf(val.asString().c_str());
  if(-1 != idx)
    m_cbx->ItemIndex = idx;
}
//---------------------------------------------------------------------------

EsVariant EsComboBoxPropertyLink::doControlValGet() const
{
  const EsVariant& restriction = m_useLookup ?
    lookupGet() :
    restrictionGet();

  if(m_useLookup && -1 < m_cbx->ItemIndex)
  {
    return restriction.itemGet(m_cbx->ItemIndex)
      .itemGet(1);
  }

  if(restriction.isObject())
  {
    EsReflectedClassIntf::Ptr _enum = restriction.asExistingObject();
    ES_ASSERT(_enum);
    if(_enum->isKindOf(esT("EsEnumeration")))
    {
      long idx = m_cbx->ItemIndex;
      ES_ASSERT(-1 != idx);
      return _enum->call(EsStdNames::itemGet(), idx);
    }
  }

  if(m_cbx->Selected)
    return m_cbx->Selected->Text.c_str();

  return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// spin control link implementation
//
__fastcall EsSpinCtlPropertyLink::EsSpinCtlPropertyLink(const EsString& propName, TNumberBox* ctl, TLabel* lbl) :
  EsReflectedClassPropertyLink(ctl, propName),
  m_ctl(ctl),
  m_lbl(lbl)
{
  ES_ASSERT(m_ctl);
}
//---------------------------------------------------------------------------

__fastcall EsSpinCtlPropertyLink::~EsSpinCtlPropertyLink()
{}
//---------------------------------------------------------------------------

bool EsSpinCtlPropertyLink::isUnidirectional() const
{
  return !m_ctl->Enabled || m_ctl->ReadOnly;
}
//---------------------------------------------------------------------------

void __fastcall EsSpinCtlPropertyLink::onI18nStringsUpdate(TObject* sender)
{
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(label.c_str()) + esT(":");
    m_lbl->Text = txt.c_str();
  }

  ES_ASSERT(m_ctl);
  m_ctl->Hint = EsStringI18n::translate(description.c_str()).c_str();
}
//---------------------------------------------------------------------------

void EsSpinCtlPropertyLink::initialize()
{
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(label.c_str()) + esT(":");
    m_lbl->Text = txt.c_str();
  }

  m_ctl->Hint = EsStringI18n::translate(description.c_str()).c_str();

  const EsVariant& restriction = restrictionGet();
  if(!restriction.isEmpty())
  {
    EsVariant min;
    EsVariant max;
    if(restriction.isObject())
    {
      EsReflectedClassIntf::Ptr range = restriction.asExistingObject();
      ES_ASSERT(range);

      if(range->isKindOf(esT("EsRange")))
      {
        min = range->propertyGet(esT("min"));
        max = range->propertyGet(esT("max"));
      }
    }
    else if(restriction.isCollection())
    {
      min = restriction.itemGet(0);
      max = restriction.itemGet(1);
    }

    EsUpdateLocker<TControl> lock(m_ctl);
    if(min.isEmpty())
    {
      m_ctl->Min = INT_MIN;
      m_ctl->Max = max.asInt();
    }
    else if(max.isEmpty())
    {
      m_ctl->Max = INT_MAX;
      m_ctl->Min = min.asInt();
    }
    else
    {
      m_ctl->Min = min.asInt();
      m_ctl->Max = max.asInt();
    }
  }
}
//---------------------------------------------------------------------------

void EsSpinCtlPropertyLink::doControlValSet(const EsVariant& val)
{
  m_ctl->Value = val.asLong();
}
//---------------------------------------------------------------------------

EsVariant EsSpinCtlPropertyLink::doControlValGet() const
{
  return static_cast<long>(m_ctl->Value);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// spin control double link implementation
//
__fastcall EsSpinCtlDoublePropertyLink::EsSpinCtlDoublePropertyLink(const EsString& propName,
  TNumberBox* ctl, TLabel* lbl) :
  EsReflectedClassPropertyLink(ctl, propName),
  m_ctl(ctl),
  m_lbl(lbl)
{
  ES_ASSERT(m_ctl);
}
//---------------------------------------------------------------------------

__fastcall EsSpinCtlDoublePropertyLink::~EsSpinCtlDoublePropertyLink()
{}
//---------------------------------------------------------------------------

bool EsSpinCtlDoublePropertyLink::isUnidirectional() const
{
  return !m_ctl->Enabled || m_ctl->ReadOnly;
}
//---------------------------------------------------------------------------

void __fastcall EsSpinCtlDoublePropertyLink::onI18nStringsUpdate(TObject* sender)
{
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(label.c_str()) + esT(":");
    m_lbl->Text = txt.c_str();
  }

  m_ctl->Hint = EsStringI18n::translate(description.c_str()).c_str();
}
//---------------------------------------------------------------------------

void EsSpinCtlDoublePropertyLink::initialize()
{
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(label.c_str()) + esT(":");
    m_lbl->Text = txt.c_str();
  }

  m_ctl->Hint = EsStringI18n::translate(description.c_str()).c_str();

  const EsVariant& restriction = restrictionGet();
  if(!restriction.isEmpty())
  {
    EsVariant min;
    EsVariant max;
    if(restriction.isObject())
    {
      EsReflectedClassIntf::Ptr range = restriction.asExistingObject();
      ES_ASSERT(range);

      if(range->isKindOf(esT("EsRange")))
      {
        min = range->propertyGet(esT("min"));
        max = range->propertyGet(esT("max"));
      }
    }
    else if(restriction.isCollection())
    {
      min = restriction.itemGet(0);
      max = restriction.itemGet(1);
    }

    EsUpdateLocker<TControl> lock(m_ctl);
    if(min.isEmpty())
    {
      m_ctl->Min = DBL_MIN;
      m_ctl->Max = max.asDouble();
    }
    else if(max.isEmpty())
    {
      m_ctl->Max = DBL_MAX;
      m_ctl->Min = min.asDouble();
    }
    else
    {
      m_ctl->Min = min.asDouble();
      m_ctl->Max = max.asDouble();
    }
  }
}
//---------------------------------------------------------------------------

void EsSpinCtlDoublePropertyLink::doControlValSet(const EsVariant& val)
{
  m_ctl->Value = val.asDouble();
}
//---------------------------------------------------------------------------

EsVariant EsSpinCtlDoublePropertyLink::doControlValGet() const
{
  return static_cast<double>(m_ctl->Value);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// text control link implementation
//
__fastcall EsTextCtlPropertyLink::EsTextCtlPropertyLink(const EsString& propName,
  TCustomEdit* ctl, TLabel* lbl) :
  EsReflectedClassPropertyLink(ctl, propName),
  m_ctl(ctl),
  m_lbl(lbl)
{
  ES_ASSERT(m_ctl);
}
//---------------------------------------------------------------------------

__fastcall EsTextCtlPropertyLink::~EsTextCtlPropertyLink()
{}
//---------------------------------------------------------------------------

bool EsTextCtlPropertyLink::isUnidirectional() const
{
  return !m_ctl->Enabled || m_ctl->ReadOnly;
}
//---------------------------------------------------------------------------

void __fastcall EsTextCtlPropertyLink::onI18nStringsUpdate(TObject* sender)
{
  initialize();
}
//---------------------------------------------------------------------------

void EsTextCtlPropertyLink::initialize()
{
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(label.c_str()) + esT(":");
    m_lbl->Text = txt.c_str();
  }

  m_ctl->Hint = EsStringI18n::translate(description.c_str()).c_str();

  const EsVariant& prompt = attributeGet(esT("textPrompt"));
  if(!prompt.isEmpty())
    m_ctl->TextPrompt = EsStringI18n::translate(prompt.asString()).c_str();
}
//---------------------------------------------------------------------------

void EsTextCtlPropertyLink::doControlValSet(const EsVariant& val)
{
  m_ctl->Text = val.asString().c_str();
}
//---------------------------------------------------------------------------

EsVariant EsTextCtlPropertyLink::doControlValGet() const
{
  return m_ctl->Text.c_str();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// checkbox property link
//
__fastcall EsCheckBoxPropertyLink::EsCheckBoxPropertyLink(const EsString& propName, TCheckBox* cbx) :
  EsReflectedClassPropertyLink(cbx, propName),
  m_ctl(cbx)
{
  ES_ASSERT(m_ctl);
}
//---------------------------------------------------------------------------

__fastcall EsCheckBoxPropertyLink::~EsCheckBoxPropertyLink()
{}
//---------------------------------------------------------------------------

bool EsCheckBoxPropertyLink::isUnidirectional() const
{
  return !m_ctl->Enabled;
}
//---------------------------------------------------------------------------

void __fastcall EsCheckBoxPropertyLink::onI18nStringsUpdate(TObject* sender)
{
  initialize();
}
//---------------------------------------------------------------------------

void EsCheckBoxPropertyLink::initialize()
{
  m_ctl->Text = EsStringI18n::translate(label.c_str()).c_str();
  m_ctl->Hint = EsStringI18n::translate(description.c_str()).c_str();
}
//---------------------------------------------------------------------------

void EsCheckBoxPropertyLink::doControlValSet(const EsVariant& val)
{
  m_ctl->IsChecked = val.asBool();
}
//---------------------------------------------------------------------------

EsVariant EsCheckBoxPropertyLink::doControlValGet() const
{
  return m_ctl->IsChecked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// labelled switch property link
///
__fastcall EsSwitchPropertyLink::EsSwitchPropertyLink(const EsString& propName, TSwitch* cbx, TLabel* lbl) :
  EsReflectedClassPropertyLink(cbx, propName),
  m_ctl(cbx),
  m_lbl(lbl)
{
  ES_ASSERT(m_ctl);
}
//---------------------------------------------------------------------------

__fastcall EsSwitchPropertyLink::~EsSwitchPropertyLink()
{}
//---------------------------------------------------------------------------

bool EsSwitchPropertyLink::isUnidirectional() const
{
  return !m_ctl->Enabled;
}
//---------------------------------------------------------------------------

void __fastcall EsSwitchPropertyLink::onI18nStringsUpdate(TObject* sender)
{
  initialize();
}
//---------------------------------------------------------------------------

void EsSwitchPropertyLink::initialize()
{
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(label.c_str()) + esT(":");
    m_lbl->Text = txt.c_str();
  }

  m_ctl->Hint = EsStringI18n::translate(description.c_str()).c_str();
}
//---------------------------------------------------------------------------

void EsSwitchPropertyLink::doControlValSet(const EsVariant& val)
{
  m_ctl->IsChecked = val.asBool();
}
//---------------------------------------------------------------------------

EsVariant EsSwitchPropertyLink::doControlValGet() const
{
  return m_ctl->IsChecked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
