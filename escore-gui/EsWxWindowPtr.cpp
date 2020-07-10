#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsWxWindowPtr.h"
//--------------------------------------------------------------------------------

class EsWxWindowPtrCtorAcc : public EsWxWindowPtr
{
public:
  EsWxWindowPtrCtorAcc(const wxWindowRef& ref) :
  EsWxWindowPtr(ref)
  {}
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsWxWindowPtr::EsWxWindowPtr(const wxWindowRef& ref) :
m_ref(ref)
{ 
  m_dynamic = true; //< Dynamic by design
}
//--------------------------------------------------------------------------------

EsWxWindowPtrIntf::Ptr EsWxWindowPtr::create(const wxWindowRef& ref)
{
  std::unique_ptr<EsWxWindowPtrCtorAcc> ptr = ES_MAKE_UNIQUE(
    EsWxWindowPtrCtorAcc,
    ref
  );
  ES_ASSERT(ptr);

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

EsWxWindowPtrIntf::Ptr EsWxWindowPtr::create(wxWindow* wnd)
{
  std::unique_ptr<EsWxWindowPtrCtorAcc> ptr = ES_MAKE_UNIQUE(
    EsWxWindowPtrCtorAcc,
    wxWindowRef(wnd)
  );
  ES_ASSERT(ptr);

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

const wxWindowRef& EsWxWindowPtr::windowRefGet() const
{
  return m_ref;
}
//--------------------------------------------------------------------------------

wxWindowRef& EsWxWindowPtr::windowRefGet()
{
  return m_ref;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

class EsWxHandlerPtrCtorAcc : public EsWxHandlerPtr
{
public:
  EsWxHandlerPtrCtorAcc(const wxEvtHandlerRef& ref) :
  EsWxHandlerPtr(ref)
  {}
};
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsWxHandlerPtr::EsWxHandlerPtr(const wxEvtHandlerRef& ref) :
  m_ref(ref)
{
  m_dynamic = true; //< Dynamic by design
}
//--------------------------------------------------------------------------------

EsWxHandlerPtrIntf::Ptr EsWxHandlerPtr::create(const wxEvtHandlerRef& ref)
{
  std::unique_ptr<EsWxHandlerPtrCtorAcc> ptr = ES_MAKE_UNIQUE(
    EsWxHandlerPtrCtorAcc,
    ref
  );
  ES_ASSERT(ptr);

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

EsWxHandlerPtrIntf::Ptr EsWxHandlerPtr::create(wxEvtHandler* wnd)
{
  std::unique_ptr<EsWxHandlerPtrCtorAcc> ptr = ES_MAKE_UNIQUE(
    EsWxHandlerPtrCtorAcc,
    wxEvtHandlerRef(wnd)
  );
  ES_ASSERT(ptr);

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

const wxEvtHandlerRef& EsWxHandlerPtr::handlerRefGet() const
{
  return m_ref;
}
//--------------------------------------------------------------------------------

wxEvtHandlerRef& EsWxHandlerPtr::handlerRefGet()
{
  return m_ref;
}
//--------------------------------------------------------------------------------
