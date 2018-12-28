#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsReflectedClassConfigPane.h"
//---------------------------------------------------------------------------

EsReflectedClassConfigPane::EsReflectedClassConfigPane(
  wxWindow* parent, 
  const EsString& className,
	const EsMetaclassIntf::Ptr& meta /*= nullptr*/
) :
wxPanel(parent),
m_src(
  this,
  className,
  meta
),
m_applyingFromObject(false)
{
  SetSizer(
    &m_layContents
  );
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::i18nStringsUpdate(const EsString& loc)
{
  m_src.i18nStringsUpdate(loc);
}
//---------------------------------------------------------------------------

EsReflectedClassDataSource& EsReflectedClassConfigPane::sourceGet()
{
  return m_src;
}
//---------------------------------------------------------------------------

EsString EsReflectedClassConfigPane::supportedTypeNameGet() const
{
	return m_src.objectClassNameGet();
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::doApplyFromObject(const EsReflectedClassIntf::Ptr& obj)
{
	ES_ASSERT( obj );

	m_src.controlsUpdateFromObject(obj);
}
//---------------------------------------------------------------------------

struct ApplyingScope
{
  ApplyingScope(bool& flag) :
  m_flag(flag)
  {
    m_flag = true;
  }

  ~ApplyingScope()
  {
    m_flag = false;
  }

  bool& m_flag;

private:  
  ApplyingScope() ES_REMOVEDECL;
  ApplyingScope(const ApplyingScope&) ES_REMOVEDECL;
  ApplyingScope& operator=(const ApplyingScope&) ES_REMOVEDECL;
};

void EsReflectedClassConfigPane::applyFromObject(const EsReflectedClassIntf::Ptr& obj)
{
  ApplyingScope scope(m_applyingFromObject);
  doApplyFromObject(obj);
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::doApplyToObject(const EsReflectedClassIntf::Ptr& obj)
{
	ES_ASSERT( obj );

	m_src.objectUpdateFromControls(obj);
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::applyToObject(const EsReflectedClassIntf::Ptr& obj)
{
  doApplyToObject(obj);
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::sizeChangedPost(int dx, int dy)
{
  if( 
    0 == dx &&
    0 == dy
  )
    return;

  EsVariant payload(EsVariant::VAR_VARIANT_COLLECTION);
  payload.addToVariantCollection( EsVariant(this, EsVariant::ACCEPT_POINTER) )
    .addToVariantCollection(dx)
    .addToVariantCollection(dy);

  EsEventDispatcher::eventPost(
    ES_EVTC_CLASSCFG_PANE_NOTIFICATION,
    static_cast<ulong>(EsReflectedClassConfigPaneEvent::SizeChanged),
    payload
  );
}
//---------------------------------------------------------------------------
