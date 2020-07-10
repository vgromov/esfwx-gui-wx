#ifndef _es_wx_window_ptr_h_
#define _es_wx_window_ptr_h_

#include <wx/weakref.h>

/// Special proxy smart weak pointer declaration and implementation,
/// to pass wxWeakRef<wxWindow> around in EsVariant containers
///
ES_DECL_INTF_BEGIN(DCE184EE, 15314DE7, F1755C22, 6DE185A0, EsWxWindowPtrIntf)
  ES_DECL_INTF_METHOD(const wxWindowRef&, windowRefGet)() const = 0;
  ES_DECL_INTF_METHOD(wxWindowRef&, windowRefGet)() = 0;
ES_DECL_INTF_END

/// Special proxy smart weak pointer declaration and implementation,
/// to pass wxWeakRef<wxEvtHandler> around in EsVariant containers
///
ES_DECL_INTF_BEGIN(DCE185AE, 16384DA8, D1755F12, 6AB185AA, EsWxHandlerPtrIntf)
  ES_DECL_INTF_METHOD(const wxEvtHandlerRef&, handlerRefGet)() const = 0;
  ES_DECL_INTF_METHOD(wxEvtHandlerRef&, handlerRefGet)() = 0;
ES_DECL_INTF_END
//--------------------------------------------------------------------------------

/// Proxy wxWindow weak pointer declaration and implementation,
///
class ESCORE_GUI_CLASS ES_INTF_IMPL1(EsWxWindowPtr, EsWxWindowPtrIntf)

protected:
  EsWxWindowPtr(const wxWindowRef& ref);

public:
  /// EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }
  
  /// wxWeakRef object creation
  ///
  static EsWxWindowPtrIntf::Ptr create(const wxWindowRef& ref);
  static EsWxWindowPtrIntf::Ptr create(wxWindow* wnd);

  /// EsWxWindowPtrIntf implementation
  ///
  ES_DECL_INTF_METHOD(const wxWindowRef&, windowRefGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(wxWindowRef&, windowRefGet)() ES_OVERRIDE;

protected:
  wxWindowRef m_ref;
  
  ES_NO_DEFAULT_CTR(EsWxWindowPtr);
  ES_NON_COPYABLE(EsWxWindowPtr);
};
//--------------------------------------------------------------------------------

/// Proxy wxWindow weak pointer declaration and implementation,
///
class ESCORE_GUI_CLASS ES_INTF_IMPL1(EsWxHandlerPtr, EsWxHandlerPtrIntf)

protected:
  EsWxHandlerPtr(const wxEvtHandlerRef& ref);

public:
  /// EsBaseIntf implementation
  ///
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// wxWeakRef object creation
  ///
  static EsWxHandlerPtrIntf::Ptr create(const wxEvtHandlerRef& ref);
  static EsWxHandlerPtrIntf::Ptr create(wxEvtHandler* wnd);

  /// EsWxWindowPtrIntf implementation
  ///
  ES_DECL_INTF_METHOD(const wxEvtHandlerRef&, handlerRefGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(wxEvtHandlerRef&, handlerRefGet)() ES_OVERRIDE;

protected:
  wxEvtHandlerRef m_ref;

  ES_NO_DEFAULT_CTR(EsWxHandlerPtr);
  ES_NON_COPYABLE(EsWxHandlerPtr);
};

#endif //< _es_wx_window_ptr_h_
