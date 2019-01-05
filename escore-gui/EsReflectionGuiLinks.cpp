#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsReflectionGuiLinks.h"
//---------------------------------------------------------------------------

EsReflectedClassPropertyLink::EsReflectedClassPropertyLink(wxControl* ref, const EsString& propName) :
  m_ref(ref),
  m_onSrcSet(nullptr),
  m_onCtlChanged(nullptr),
  m_src(nullptr),
  m_propName(propName)
{
  ES_ASSERT(!m_propName.empty());
}
//---------------------------------------------------------------------------

EsReflectedClassPropertyLink::~EsReflectedClassPropertyLink()
{
  sourceSet(nullptr);
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
      m_src->linkRemove(
        shared_from_this()
      );
      m_src = nullptr;
    }

    m_src = src;

    if(m_onSrcSet)
      m_onSrcSet(*this);

    if(m_src)
    {
      m_src->linkAdd(
        shared_from_this()
      );
      // (re-) initialize stuff upon each link to source
      initialize();
      // (re-) set control to default upon each link to source
      resetControlToDefault();
    }
  }
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::controlUpdateFromObject(const EsReflectedClassIntf::Ptr& obj)
{
  if(!m_src)
    return;

  if(!obj)
    return;

  ES_ASSERT(m_src->m_className == obj->typeNameGet());
  const EsVariant& var = obj->propertyGet(m_propName);
  doControlValSet(var);

  if(m_onCtlChanged)
    m_onCtlChanged(*this);
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::doControlValueRollback(const EsVariant& val)
{
  doControlValSet(
    val
  );
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::objectUpdateFromControl(const EsReflectedClassIntf::Ptr& obj)
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
          *this,
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
      m_onCtlChanged(*this);
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

wxString EsReflectedClassPropertyLink::labelGet() const
{
  wxString result;

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

wxString EsReflectedClassPropertyLink::descriptionGet() const
{
  wxString result;

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

bool EsReflectedClassPropertyLink::visibleGet() const
{
  if(m_ref)
    return m_ref->IsShown();

  return false;
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::visibleSet(bool visible)
{
  if(m_ref)
    m_ref->Show(visible);
}
//---------------------------------------------------------------------------

bool EsReflectedClassPropertyLink::enabledGet() const
{
  if(m_ref)
    return m_ref->IsEnabled();

  return true;
}
//---------------------------------------------------------------------------

void EsReflectedClassPropertyLink::enabledSet(bool enabled)
{
  if(m_ref)
    m_ref->Enable(enabled);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsReflectedClassDataSource::EsReflectedClassDataSource(
  wxWindow* ref,
  const EsString& className, 
  const EsMetaclassIntf::Ptr& meta /*= nullptr*/
) :
m_ref(ref),
m_readOnly(false),
m_className(className)
{
  ES_ASSERT(!m_className.empty());
  metaSet(meta);
}
//---------------------------------------------------------------------------

EsReflectedClassDataSource::~EsReflectedClassDataSource()
{
  while(!m_links.empty())
  {
    EsReflectedClassPropertyLinksT::iterator it = m_links.begin();
    (*it)->sourceSet(nullptr);
  }
}
//---------------------------------------------------------------------------

EsMetaclassIntf::Ptr EsReflectedClassDataSource::metaGet() const ES_NOTHROW
{
  return m_meta;
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::metaSet(const EsMetaclassIntf::Ptr& meta)
{
  if(meta)
    m_meta = meta;
  else
    m_meta = EsMetaclass::instance();

  if(!m_meta->hasClass(m_className))
    EsException::Throw(
      esT("Meta information for class '%s' could not be found"), 
      m_className
    );
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::i18nStringsUpdate(const EsString& loc)
{
  for(auto item : m_links)
    item->i18nStringsUpdate(loc);
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::link(const EsReflectedClassPropertyLink::Ptr& link)
{
  ES_ASSERT(link);
  link->sourceSet(this);
}
//---------------------------------------------------------------------------

EsReflectedClassPropertyLink::Ptr EsReflectedClassDataSource::linkFind(const EsString& prop) const ES_NOTHROW
{
  for(auto item : m_links)
  {
    if( prop == item->propertyNameGet() )
      return item;
  }

  return nullptr;
}
//--------------------------------------------------------------------------------

void EsReflectedClassDataSource::linkAdd(const EsReflectedClassPropertyLink::Ptr& link)
{
  ES_ASSERT(link);
  m_links.insert(link);
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::linkRemove(const EsReflectedClassPropertyLink::Ptr& link)
{
  ES_ASSERT(link);
  m_links.erase(link);
}
//---------------------------------------------------------------------------

const EsString& EsReflectedClassDataSource::objectClassNameGet() const
{
  return m_className;
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::classNameCheck(const EsReflectedClassIntf::Ptr& obj) const
{
  if(m_className != obj->typeNameGet())
    EsException::ThrowIncompatibleObjectTypes(
      m_className, 
      obj->typeNameGet()
    );
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::resetControlsToDefault()
{
  for(EsReflectedClassPropertyLinksT::iterator it = m_links.begin(); it != m_links.end(); ++it)
    (*it)->resetControlToDefault();
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::controlsUpdateFromObject(const EsReflectedClassIntf::Ptr& obj)
{
  if(obj)
    classNameCheck(obj);

  for(EsReflectedClassPropertyLinksT::iterator it = m_links.begin(); it != m_links.end(); ++it)
    (*it)->controlUpdateFromObject(obj);
}
//---------------------------------------------------------------------------

void EsReflectedClassDataSource::objectUpdateFromControls(const EsReflectedClassIntf::Ptr& obj) const
{
  if(m_readOnly)
    return;

  if(obj)
  {
    classNameCheck(obj);
    for(EsReflectedClassPropertyLinksT::const_iterator it = m_links.begin(); it != m_links.end(); ++it)
      (*it)->objectUpdateFromControl(obj);
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsReflectedClassContainerDataSource::EsReflectedClassContainerDataSource(
  wxWindow* ref, 
  const EsReflectedClassIntf::Ptr& obj
) :
EsReflectedClassDataSource(
  ref, 
  obj->typeNameGet(), 
  obj->metaGet()
),
m_obj(obj)
{
}
//---------------------------------------------------------------------------

EsReflectedClassContainerDataSource::EsReflectedClassContainerDataSource(
  wxWindow* ref, 
  const EsString& className, 
  const EsMetaclassIntf::Ptr& meta
) :
EsReflectedClassDataSource(
  ref, 
  className, 
  meta
)
{
  ES_ASSERT(meta);
}
//---------------------------------------------------------------------------

EsReflectedClassContainerDataSource::~EsReflectedClassContainerDataSource()
{
  objectSet(nullptr);
  m_meta.reset();
}
//---------------------------------------------------------------------------

/// Contained object access
void EsReflectedClassContainerDataSource::objectSet(const EsReflectedClassIntf::Ptr& obj)
{
  if(obj)
  {
    if(m_className != obj->typeNameGet())
      EsException::Throw(
        esT("Could not assign incompatible object to data source. Expected '%s', got '%s'."),
        m_className, 
        obj->typeNameGet()
      );

    metaSet(obj->metaGet());
  }

  m_obj = obj;
  controlsUpdateFromObject(m_obj);
}
//---------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsReflectedClassContainerDataSource::objectGet() const
{
  if(m_obj)
    objectUpdateFromControls(m_obj);

  return m_obj;
}
//---------------------------------------------------------------------------

bool EsReflectedClassContainerDataSource::isValid() const
{
  if(m_obj)
  {
    if(m_obj->hasProperty(esT("valid")))
      return m_obj->propertyGet(esT("valid")).asBool();

    return true;
  }

  return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsComboBoxPropertyLink::EsComboBoxPropertyLink(const EsString& prop, wxComboBox* cbx, wxStaticText* lbl, bool useLookup) :
EsReflectedClassPropertyLink(
  cbx, 
  prop
),
m_cbx(cbx),
m_lbl(lbl),
m_useLookup(useLookup)
{
  ES_ASSERT(cbx);
}
//---------------------------------------------------------------------------

EsReflectedClassPropertyLink::Ptr EsComboBoxPropertyLink::create(const EsString& prop, wxComboBox* cbx, wxStaticText* lbl /*= nullptr*/, bool useLookup /*= false*/)
{
  Ptr ptr = ES_MAKE_SHARED(
    EsComboBoxPropertyLink,
    prop,
    cbx,
    lbl,
    useLookup
  );
  ES_ASSERT(ptr);

  return ptr;
}
//---------------------------------------------------------------------------

bool EsComboBoxPropertyLink::isUnidirectional() const
{
  if( m_cbx )
    return !m_cbx->IsEnabled();

  return true;
}
//---------------------------------------------------------------------------

void EsComboBoxPropertyLink::i18nStringsUpdate(const EsString& ES_UNUSED(loc))
{
  if(!m_cbx)
    return;

  initialize();
}
//---------------------------------------------------------------------------

void EsComboBoxPropertyLink::itemsPopulate()
{
  ES_ASSERT(m_cbx);
  wxWindowUpdateLocker lock(m_cbx);

  int sel = m_cbx->GetSelection(); //< Save selection
  m_cbx->Clear();

  const EsVariant& restriction = m_useLookup ?
    lookupGet() :
    restrictionGet();

  if(!restriction.isEmpty())
  {
    if(m_useLookup)
    {
      const EsVariant::Array& va = restriction.asVariantCollection();
      for(size_t idx = 0; idx < va.size(); ++idx)
      {
        const EsString& lbl = EsStringI18n::translate(
          va[idx][0].asString()
        );

        m_cbx->AppendString(
          lbl.c_str()
        );
      }
    }
    else if(restriction.isCollection())
    {
      const EsString::Array& sa = restriction.asStringCollection();
      for(auto const& item : sa)
        m_cbx->AppendString(
          item.c_str()
        );
    }
    else if(restriction.isObject())
    {
      EsReflectedClassIntf::Ptr _enum = restriction.asExistingObject();
      ES_ASSERT(_enum);
      if(_enum->isKindOf(EsEnumeration::classNameGetStatic()))
      {
        const EsString::Array& sa = _enum->propertyGet(esT("labels")).asStringCollection();
        for(auto const& item : sa)
          m_cbx->AppendString(
            item.c_str()
          );
      }
    }
  }

  if( wxNOT_FOUND != sel )
  {
    if( 0 != m_cbx->GetCount() )
      if( sel >= static_cast<int>(m_cbx->GetCount()) )
        sel = m_cbx->GetCount()-1;
    else
      sel = wxNOT_FOUND;
  }

  m_cbx->SetSelection(sel); //< Restore selection, if any
}
//---------------------------------------------------------------------------

void EsComboBoxPropertyLink::initialize()
{
  ES_ASSERT(m_src);
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(
      labelGet().wc_str()
    ) + esT(":");
    
    m_lbl->SetLabel(
      txt.c_str()
    );
  }

  ES_ASSERT(m_cbx);
  m_cbx->SetToolTip(
    EsStringI18n::translate(
      descriptionGet().wc_str()
    ).c_str()
  );

  itemsPopulate();
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

    if( m_cbx )
      m_cbx->SetSelection( itemIdx );

    return;
  }

  if(restriction.isObject())
  {
    EsReflectedClassIntf::Ptr _enum = restriction.asExistingObject();
    ES_ASSERT(_enum);
    if(_enum->isKindOf(EsEnumeration::classNameGetStatic()))
    {
      ulong idx = _enum->call(esT("valueIndexGet"), val).asULong();
      if(m_cbx)
        m_cbx->SetSelection( idx );

      return;
    }
  }

  if(!m_cbx)
    return;

  int idx = m_cbx->FindString(val.asString().c_str(), true);
  if(wxNOT_FOUND != idx)
    m_cbx->SetSelection( idx );
}
//---------------------------------------------------------------------------

EsVariant EsComboBoxPropertyLink::doControlValGet() const
{
  if(!m_cbx)
    return EsVariant::null();

  const EsVariant& restriction = m_useLookup ?
    lookupGet() :
    restrictionGet();

  if(m_useLookup && wxNOT_FOUND != m_cbx->GetSelection())
    return restriction[m_cbx->GetSelection()][1];

  if(restriction.isObject())
  {
    EsReflectedClassIntf::Ptr _enum = restriction.asExistingObject();
    ES_ASSERT(_enum);

    if(_enum->isKindOf(EsEnumeration::classNameGetStatic()))
    {
      long idx = m_cbx->GetSelection();
      ES_ASSERT(wxNOT_FOUND != idx);
      return _enum->call(
        EsStdNames::itemGet(), 
        idx
      );
    }
  }

  if(wxNOT_FOUND != m_cbx->GetSelection())
    return EsVariant(
      m_cbx->GetStringSelection().wc_str(),
      EsVariant::ACCEPT_STRING
    );

  return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsSpinCtlPropertyLink::EsSpinCtlPropertyLink(
  const EsString& propName, 
  wxSpinCtrl* ctl, 
  wxStaticText* lbl
) :
EsReflectedClassPropertyLink(
  ctl, 
  propName
),
m_ctl(ctl),
m_lbl(lbl)
{
  ES_ASSERT(m_ctl);
}
//---------------------------------------------------------------------------

EsReflectedClassPropertyLink::Ptr EsSpinCtlPropertyLink::create(const EsString& propName, wxSpinCtrl* ctl, wxStaticText* lbl /*= nullptr*/)
{
  Ptr ptr = ES_MAKE_SHARED(
    EsSpinCtlPropertyLink,
    propName,
    ctl,
    lbl
  );
  ES_ASSERT(ptr);

  return ptr;
}
//---------------------------------------------------------------------------

bool EsSpinCtlPropertyLink::isUnidirectional() const
{
  if(m_ctl)
    return !m_ctl->IsEnabled();

  return true;
}
//---------------------------------------------------------------------------

void EsSpinCtlPropertyLink::i18nStringsUpdate(const EsString& ES_UNUSED(loc))
{
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(
      labelGet().wc_str()
    ) + esT(":");

    m_lbl->SetLabel(txt.c_str());
  }

  ES_ASSERT(m_ctl);
  m_ctl->SetToolTip(
    EsStringI18n::translate(
      descriptionGet().wc_str()
    ).c_str()
  );
}
//---------------------------------------------------------------------------

void EsSpinCtlPropertyLink::initialize()
{
  i18nStringsUpdate(
    EsStringI18n::instanceGet().currentLocaleGet()    
  );

  const EsVariant& restriction = restrictionGet();
  if(!restriction.isEmpty())
  {
    EsVariant min;
    EsVariant max;
    if(restriction.isObject())
    {
      EsReflectedClassIntf::Ptr range = restriction.asExistingObject();
      ES_ASSERT(range);

      if(range->isKindOf(EsRange::classNameGetStatic()))
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

    wxWindowUpdateLocker lock(m_ctl);
    if(min.isEmpty())
      m_ctl->SetRange(
        std::numeric_limits<int>::min(),
        max.asInt()
      );
    else if(max.isEmpty())
      m_ctl->SetRange(
        min.asInt(),
        std::numeric_limits<int>::max()
      );
    else
      m_ctl->SetRange(
        min.asInt(),
        max.asInt()
      );
  }
}
//---------------------------------------------------------------------------

void EsSpinCtlPropertyLink::doControlValSet(const EsVariant& val)
{
  if(m_ctl)
    m_ctl->SetValue( val.asLong() );
}
//---------------------------------------------------------------------------

EsVariant EsSpinCtlPropertyLink::doControlValGet() const
{
  if( m_ctl )
    return static_cast<long>(m_ctl->GetValue());

  return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsSpinCtlDoublePropertyLink::EsSpinCtlDoublePropertyLink(
  const EsString& propName,
  wxSpinCtrlDouble* ctl, 
  wxStaticText* lbl
) :
EsReflectedClassPropertyLink(
  ctl, 
  propName
),
m_ctl(ctl),
m_lbl(lbl)
{
  ES_ASSERT(m_ctl);
}
//---------------------------------------------------------------------------

EsReflectedClassPropertyLink::Ptr EsSpinCtlDoublePropertyLink::create(const EsString& propName, wxSpinCtrlDouble* ctl, wxStaticText* lbl /*= nullptr*/)
{
  Ptr ptr = ES_MAKE_SHARED(
    EsSpinCtlDoublePropertyLink,
    propName,
    ctl,
    lbl
  );
  ES_ASSERT(ptr);

  return ptr;
}
//---------------------------------------------------------------------------

bool EsSpinCtlDoublePropertyLink::isUnidirectional() const
{
  if(m_ctl)
    return !m_ctl->IsEnabled();

  return true;
}
//---------------------------------------------------------------------------

void EsSpinCtlDoublePropertyLink::i18nStringsUpdate(const EsString& ES_UNUSED(loc))
{
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(
      labelGet().wc_str()
    ) + esT(":");

    m_lbl->SetLabel(txt.c_str());
  }

  ES_ASSERT(m_ctl);
  m_ctl->SetToolTip(
    EsStringI18n::translate(
      descriptionGet().wc_str()
    ).c_str()
  );
}
//---------------------------------------------------------------------------

void EsSpinCtlDoublePropertyLink::initialize()
{
  i18nStringsUpdate(
    EsStringI18n::instanceGet().currentLocaleGet()
  );

  const EsVariant& restriction = restrictionGet();
  if(!restriction.isEmpty())
  {
    EsVariant min;
    EsVariant max;
    if(restriction.isObject())
    {
      EsReflectedClassIntf::Ptr range = restriction.asExistingObject();
      ES_ASSERT(range);

      if(range->isKindOf(EsRange::classNameGetStatic()))
      {
        min = range->propertyGet(esT("min"));
        max = range->propertyGet(esT("max"));
      }
    }
    else if(restriction.isCollection())
    {
      min = restriction[0];
      max = restriction[1];
    }

    wxWindowUpdateLocker lock(m_ctl);
    if(min.isEmpty())
    {
      m_ctl->SetRange(
        std::numeric_limits<double>::min(),
        max.asDouble()
      );
    }
    else if(max.isEmpty())
    {
      m_ctl->SetRange(
        min.asDouble(),
        std::numeric_limits<double>::max()
      );
    }
    else
    {
      m_ctl->SetRange(
        min.asDouble(),
        max.asDouble()
      );
    }
  }
}
//---------------------------------------------------------------------------

void EsSpinCtlDoublePropertyLink::doControlValSet(const EsVariant& val)
{
  if( m_ctl )
    m_ctl->SetValue(
      val.asDouble()
    );
}
//---------------------------------------------------------------------------

EsVariant EsSpinCtlDoublePropertyLink::doControlValGet() const
{
  if( m_ctl )
    return m_ctl->GetValue();

  return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsTextCtlPropertyLink::EsTextCtlPropertyLink(
  const EsString& propName,
  wxTextCtrl* ctl, 
  wxStaticText* lbl
) :
EsReflectedClassPropertyLink(
  ctl, 
  propName
),
m_ctl(ctl),
m_lbl(lbl)
{
  ES_ASSERT(m_ctl);
}
//---------------------------------------------------------------------------

EsReflectedClassPropertyLink::Ptr EsTextCtlPropertyLink::create(const EsString& propName, wxTextCtrl* ctl, wxStaticText* lbl /*= nullptr*/)
{
  Ptr ptr = ES_MAKE_SHARED(
    EsTextCtlPropertyLink,
    propName,
    ctl,
    lbl
  );
  ES_ASSERT(ptr);

  return ptr;
}
//---------------------------------------------------------------------------

bool EsTextCtlPropertyLink::isUnidirectional() const
{
  if(m_ctl)
    return !m_ctl->IsEnabled();

  return true;
}
//---------------------------------------------------------------------------

void EsTextCtlPropertyLink::i18nStringsUpdate(const EsString& ES_UNUSED(loc))
{
  if(m_lbl)
  {
    const EsString& txt = EsStringI18n::translate(
      labelGet().wc_str()
    ) + esT(":");

    m_lbl->SetLabelText(
      txt.c_str()
    );
  }

  ES_ASSERT(m_ctl);
  m_ctl->SetToolTip(
    EsStringI18n::translate(
      descriptionGet().wc_str()
    ).c_str()
  );
}
//---------------------------------------------------------------------------

void EsTextCtlPropertyLink::initialize()
{
  i18nStringsUpdate(
    EsStringI18n::instanceGet().currentLocaleGet()
  );

  const EsVariant& prompt = attributeGet(esT("textPrompt"));
  if(!prompt.isEmpty())
    m_ctl->SetHint(
      EsStringI18n::translate(
        prompt.asString()
      ).c_str()
    );
}
//---------------------------------------------------------------------------

void EsTextCtlPropertyLink::doControlValSet(const EsVariant& val)
{
  if(m_ctl)
    m_ctl->SetValue( val.asString().c_str() );
}
//---------------------------------------------------------------------------

EsVariant EsTextCtlPropertyLink::doControlValGet() const
{
  if( m_ctl )
    return EsVariant(
      m_ctl->GetValue().wc_str(),
      EsVariant::ACCEPT_STRING
    );

  return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsCheckBoxPropertyLink::EsCheckBoxPropertyLink(
  const EsString& propName, 
  wxCheckBox* cbx
) :
EsReflectedClassPropertyLink(
  cbx, 
  propName
),
m_ctl(cbx)
{
  ES_ASSERT(m_ctl);
}
//---------------------------------------------------------------------------

EsReflectedClassPropertyLink::Ptr EsCheckBoxPropertyLink::create(const EsString& propName, wxCheckBox* cbx)
{
  Ptr ptr = ES_MAKE_SHARED(
    EsCheckBoxPropertyLink,
    propName,
    cbx
  );
  ES_ASSERT(ptr);

  return ptr;
}
//---------------------------------------------------------------------------

bool EsCheckBoxPropertyLink::isUnidirectional() const
{
  if( m_ctl )
    return !m_ctl->IsEnabled();

  return true;
}
//---------------------------------------------------------------------------

void EsCheckBoxPropertyLink::i18nStringsUpdate(const EsString& ES_UNUSED(loc))
{
  if(!m_ctl)
    return;

  m_ctl->SetLabel(
    EsStringI18n::translate(
      labelGet().wc_str()
    ).c_str()
  );

  m_ctl->SetToolTip(
    EsStringI18n::translate(
      descriptionGet().wc_str()
    ).c_str()
  );
}
//---------------------------------------------------------------------------

void EsCheckBoxPropertyLink::initialize()
{
  ES_ASSERT(m_ctl);

  i18nStringsUpdate(
    EsStringI18n::instanceGet().currentLocaleGet()
  );
}
//---------------------------------------------------------------------------

void EsCheckBoxPropertyLink::doControlValSet(const EsVariant& val)
{
  if( m_ctl )
    m_ctl->SetValue( val.asBool() );
}
//---------------------------------------------------------------------------

EsVariant EsCheckBoxPropertyLink::doControlValGet() const
{
  if( m_ctl )
    return EsVariant( m_ctl->GetValue() );

  return EsVariant::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
