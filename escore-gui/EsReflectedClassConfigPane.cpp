#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsReflectedClassConfigPane.h"
//---------------------------------------------------------------------------
__fastcall EsReflectedClassConfigPane::EsReflectedClassConfigPane(TComponent* Owner, const EsString& className,
								const EsMetaclassIntf::Ptr& meta /*= nullptr*/) :
TFrame(Owner),
m_i18nScope(this),
m_src(nullptr),
m_applyingFromObject(false)
{
	m_src = new EsReflectedClassDataSource(this, className, meta);
	ES_ASSERT(m_src);
}
//---------------------------------------------------------------------------

EsReflectedClassConfigPane::~EsReflectedClassConfigPane()
{}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::i18nMgrSet(PlainEsI18nManagerPtrT mgr)
{
  m_i18nScope.managerSet(mgr);
  ES_ASSERT(m_src);

  m_i18nScope.onGuiStringsUpdate = onGuiStringsUpdate;
  m_src->i18nInit();
}
//---------------------------------------------------------------------------

void TFrmReflectedClassConfig::i18nStringsUpdate(const EsString& loc)
{
  // Do nothing by default
}
//---------------------------------------------------------------------------

void TFrmReflectedClassConfig::onGuiStringsUpdate(TObject* ES_UNUSED(sender))
{
  i18nStringsUpdate(
    m_i18nScope.managerGet()->currentLocaleGet()
  );
}
//---------------------------------------------------------------------------

EsReflectedClassDataSource& TFrmReflectedClassConfig::sourceGet()
{
  return *m_src;
}
//---------------------------------------------------------------------------

EsString EsReflectedClassConfigPane::supportedTypeNameGet() const
{
	ES_ASSERT(m_src);
	return m_src->classNameGet();
}
//---------------------------------------------------------------------------

void TFrmReflectedClassConfig::doApplyFromObject(const EsReflectedClassIntf::Ptr& obj)
{
	ES_ASSERT( obj );
	ES_ASSERT( m_src );

	m_src->updateControls(obj);
}
//---------------------------------------------------------------------------

void TFrmReflectedClassConfig::applyFromObject(const EsReflectedClassIntf::Ptr& obj)
{
  try
  {
    m_applyingFromObject = true;

    doApplyFromObject(obj);
  }
  __finally
  {
    m_applyingFromObject = false;
  }
}
//---------------------------------------------------------------------------

void TFrmReflectedClassConfig::doApplyToObject(const EsReflectedClassIntf::Ptr& obj)
{
	ES_ASSERT( obj );
	ES_ASSERT( m_src );

	m_src->updateObject(obj);
}
//---------------------------------------------------------------------------

void TFrmReflectedClassConfig::applyToObject(const EsReflectedClassIntf::Ptr& obj)
{
  doApplyToObject(obj);
}
//---------------------------------------------------------------------------

void TFrmReflectedClassConfig::sizeChangedPost(double dx, double dy)
{
  if( EsUtilities::areEqualFloats(0, dx) &&
      EsUtilities::areEqualFloats(0, dy)
  )
    return;

  EsVariant payload(EsVariant::VAR_VARIANT_COLLECTION);
  payload.addToVariantCollection( EsVariant(this, EsVariant::ACCEPT_POINTER) )
    .addToVariantCollection(dx)
    .addToVariantCollection(dy);

  EsEventDispatcher::eventPost(
    EVTC_CLASSCFG_PANE_NOTIFICATION,
    static_cast<ulong>(EsReflectedClassConfigPaneEvent::SizeChanged),
    payload
  );
}
//---------------------------------------------------------------------------
