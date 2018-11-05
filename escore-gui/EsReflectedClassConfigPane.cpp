#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsReflectedClassConfigPane.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"

//---------------------------------------------------------------------------
__fastcall EsReflectedClassConfigPane::EsReflectedClassConfigPane(TComponent* Owner, const EsString& className,
								const EsMetaclassIntf::Ptr& meta /*= EsMetaclassIntf::Ptr()*/) :
TFrame(Owner),
m_i18nScope(this),
m_src(0)
{
	m_src = new EsReflectedClassDataSource(this, className, meta);
	ES_ASSERT(m_src);
}
//---------------------------------------------------------------------------

__fastcall EsReflectedClassConfigPane::~EsReflectedClassConfigPane()
{}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::i18nMgrSet(PlainEsI18nManagerPtrT mgr)
{
  m_i18nScope.managerSet(mgr);
}
//---------------------------------------------------------------------------

EsReflectedClassDataSource& EsReflectedClassConfigPane::sourceGet()
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

void EsReflectedClassConfigPane::applyFromObject(const EsReflectedClassIntf::Ptr& obj)
{
	ES_ASSERT( obj );
	ES_ASSERT( m_src );

	m_src->updateControls(obj);
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::applyToObject(const EsReflectedClassIntf::Ptr& obj)
{
	ES_ASSERT( obj );
	ES_ASSERT( m_src );

	m_src->updateObject(obj);
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::sizeChangedPost(double dx, double dy)
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
