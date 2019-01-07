#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsReflectedClassConfigPane.h"
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsReflectedClassConfigPane::
PaneWnd::PaneWnd(EsReflectedClassConfigPane& intf, wxWindow* parent) ES_NOTHROW :
wxPanel(parent),
m_intf(intf),
m_layContents(nullptr),
m_orphan(false)
{
  m_layContents = new wxFlexGridSizer(1); //< Start with single column layout
  ES_ASSERT(m_layContents);

  SetSizer(
    m_layContents
  );
}
//--------------------------------------------------------------------------------

void EsReflectedClassConfigPane::PaneWnd::sizeChangedPost(int dx, int dy)
{
  if( 
    0 == dx &&
    0 == dy
  )
    return;

  const EsVariant::Array& payload = {
    EsVariant(
      this, 
      EsVariant::ACCEPT_POINTER
    ),
    dx,
    dy
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_CLASSCFG_PANE_NOTIFICATION,
    static_cast<ulong>(EsReflectedClassConfigPaneEvent::SizeChanged),
    payload
  );
}
//--------------------------------------------------------------------------------

bool EsReflectedClassConfigPane::PaneWnd::isApplyingFromObject() const ES_NOTHROW
{
  return m_intf.m_applyingFromObject;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsReflectedClassConfigPane::EsReflectedClassConfigPane() :
m_applyingFromObject(false)
{
  m_dynamic = true;
}
//---------------------------------------------------------------------------

EsReflectedClassConfigPane::~EsReflectedClassConfigPane()
{
  m_src.reset();

  if( !m_pane )
    return;

  m_pane->m_orphan = true; //< Signal we're no longer exist
  m_pane->Hide();
  m_pane->SetParent(nullptr);
  m_pane->Close(true); //< Calling proper delayed destruction
}
//--------------------------------------------------------------------------------

void EsReflectedClassConfigPane::init(wxWindow* parent, const EsString& className, const EsMetaclassIntf::Ptr& meta)
{
  if( !m_pane )
  {
    m_pane = doPaneWndCreate(parent);
    ES_ASSERT(m_pane);
  }
  else
    m_pane->SetParent(parent);

  m_src.reset(
    doDataSourceCreate(
      className, 
      meta
    )
  );

  m_pane->Layout();
}
//--------------------------------------------------------------------------------

void EsReflectedClassConfigPane::i18nStringsUpdate(const EsString& loc)
{
  ES_ASSERT(m_src);
  m_src->i18nStringsUpdate(loc);
}
//---------------------------------------------------------------------------

EsString EsReflectedClassConfigPane::supportedTypeNameGet() const ES_NOTHROW
{
  ES_ASSERT(m_src);
	return m_src->objectClassNameGet();
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::doApplyFromObject(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT( m_src );
	ES_ASSERT( obj );

	m_src->controlsUpdateFromObject(obj);
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::doApplyToObject(const EsReflectedClassIntf::Ptr& obj)
{
  ES_ASSERT( m_src );
	ES_ASSERT( obj );

	m_src->objectUpdateFromControls(obj);
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

void EsReflectedClassConfigPane::controlsUpdateFromObject(const EsVariant& obj)
{
  ApplyingScope scope(m_applyingFromObject);

  doApplyFromObject(obj.asObject());
}
//---------------------------------------------------------------------------

void EsReflectedClassConfigPane::objectUpdateFromControls(const EsVariant& obj)
{
  doApplyToObject(
    obj.asExistingObject()
  );
}
//--------------------------------------------------------------------------------

void EsReflectedClassConfigPane::controlsResetToDefault()
{
  if( m_src )
	  m_src->resetControlsToDefault();	
}
//--------------------------------------------------------------------------------
