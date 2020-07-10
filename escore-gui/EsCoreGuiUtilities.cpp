#include "escore_gui_pch.h"
#pragma hdrstop

#include <wx/wupdlock.h>
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/aui/aui.h>
#if wxUSE_STREAMS
# include "wx/stream.h"
# include "wx/txtstrm.h"
#endif

#include "EsCoreGuiUtilities.h"
//--------------------------------------------------------------------------------

class wxVariantDataBinBuffer : public wxVariantData
{
public:
  wxVariantDataBinBuffer() {}
  wxVariantDataBinBuffer(const EsBinBuffer& value) : m_value(value) { }

  const EsBinBuffer& GetValue() const { return m_value; }
  void SetValue(const EsBinBuffer& value) { m_value = value; }

  virtual bool Eq(wxVariantData& data) const wxOVERRIDE;

#if wxUSE_STD_IOSTREAM
  virtual bool Write(wxSTD ostream& str) const wxOVERRIDE;
#endif

  virtual bool Write(wxString& str) const wxOVERRIDE;

#if wxUSE_STD_IOSTREAM
  virtual bool Read(wxSTD istream& str) wxOVERRIDE;
#endif

  virtual bool Read(wxString& str) wxOVERRIDE;
  
  virtual wxString GetType() const wxOVERRIDE { return wxT("esbinbuffer"); }
  virtual wxVariantData* Clone() const wxOVERRIDE { return new wxVariantDataBinBuffer(m_value); }

  DECLARE_WXANY_CONVERSION()

protected:
  EsBinBuffer m_value;
};
//--------------------------------------------------------------------------------

IMPLEMENT_TRIVIAL_WXANY_CONVERSION(EsBinBuffer, wxVariantDataBinBuffer)
//--------------------------------------------------------------------------------

bool wxVariantDataBinBuffer::Eq(wxVariantData& data) const
{
  wxASSERT_MSG( data.GetType() == GetType(), wxT("wxVariantDataBinBuffer::Eq: argument mismatch") );

  wxVariantDataBinBuffer& otherData = (wxVariantDataBinBuffer&)data;

  return otherData.m_value == m_value;
}
//--------------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
bool wxVariantDataBinBuffer::Write(wxSTD ostream& str) const
{
  str << static_cast<uint32_t>( m_value.size() );
  str.write(
    reinterpret_cast<const wxSTD ostream::char_type*>(m_value.data()),
    m_value.size()
  );

  return !str.bad();
}
#endif
//--------------------------------------------------------------------------------

bool wxVariantDataBinBuffer::Write(wxString& str) const
{
  str = EsString::binToHex( m_value ).as<wxString>();

  return true;
}
//--------------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
bool wxVariantDataBinBuffer::Read(wxSTD istream& str)
{
  uint32_t len = 0;
  str >> len;
  m_value.resize(len);

  if( str.fail() )
    return false;

  str.read(
    reinterpret_cast<wxSTD istream::char_type*>(m_value.data()),
    m_value.size()
  );

  return !str.bad();
}
#endif
//--------------------------------------------------------------------------------

bool wxVariantDataBinBuffer::Read(wxString& str)
{
  const EsBinBuffer& bb = EsString::hexToBin(
    str.t_str()
  );
  m_value = bb;

  return true;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

class wxVariantDataByte : public wxVariantData
{
public:
  wxVariantDataByte() { m_value = 0; }
  wxVariantDataByte(esU8 value) { m_value = value; }

  inline esU8 GetValue() const { return m_value; }
  inline void SetValue(esU8 value) { m_value = value; }

  virtual bool Eq(wxVariantData& data) const wxOVERRIDE;

#if wxUSE_STD_IOSTREAM
  virtual bool Write(wxSTD ostream& str) const wxOVERRIDE;
#endif

  virtual bool Write(wxString& str) const wxOVERRIDE;

  virtual bool Read(wxString& str) wxOVERRIDE;

#if wxUSE_STD_IOSTREAM
  virtual bool Read(wxSTD istream& str) wxOVERRIDE;
#endif

#if wxUSE_STREAMS
  virtual bool Read(wxInputStream& str);

  virtual bool Write(wxOutputStream& str) const;

#endif // wxUSE_STREAMS

  virtual wxString GetType() const wxOVERRIDE { return wxT("esbyte"); }

  wxVariantData* Clone() const wxOVERRIDE { return new wxVariantDataByte(m_value); }

  DECLARE_WXANY_CONVERSION()

protected:
  esU8 m_value;
};
//--------------------------------------------------------------------------------

IMPLEMENT_TRIVIAL_WXANY_CONVERSION(esU8, wxVariantDataByte)
//--------------------------------------------------------------------------------

bool wxVariantDataByte::Eq(wxVariantData& data) const
{
  wxASSERT_MSG( (data.GetType() == wxT("esbyte")), wxT("wxVariantDataByte::Eq: argument mismatch") );

  wxVariantDataByte& otherData = (wxVariantDataByte&) data;

  return (otherData.m_value == m_value);
}
//--------------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
bool wxVariantDataByte::Write(wxSTD ostream& str) const
{
  str << m_value;

  return !str.fail();
}
#endif
//--------------------------------------------------------------------------------

bool wxVariantDataByte::Write(wxString& str) const
{
  str = EsString::fromULong( m_value ).as<wxString>();
  return true;
}
//--------------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
bool wxVariantDataByte::Read(wxSTD istream& str)
{
  str >> m_value;

  return !str.fail();
}
#endif
//--------------------------------------------------------------------------------

#if wxUSE_STREAMS
bool wxVariantDataByte::Write(wxOutputStream& str) const
{
  wxTextOutputStream s(str);

  s.Write8(m_value);
  return true;
}
//--------------------------------------------------------------------------------

bool wxVariantDataByte::Read(wxInputStream& str)
{
  wxTextInputStream s(str);

  m_value = s.Read8();
  return true;
}
#endif // wxUSE_STREAMS
//--------------------------------------------------------------------------------

bool wxVariantDataByte::Read(wxString& str)
{
  m_value = static_cast<esU8>( wxAtol(str) );
  return true;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

class wxVariantDataUint : public wxVariantData
{
public:
  wxVariantDataUint() { m_value = 0; }
  wxVariantDataUint(esU32 value) { m_value = value; }

  inline esU32 GetValue() const { return m_value; }
  inline void SetValue(esU32 value) { m_value = value; }

  virtual bool Eq(wxVariantData& data) const wxOVERRIDE;

#if wxUSE_STD_IOSTREAM
  virtual bool Write(wxSTD ostream& str) const wxOVERRIDE;
#endif

  virtual bool Write(wxString& str) const wxOVERRIDE;

  virtual bool Read(wxString& str) wxOVERRIDE;

#if wxUSE_STD_IOSTREAM
  virtual bool Read(wxSTD istream& str) wxOVERRIDE;
#endif

#if wxUSE_STREAMS
  virtual bool Read(wxInputStream& str);

  virtual bool Write(wxOutputStream& str) const;

#endif // wxUSE_STREAMS

  virtual wxString GetType() const wxOVERRIDE { return wxT("esuint"); }

  wxVariantData* Clone() const wxOVERRIDE { return new wxVariantDataUint(m_value); }

  DECLARE_WXANY_CONVERSION()

protected:
  esU32 m_value;
};
//--------------------------------------------------------------------------------

IMPLEMENT_TRIVIAL_WXANY_CONVERSION(esU8, wxVariantDataUint)
//--------------------------------------------------------------------------------

bool wxVariantDataUint::Eq(wxVariantData& data) const
{
  wxASSERT_MSG( (data.GetType() == wxT("esuint")), wxT("wxVariantDataUint::Eq: argument mismatch") );

  wxVariantDataUint& otherData = (wxVariantDataUint&) data;

  return (otherData.m_value == m_value);
}
//--------------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
bool wxVariantDataUint::Write(wxSTD ostream& str) const
{
  str << m_value;

  return !str.fail();
}
#endif
//--------------------------------------------------------------------------------

bool wxVariantDataUint::Write(wxString& str) const
{
  str = EsString::fromULong( m_value ).as<wxString>();
  return true;
}
//--------------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
bool wxVariantDataUint::Read(wxSTD istream& str)
{
  str >> m_value;

  return !str.fail();
}
#endif
//--------------------------------------------------------------------------------

#if wxUSE_STREAMS
bool wxVariantDataUint::Write(wxOutputStream& str) const
{
  wxTextOutputStream s(str);

  s.Write32(m_value);
  return true;
}
//--------------------------------------------------------------------------------

bool wxVariantDataUint::Read(wxInputStream& str)
{
  wxTextInputStream s(str);

  m_value = s.Read32();
  return true;
}
#endif // wxUSE_STREAMS
//--------------------------------------------------------------------------------

bool wxVariantDataUint::Read(wxString& str)
{
  m_value = static_cast<esU32>( wxAtol(str) );
  return true;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

class wxVariantDataInt : public wxVariantData
{
public:
  wxVariantDataInt() { m_value = 0; }
  wxVariantDataInt(esI32 value) { m_value = value; }

  inline esI32 GetValue() const { return m_value; }
  inline void SetValue(esI32 value) { m_value = value; }

  virtual bool Eq(wxVariantData& data) const wxOVERRIDE;

#if wxUSE_STD_IOSTREAM
  virtual bool Write(wxSTD ostream& str) const wxOVERRIDE;
#endif

  virtual bool Write(wxString& str) const wxOVERRIDE;

  virtual bool Read(wxString& str) wxOVERRIDE;

#if wxUSE_STD_IOSTREAM
  virtual bool Read(wxSTD istream& str) wxOVERRIDE;
#endif

#if wxUSE_STREAMS
  virtual bool Read(wxInputStream& str);

  virtual bool Write(wxOutputStream& str) const;

#endif // wxUSE_STREAMS

  virtual wxString GetType() const wxOVERRIDE { return wxT("esint"); }

  wxVariantData* Clone() const wxOVERRIDE { return new wxVariantDataInt(m_value); }

  DECLARE_WXANY_CONVERSION()

protected:
  esI32 m_value;
};
//--------------------------------------------------------------------------------

IMPLEMENT_TRIVIAL_WXANY_CONVERSION(esI32, wxVariantDataInt)
//--------------------------------------------------------------------------------

bool wxVariantDataInt::Eq(wxVariantData& data) const
{
  wxASSERT_MSG( (data.GetType() == wxT("esint")), wxT("wxVariantDataInt::Eq: argument mismatch") );

  wxVariantDataInt& otherData = (wxVariantDataInt&) data;

  return (otherData.m_value == m_value);
}
//--------------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
bool wxVariantDataInt::Write(wxSTD ostream& str) const
{
  str << m_value;

  return !str.fail();
}
//--------------------------------------------------------------------------------

bool wxVariantDataInt::Read(wxSTD istream& str)
{
  str >> m_value;

  return !str.fail();
}
#endif
//--------------------------------------------------------------------------------

bool wxVariantDataInt::Write(wxString& str) const
{
  str = EsString::fromLong( m_value ).as<wxString>();
  return true;
}
//--------------------------------------------------------------------------------

bool wxVariantDataInt::Read(wxString& str)
{
  m_value = static_cast<esI32>( wxAtol(str) );
  return true;
}
//--------------------------------------------------------------------------------

#if wxUSE_STREAMS
bool wxVariantDataInt::Write(wxOutputStream& str) const
{
  wxTextOutputStream s(str);

  s.Write32(m_value);
  return true;
}
//--------------------------------------------------------------------------------

bool wxVariantDataInt::Read(wxInputStream& str)
{
  wxTextInputStream s(str);

  m_value = s.Read32S();
  return true;
}
#endif // wxUSE_STREAMS
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

class wxVariantDataEsObject : public wxVariantData
{
public:
  wxVariantDataEsObject() {}
  wxVariantDataEsObject(const EsBaseIntfPtr& value) { m_value = value; }

  inline EsBaseIntfPtr GetValue() const { return m_value; }
  inline void SetValue(const EsBaseIntfPtr& value) { m_value = value; }

  virtual bool Eq(wxVariantData& data) const wxOVERRIDE;

#if wxUSE_STD_IOSTREAM
  virtual bool Write(wxSTD ostream& str) const wxOVERRIDE;
  virtual bool Read(wxSTD istream& str) wxOVERRIDE;
#endif

  virtual bool Write(wxString& str) const wxOVERRIDE;
  virtual bool Read(wxString& str) wxOVERRIDE;

#if wxUSE_STREAMS
  virtual bool Read(wxInputStream& str);
  virtual bool Write(wxOutputStream& str) const;
#endif // wxUSE_STREAMS

  virtual wxString GetType() const wxOVERRIDE { return wxT("esobject"); }

  wxVariantData* Clone() const wxOVERRIDE { return new wxVariantDataEsObject(m_value); }

  DECLARE_WXANY_CONVERSION()

protected:
  EsBaseIntfPtr m_value;
};
//--------------------------------------------------------------------------------

IMPLEMENT_TRIVIAL_WXANY_CONVERSION(EsBaseIntfPtr, wxVariantDataEsObject)
//--------------------------------------------------------------------------------

bool wxVariantDataEsObject::Eq(wxVariantData& data) const
{
  wxASSERT_MSG( (data.GetType() == wxT("esobject")), wxT("wxVariantDataEsObject::Eq: argument mismatch") );

  wxVariantDataEsObject& otherData = (wxVariantDataEsObject&) data;

  return (otherData.m_value == m_value);
}
//--------------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
bool wxVariantDataEsObject::Write(wxSTD ostream& ES_UNUSED(str)) const
{
  return false;
}
//--------------------------------------------------------------------------------

bool wxVariantDataEsObject::Read(wxSTD istream& ES_UNUSED(str))
{
  return false;
}
#endif
//--------------------------------------------------------------------------------

bool wxVariantDataEsObject::Write(wxString& ES_UNUSED(str)) const
{
  return false;
}
//--------------------------------------------------------------------------------

bool wxVariantDataEsObject::Read(wxString& ES_UNUSED(str))
{
  return false;
}
//--------------------------------------------------------------------------------

#if wxUSE_STREAMS
bool wxVariantDataEsObject::Write(wxOutputStream& ES_UNUSED(str)) const
{
  return false;
}
//--------------------------------------------------------------------------------

bool wxVariantDataEsObject::Read(wxInputStream& str)
{
  return false;
}
#endif // wxUSE_STREAMS
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void EsUtilities::wxVariantFromEsVariant(wxVariant& wxvar, const EsVariant& var)
{
  switch( var.typeGet() )
  {
  case EsVariant::VAR_EMPTY:
    wxvar.MakeNull();
    wxvar.Clear();
    break;
  case EsVariant::VAR_BOOL:
    wxvar = var.doInterpretAsBool();
    break;
  case EsVariant::VAR_BYTE:
    wxvar.SetData(
      new wxVariantDataByte(
        var.doInterpretAsByte()
      )
    );
    break;
  case EsVariant::VAR_CHAR:
    wxvar = var.doInterpretAsChar();
    break;
  case EsVariant::VAR_UINT:
    wxvar.SetData(
      new wxVariantDataUint(
        var.doInterpretAsUInt() 
      )
    );
    break;
  case EsVariant::VAR_INT:
    wxvar.SetData(
      new wxVariantDataInt(
        var.doInterpretAsInt()      
      )
    );
    break;
  case EsVariant::VAR_UINT64:
    wxvar = static_cast<wxULongLong>( var.asULLong() );
    break;
  case EsVariant::VAR_INT64:
    wxvar = static_cast<wxLongLong>( var.asLLong() );
    break;
  case EsVariant::VAR_DOUBLE:
    wxvar = var.asDouble();
    break;
  case EsVariant::VAR_BIN_BUFFER:
    wxvar.SetData(
      new wxVariantDataBinBuffer(
        var.doInterpretAsBinBuffer()
      )
    );
    break;
  case EsVariant::VAR_STRING:
    wxvar = static_cast<wxString>(
      var.doInterpretAsString().c_str()
    );
    break;
  case EsVariant::VAR_STRING_COLLECTION:
    wxvar = EsUtilities::esStringArrayToWxArrayString(
      var.doInterpretAsStringCollection()
    );
    break;
  case EsVariant::VAR_POINTER:
    wxvar = var.asPointer();
    break;
  case EsVariant::VAR_OBJECT:
    wxvar.SetData(
      new wxVariantDataEsObject(
        var.doInterpretAsObject()
      )
    );
    break;
  case EsVariant::VAR_VARIANT_COLLECTION:
    wxvar.NullList();
    for(auto const& item : var.doInterpretAsVariantCollection() )
      wxvar.Append(
        EsUtilities::wxVariantFromEsVariant(
          item
        )
      );
    break;
  }
}
//--------------------------------------------------------------------------------

wxVariant EsUtilities::wxVariantFromEsVariant(const EsVariant& var)
{
  wxVariant wxvar;
  EsUtilities::wxVariantFromEsVariant(
    wxvar,
    var
  );

  return wxvar;
}
//--------------------------------------------------------------------------------

template <typename wxVariantData_T>
const wxVariantData_T& variantDataAccess(const wxVariant& wxvar)
{
  const wxVariantData_T* dataptr = dynamic_cast<const wxVariantData_T*>( wxvar.GetData() );
  ES_ASSERT(dataptr);

  return *dataptr;
}
//--------------------------------------------------------------------------------

void EsUtilities::esVariantFromWxVariant(EsVariant& var, const wxVariant& wxvar)
{
  if( wxvar.IsNull() )
  {
    var.setEmpty();
    return;
  }

  const wxString& type = wxvar.GetType();
  if( "bool" == type )
    var = wxvar.GetBool();
  else if( "esbyte" == type )
    var = variantDataAccess<wxVariantDataByte>(wxvar).GetValue();
  else if( "char" == type )
    var = static_cast<ES_CHAR>(wxvar.GetChar());
  else if( "esuint" == type )
    var = variantDataAccess<wxVariantDataUint>(wxvar).GetValue();
  else if( "esint" == type )
    var = variantDataAccess<wxVariantDataInt>(wxvar).GetValue();
  else if( "long" == type )
    var = wxvar.GetLong();
  else if( "ulonglong" == type )
    var = wxvar.GetULongLong().GetValue();
  else if( "longlong" == type )
    var = wxvar.GetLongLong().GetValue();
  else if( "double" == type )
    var = wxvar.GetDouble();
  else if( "esbinbuffer" == type )
    var.assignBinBuffer(
      variantDataAccess<wxVariantDataBinBuffer>(wxvar).GetValue()
    );
  else if( "string" == type )
  {
    const wxString& str = wxvar.GetString();
    auto stdstr = str.tchar_str<ES_CHAR>();

    var.assignString(
      stdstr.data(),
      stdstr.length()
    );
  }
  else if( "arrstring" == type )
    var = EsUtilities::wxArrayStringToEsStringArray(
      wxvar.GetArrayString()
    );
  else if( "void*" == type )
    var = wxvar.GetVoidPtr();
  else if( "esobject" == type )
    var = variantDataAccess<wxVariantDataEsObject>(wxvar).GetValue();
  else if( "list" == type )
  {
    EsVariant::Array va;
    const wxVariantList& list = wxvar.GetList();
    size_t cnt = list.GetCount();
    va.reserve( cnt );

    for( size_t idx = 0; idx < cnt; ++idx )
    {
      const wxwxVariantListNode* item = list.Item(idx);
      ES_ASSERT(item);

      const wxVariant* data = item->GetData();
      ES_ASSERT(data);

      va.push_back(
        EsUtilities::esVariantFromWxVariant( *data )
      );
    }

    var = va;
  }
}
//--------------------------------------------------------------------------------

EsVariant EsUtilities::esVariantFromWxVariant(const wxVariant& wxvar)
{
  EsVariant var;

  EsUtilities::esVariantFromWxVariant(
    var,
    wxvar
  );

  return var;
}
//--------------------------------------------------------------------------------

const wxString& EsUtilities::wxSpaceString()
{
  static const wxString sc_space(" ");
  return sc_space;
}
//--------------------------------------------------------------------------------

void EsUtilities::wxArrayStringToEsStringArray(EsString::Array& dest, const wxArrayString& src)
{
  dest.clear();
  dest.reserve(src.size());

  for( auto const& item : src )
    dest.push_back(
      item.t_str()
    );
}
//--------------------------------------------------------------------------------

EsString::Array EsUtilities::wxArrayStringToEsStringArray(const wxArrayString& src)
{
  EsString::Array result;
  wxArrayStringToEsStringArray(
    result,
    src
  );

  return result;
}
//--------------------------------------------------------------------------------

void EsUtilities::esStringArrayToWxArrayString(wxArrayString& dest, const EsString::Array& src)
{
  dest.clear();
  dest.reserve(src.size());
  for( auto const& item : src )
    dest.push_back(
      item.c_str()
    );
}
//--------------------------------------------------------------------------------

wxArrayString EsUtilities::esStringArrayToWxArrayString(const EsString::Array& src)
{
  wxArrayString result;
  esStringArrayToWxArrayString(
    result,
    src
  );

  return result;
}
//--------------------------------------------------------------------------------

void EsUtilities::mruToEsStringArray(EsString::Array& dest, const wxFileHistory& src)
{
  dest.clear();
  dest.reserve(src.GetCount());

  for( size_t idx = 0; idx < src.GetCount(); ++idx )
    dest.push_back(
      src.GetHistoryFile(idx).t_str()
    );
}
//--------------------------------------------------------------------------------

void EsUtilities::esStringArrayToMru(wxFileHistory& dest, const EsString::Array& src)
{
  while(dest.GetCount())
    dest.RemoveFileFromHistory(0);

  for( size_t idx = 0; idx < src.size(); ++idx )
    dest.AddFileToHistory(
      src[idx].c_str()
    );
}
//--------------------------------------------------------------------------------

wxDateTime EsUtilities::esDateTimeToWxDateTime(const EsDateTime& dt)
{
	// NB. wxDateTime uses 0-based month values
	return wxDateTime( 
    dt.get_dayOfMonth(), 
    static_cast<wxDateTime::Month>(dt.get_month()-1), 
    dt.get_year(), 
    dt.get_hours(), 
    dt.get_minutes(), 
    dt.get_seconds(), 
    dt.get_milliseconds()
  );
}
//--------------------------------------------------------------------------------

EsDateTime EsUtilities::wxDateTimeToEsDateTime(const wxDateTime& dt)
{
	wxDateTime::Tm tm = dt.GetTm();
	// NB. wxDateTime uses 0-based month values
	EsDateTime result;
  result.compose(
    tm.year, 
    static_cast<int>(tm.mon)+1, 
    tm.mday, 
    tm.hour, 
    tm.min, 
    tm.sec, 
    tm.msec
  );

	return result;
}
//--------------------------------------------------------------------------------

void EsUtilities::wxDateTimeTimeToEsDateTime(EsDateTime& dest, const wxDateTime& dt)
{
  dest.set_hours( dt.GetHour() );
  dest.set_minutes( dt.GetMinute() );
  dest.set_seconds( dt.GetSecond() );
  dest.set_milliseconds( dt.GetMillisecond() );
}
//--------------------------------------------------------------------------------

void EsUtilities::windowPosSave(const EsStreamIntf::Ptr& out, const wxPoint& pos)
{
  ES_ASSERT(out);

  out->valueWrite(
    esT("LEFT"),
    pos.x
  );

  out->valueWrite(
    esT("TOP"),
    pos.y
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::windowPosSave(const EsStreamIntf::Ptr& out, const wxWindow* wnd)
{
  ES_ASSERT(out);
  ES_ASSERT(wnd);

  const wxRect& rect = wnd->GetRect();

  windowPosSave(
    out,
    rect.GetTopLeft()
  );

  out->valueWrite(
    esT("WIDTH"),
    rect.GetWidth()
  );

  out->valueWrite(
    esT("HEIGHT"),
    rect.GetHeight()
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::windowPosLoad(const EsStreamIntf::Ptr& in, wxPoint& pos, const wxPoint& defaultPos)
{
  ES_ASSERT(in);

  // form pos & size
  if(in->valueExists(esT("LEFT")))
  {
    pos.x = in->valueRead(
      esT("LEFT"),
      defaultPos.x
    ).asInt();

    if(in->valueExists(esT("TOP")))
    {
      pos.y = in->valueRead(
        esT("TOP"),
        defaultPos.y
      ).asInt();
    }
  }
}
//--------------------------------------------------------------------------------

void EsUtilities::windowPosLoad(const EsStreamIntf::Ptr& in, wxWindow* wnd, const wxSize& minSize)
{
  ES_ASSERT(in);
  ES_ASSERT(wnd);

  // form pos & size
  wxPoint pos = wnd->GetPosition();
  windowPosLoad(
    in,
    pos,
    wxPoint(0, 0)
  );

  wxSize sze = wnd->GetSize();
  if(in->valueExists(esT("WIDTH")))
  {
    sze.x = in->valueRead(
      esT("WIDTH"),
      minSize.x
    ).asInt();

    if(in->valueExists(esT("HEIGHT")))
    {
      sze.y = in->valueRead(
        esT("HEIGHT"),
        minSize.y
      ).asInt();
    }
  }

  pos.x = wxMax(0, pos.x);
  pos.y = wxMax(0, pos.y);
  sze.x = wxMax(minSize.x, sze.x);
  sze.y = wxMax(minSize.y, sze.y);

  int x, y;
  wxDisplaySize( &x, &y );

  if( pos.x > x ) //< Allow at least some portion of the window to be shown on visible part of screen
    pos.x = x - 100;

  if( pos.y > y )
    pos.y = y - 100;

  wnd->SetPosition(pos);
  wnd->SetSize(sze);
}
//--------------------------------------------------------------------------------

void EsUtilities::windowStateSave(const EsStreamIntf::Ptr& out, const wxFrame* wnd)
{
  ES_ASSERT(out);
  ES_ASSERT(wnd);

  EsString state = esT("normal");
  if( wnd->IsMaximized() )
    state = esT("maximized");
  else if( wnd->IsIconized() )
    state = esT("iconized");
  else if( wnd->IsFullScreen() )
    state = esT("fullScreen");

  out->valueWrite(
    esT("STATE"),
    state
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::windowStateLoad(const EsStreamIntf::Ptr& in, wxFrame* wnd, long fullScreenStyle /*= wxFULLSCREEN_ALL*/)
{
  ES_ASSERT(in);
  ES_ASSERT(wnd);

  const EsString& state = in->valueRead(
      esT("STATE"),
      esVT("normal")
    ).asString();

  if( esT("maximized") == state )
    wnd->Maximize();
  else if( esT("iconized") == state )
    wnd->Iconize();
  else if( esT("fullScreen") == state )
    wnd->ShowFullScreen(
      true,
      fullScreenStyle
    );
  else
    wnd->Maximize(false);
}
//--------------------------------------------------------------------------------

// Standard menu bitmaps
//
#include "res/menu-check-checked.xpm"
#include "res/menu-check-unchecked.xpm"
#include "res/menu-radio-checked.xpm"
#include "res/menu-radio-unchecked.xpm"

const wxBitmap& EsUtilities::stdBitmapGet(EsStdBitmapId id)
{
  static std::map<EsStdBitmapId, wxBitmap> s_stock;

  auto cit = s_stock.find(id);
  if(s_stock.end() != cit)
    return (*cit).second;

  if(EsStdBitmapId::Null != id)
  {
    switch(id)
    {
    case EsStdBitmapId::CheckChecked:
      return s_stock.insert(
        std::make_pair(
          EsStdBitmapId::CheckChecked,
          wxBitmap(menu_check_checked_xpm, wxBITMAP_TYPE_XPM)
        )
      ).first->second;
    case EsStdBitmapId::CheckUnchecked:
      return s_stock.insert(
        std::make_pair(
          EsStdBitmapId::CheckUnchecked,
          wxBitmap(menu_check_unchecked_xpm, wxBITMAP_TYPE_XPM)
        )
      ).first->second;
    case EsStdBitmapId::RadioChecked:
      return s_stock.insert(
        std::make_pair(
          EsStdBitmapId::RadioChecked,
          wxBitmap(menu_check_unchecked_xpm, wxBITMAP_TYPE_XPM)
        )
      ).first->second;
    case EsStdBitmapId::RadioUnchecked:
      return s_stock.insert(
        std::make_pair(
          EsStdBitmapId::RadioUnchecked,
          wxBitmap(menu_check_unchecked_xpm, wxBITMAP_TYPE_XPM)
        )
      ).first->second;
    }
  }

  return wxNullBitmap;
}
//--------------------------------------------------------------------------------

wxMenuItem* EsUtilities::menuItemCreate(wxMenu* parent, int id, const wxString& label,
  const wxString& help, wxItemKind kind, const wxBitmap& normalOrChecked,
  const wxBitmap& unchecked, wxMenu* submenu /*= nullptr*/)
{
  ES_ASSERT(parent);

  wxMenuItem* result = new wxMenuItem(
    parent,
    id,
    label,
    help,
    kind,
    submenu
  );
  ES_ASSERT(result);

#if ES_OS == ES_OS_WINDOWS
  if(wxItemKind::wxITEM_SEPARATOR != kind)
    result->SetBitmaps(
      normalOrChecked,
      unchecked
    );
#else
  if(wxItemKind::wxITEM_NORMAL == kind)
    result->SetBitmap(normalOrChecked);
#endif // ES_OS

  return result;
}
//--------------------------------------------------------------------------------

wxMenuItem* EsUtilities::menuNormalItemCreate(wxMenu* parent, int id, const wxString& label,
  const wxString& help, const wxBitmap& normal/* = wxNullBitmap*/, wxMenu* submenu /*= nullptr*/)
{
  return EsUtilities::menuItemCreate(
    parent,
    id,
    label,
    help,
    wxITEM_NORMAL,
    normal,
    normal,
    submenu
  );
}
//--------------------------------------------------------------------------------

wxMenuItem* EsUtilities::menuCheckItemCreate(wxMenu* parent, int id, const wxString& label,
  const wxString& help, const wxBitmap& checked/* = wxNullBitmap*/, const wxBitmap& unchecked/* = wxNullBitmap*/)
{
  return EsUtilities::menuItemCreate(
    parent,
    id,
    label,
    help,
    wxITEM_CHECK,
    checked.IsNull() ?
      stdBitmapGet(EsStdBitmapId::CheckChecked) :
      checked,
    unchecked.IsNull() ?
      stdBitmapGet(EsStdBitmapId::CheckUnchecked) :
      unchecked,
    nullptr
  );
}
//--------------------------------------------------------------------------------

wxMenuItem* EsUtilities::menuRadioItemCreate(wxMenu* parent, int id, const wxString& label,
  const wxString& help, const wxBitmap& checked/* = wxNullBitmap*/, const wxBitmap& unchecked/* = wxNullBitmap*/)
{
  return EsUtilities::menuItemCreate(
    parent,
    id,
    label,
    help,
    wxITEM_CHECK/*wxITEM_RADIO*/,
    checked.IsNull() ?
      stdBitmapGet(EsStdBitmapId::RadioChecked) :
      checked,
    unchecked.IsNull() ?
      stdBitmapGet(EsStdBitmapId::RadioUnchecked) :
      unchecked,
    nullptr
  );
}
//--------------------------------------------------------------------------------

wxMenu* EsUtilities::menuLanguageSelectionCreate(const EsString::Array& langs, int startId)
{
  wxMenu* result = new wxMenu;
  ES_ASSERT(result);

  // create language selection menu
  for(size_t idx = 0; idx < langs.size(); ++idx)
  {
    const EsString& nativeLang = EsStringI18n::nativeLanguageNameGet(
      langs[idx]
    );
    ES_ASSERT(!nativeLang.empty());

    result->AppendCheckItem(
      startId + idx,
      nativeLang.c_str(),
      wxString::Format(
        _("Select '%s' user interface language").c_str(),
        nativeLang.c_str()
      )
    );
  }

  return result;
}
//--------------------------------------------------------------------------------

void EsUtilities::menuItemBitmapSet(wxMenu* mnu, int id, const wxSize& bmSize, const wxArtClient& client, const wxArtID& bm, const wxArtID& bmDisabled /*= wxEmptyString*/)
{
  ES_ASSERT(mnu);

  wxMenuItem* item = mnu->FindItem(id);
  if( !item )
    return;

  wxBitmap img;
  if( !bm.IsEmpty() )
    img = wxArtProvider::GetBitmap(
      bm,
      client,
      bmSize
    );
  if( img.IsOk() )
    item->SetBitmap(img);

#if ES_OS == ES_OS_WINDOWS
  wxBitmap imgDisabled;
  if( !bmDisabled.IsEmpty() )
    imgDisabled = wxArtProvider::GetBitmap(
      bmDisabled,
      client,
      bmSize
    );
  if( imgDisabled.IsOk() )
    item->SetDisabledBitmap(imgDisabled);
#endif // ES_OS
}
//--------------------------------------------------------------------------------

wxMenuItemPtrs EsUtilities::menuClone(wxMenu* dest, const wxMenu* src, size_t atPos /*= static_cast<size_t>(-1)*/)
{
  wxMenuItemPtrs result;

  if(!src)
    return result;

  ES_ASSERT(dest);

  if( static_cast<size_t>(-1) == atPos || atPos > dest->GetMenuItemCount() )
    atPos = dest->GetMenuItemCount();

  const wxMenuItemList& items = src->GetMenuItems();
  result.reserve(items.size());

  for(auto const& item : items)
  {
    wxMenuItem* newItem = nullptr;

    if(item->IsSubMenu())
    {
      std::unique_ptr<wxMenu> submenu( 
        menuCloneCreate(
          item->GetSubMenu()
        )
      );
      ES_ASSERT(submenu);

      newItem = dest->Insert(
        atPos++,
        item->GetId(),
        item->GetItemLabel(),
        submenu.release(),
        item->GetHelp()
      );
      ES_ASSERT(newItem);
    }
    else
    {
      newItem = new wxMenuItem(
        dest,
        item->GetId(),
        item->GetItemLabel(),
        item->GetHelp(),
        item->GetKind()
      );
      ES_ASSERT(newItem);

      dest->Insert(
        atPos++,
        newItem
      );
    }

#if ES_OS == ES_OS_WINDOWS
    newItem->SetBitmaps(
      item->GetBitmap(),
      item->GetBitmap(false)
    );
    newItem->SetDisabledBitmap(
      item->GetDisabledBitmap()
    );
#else
    newItem->SetBitmap(
      item->GetBitmap()
    );
#endif

    result.push_back( 
      newItem 
    );
  }

  return result;
}
//--------------------------------------------------------------------------------

wxMenu* EsUtilities::menuCloneCreate(const wxMenu* src)
{
  if(!src)
    return nullptr;

  std::unique_ptr<wxMenu> dest = ES_MAKE_UNIQUE(
    wxMenu,
    src->GetTitle(),
    src->GetStyle()
  );

  menuClone(
    dest.get(),
    src
  );

  return dest.release();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

wxWindow* EsUtilities::topLevelWindowGetFor(wxWindow* wnd)
{
  while(wnd)
  {
    if(wnd->IsTopLevel())
      return wnd;

    wnd = wnd->GetParent();
  }

  return nullptr;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static void menuFontSet(wxMenu* menu, const wxFont& fnt)
{
#if ES_OS == ES_OS_WINDOWS
  ES_ASSERT(menu);
  wxMenuItemList& items = menu->GetMenuItems();
  for(size_t idx = 0; idx < items.size(); ++idx)
  {
    wxMenuItem* item = items[idx];
    ES_ASSERT(item);
    if(!item->IsSeparator())
    {
      menu->Remove(item);
      item->SetFont(fnt);
      menu->Insert(idx, item);
    }
    if(item->IsSubMenu())
      menuFontSet(item->GetSubMenu(), fnt);
  }
#endif
}
//--------------------------------------------------------------------------------

void EsUtilities::menuFontApply(wxMenuBar* mnu, const wxFont& fnt)
{
  if(mnu)
  {
    wxWindowUpdateLocker lock(mnu);

    for(size_t idx = 0; idx < mnu->GetMenuCount(); ++idx)
    {
      wxMenu* menu = mnu->GetMenu(idx);
      menuFontSet(menu, fnt);
    }
  }
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static void ctlsFontSet(wxWindow* wnd, const wxFont& fnt)
{
  ES_ASSERT(wnd);
  wxWindowList& children = wnd->GetChildren();
  wxSize sze;
  int height = fnt.GetPixelSize().y;
  for(size_t idx = 0; idx < children.size(); ++idx)
  {
    wxWindow* child = children[idx];
    ES_ASSERT(child);
    ctlsFontSet(child, fnt);
    child->SetFont(fnt);
    sze = child->GetClientSize();
    sze.y = wxMax(sze.y, height);
    child->SetClientSize(sze);
  }
  wnd->SetFont(fnt);
  sze = wnd->GetClientSize();
  sze.y = wxMax(sze.y, height);
  wnd->SetClientSize(sze);
}

void EsUtilities::ctlsFontApply(wxWindow* wnd, const wxFont& fnt)
{
  if(wnd)
  {
    wxWindowUpdateLocker lock(wnd);

    ctlsFontSet(wnd, fnt);
    wnd->Layout();
    wnd->Refresh();
  }
}

// our own message dialog implementation (use centralized font settings)
int EsUtilities::messageShow(const EsString& msg, const EsString& caption, long lStyle, const wxPoint& pos)
{
  wxMessageDialog dlg(wxTheApp->GetTopWindow(), msg.c_str(), caption.c_str(), lStyle, pos);
  return dlg.ShowModal();
}
//--------------------------------------------------------------------------------

// show modal error message box
void EsUtilities::errorShow(const EsString& msg, const EsString& caption/* = _("Error")*/)
{
  EsUtilities::messageShow(msg.c_str(), caption.c_str(), wxCENTRE|wxOK|wxICON_ERROR);
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7,
      arg8
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7,
      arg8,
      arg9
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7,
      arg8,
      arg9,
      arg10
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

void EsUtilities::errorShowFmt(const EsString& caption, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11)
{
  EsUtilities::errorShow(
    EsString::format(
      fmt,
      arg0,
      arg1,
      arg2,
      arg3,
      arg4,
      arg5,
      arg6,
      arg7,
      arg8,
      arg9,
      arg10,
      arg11
    ),
    caption
  );
}
//--------------------------------------------------------------------------------

// get longest string width in pixels, using wnd visual settings
int EsUtilities::longestStringWidthGet(const EsString::Array& strings, const wxWindow& wnd, const wxFont& fnt /*= wxNullFont*/)
{
  int result = 0;

  for(size_t idx = 0; idx < strings.size(); ++idx)
  {
    int w, h;
    if(!fnt.IsNull())
      wnd.GetTextExtent(strings[idx].c_str(), &w, &h, 0, 0, &fnt);
    else
      wnd.GetTextExtent(strings[idx].c_str(), &w, &h);
    result = wxMax(result, w);
  }

  return result;
}
//--------------------------------------------------------------------------------

int EsUtilities::longestStringWidthGet(const wxArrayString& strings, const wxWindow& wnd, const wxFont& fnt /*= wxNullFont*/)
{
  int result = 0;

  for(size_t idx = 0; idx < strings.size(); ++idx)
  {
    int w, h;
    if(!fnt.IsNull())
      wnd.GetTextExtent(strings[idx], &w, &h, 0, 0, &fnt);
    else
      wnd.GetTextExtent(strings[idx], &w, &h);
    result = wxMax(result, w);
  }

  return result;
}
//--------------------------------------------------------------------------------

wxAuiToolBarItem* EsUtilities::toolbarToolAddFromMenuItem(wxAuiToolBar* tb, wxMenu* mnu, int id, const wxArtClient& cli /*= wxEmptyString*/, const wxArtID& bm /*= wxEmptyString*/, const wxArtID& bmDisabled /*= wxEmptyString*/, const wxString& helpLong /*= wxEmptyString*/)
{
  ES_ASSERT(tb);
  ES_ASSERT(mnu);

  wxMenuItem* item = mnu->FindItem(id);
  if( !item )
    return nullptr;

  wxBitmap img;
  if( !bm.IsEmpty() )
    img = wxArtProvider::GetBitmap(
      bm,
      cli,
      tb->GetToolBitmapSize()
    );
#if ES_OS == ES_OS_WINDOWS
  if( !img.IsOk() )
    img = item->GetBitmap(false);
#endif

  wxBitmap imgDisabled;
  if( !bmDisabled.IsEmpty() )
    imgDisabled = wxArtProvider::GetBitmap(
      bmDisabled,
      cli,
      tb->GetToolBitmapSize()
    );
#if ES_OS == ES_OS_WINDOWS
  if( !imgDisabled.IsOk() )
    imgDisabled = item->GetDisabledBitmap();
#endif

  return tb->AddTool(
    id,
    item->GetItemLabelText(),
    img,
    imgDisabled,
    item->GetKind(),
    item->GetHelp(),
    helpLong,
    nullptr
  );
}
//--------------------------------------------------------------------------------

wxSize EsUtilities::appSizePropertyGet(const wxString& prop)
{
  int wh = wxSystemOptions::GetOptionInt(prop);
  ES_ASSERT(-1 != wh);
  return wxSize(
    wh,
    wh
  );
}
//--------------------------------------------------------------------------------

wxSize EsUtilities::appMainFrameMinSizeGet()
{
  int w = wxSystemOptions::GetOptionInt(ES_APP_OPT_MAINFRM_MIN_WIDTH);
  ES_ASSERT(-1 != w);

  int h = wxSystemOptions::GetOptionInt(ES_APP_OPT_MAINFRM_MIN_HEIGHT);
  ES_ASSERT(-1 != h);

  return wxSize(
    w,
    h
  );
}
//--------------------------------------------------------------------------------

wxToolBarToolBase* EsUtilities::toolbarToolAddFromMenuItem(wxToolBar* tb, wxMenu* mnu, int id, const wxArtClient& cli /*= wxEmptyString*/, const wxArtID& bm /*= wxEmptyString*/, const wxArtID& bmDisabled /*= wxEmptyString*/, const wxString& helpLong /*= wxEmptyString*/)
{
  ES_ASSERT(tb);
  ES_ASSERT(mnu);

  wxMenuItem* item = mnu->FindItem(id);
  if(!item)
    return nullptr;

  wxBitmap img;
  if(!bm.IsEmpty())
    img = wxArtProvider::GetBitmap(
      bm,
      cli,
      tb->GetToolBitmapSize()
    );
#if ES_OS == ES_OS_WINDOWS
  if(!img.IsOk())
    img = item->GetBitmap(false);
#endif // ES_OS

  wxBitmap imgDisabled;
  if(!bmDisabled.IsEmpty())
    imgDisabled = wxArtProvider::GetBitmap(
      bmDisabled,
      cli,
      tb->GetToolBitmapSize()
    );
#if ES_OS == ES_OS_WINDOWS
  if(!imgDisabled.IsOk())
    imgDisabled = item->GetDisabledBitmap();
#endif // ES_OS

  return tb->AddTool(
    id,
    item->GetItemLabelText(),
    img,
    imgDisabled,
    item->GetKind(),
    item->GetHelp(),
    helpLong,
    nullptr
  );
}
//--------------------------------------------------------------------------------

wxString EsUtilities::shortHintGet(const wxString& hint)
{
  const wxArrayString& result = wxSplit(hint, '|');
  if(result.size() > 0)
    return result[0];

  return hint;
}
//--------------------------------------------------------------------------------

wxString EsUtilities::longHintGet(const wxString& hint)
{
  const wxArrayString& result = wxSplit(hint, '|');
  if( result.size() > 1 )
    return result[1];

  return hint;
}
//--------------------------------------------------------------------------------

bool EsUtilities::menuItemLabelAndHintUpdate(wxMenuBase* menu, wxWindowID itemId, const wxString& label, const wxString& hint, const wxBitmap& bm /*= wxNullBitmap*/)
{
  if(!menu)
    return false;

  wxString existingLbl;
  wxString existingHint;
  bool itemChanged = false;
  wxMenuItem* item = menu->FindItem(itemId);

  if(!item)
    return false;

  existingLbl = item->GetItemLabel();
  existingHint = item->GetHelp();

  if(existingLbl != label)
  {
    item->SetItemLabel(label);
    itemChanged = true;
  }

  wxString newHint = longHintGet(hint);

  if(existingHint != newHint)
  {
    item->SetHelp(newHint);
    itemChanged = true;
  }

  if(bm.IsOk())
  {
    if(item->IsEnabled())
      item->SetBitmap(bm);
    else
      item->SetDisabledBitmap(bm);

    itemChanged = true;
  }

  return itemChanged;
}
//--------------------------------------------------------------------------------

bool EsUtilities::toolbarItemLabelAndHintUpdate(wxToolBarBase* tb, wxWindowID itemId, const wxString& label, const wxString& hint, const wxBitmap& bm /*= wxNullBitmap*/, bool doUpdateTarget /*= true*/)
{
  if(!tb)
    return false;

  wxToolBarToolBase* tool = tb->FindById(itemId);
  if(!tool)
    return false;

  wxString existingLbl;
  wxString existingHint;
  bool itemChanged = false;

  existingLbl = tool->GetLabel();
  wxString newLbl = wxStripMenuCodes(label);

  if(existingLbl != newLbl)
  {
    tool->SetLabel(newLbl);
    itemChanged = true;
  }

  existingHint = tool->GetShortHelp();
  wxString newHint = shortHintGet(hint);

  if(existingHint != newHint)
  {
    tool->SetShortHelp(newHint);
    itemChanged = true;
  }

  existingHint = tool->GetLongHelp();
  newHint = longHintGet(hint);

  if(existingHint != newHint)
  {
    tool->SetLongHelp(newHint);
    itemChanged = true;
  }

  if(bm.IsOk())
  {
    wxBitmap bmOld;
    if(tool->IsEnabled())
      bmOld = tool->GetNormalBitmap();
    else
      bmOld = tool->GetDisabledBitmap();

    // The bitmap object holds its data pointer by refcount, so, it toolbar tool and menu item
    // refer to the same bitmap, it results in referencing the same bitmap data object.
    // To avoid unnecessary toolbar re-realization, compare the shared bitmap data pointer
    // and it matches, do not change anything.
    // 
    if(bmOld.GetBitmapData() != bm.GetBitmapData())
    {
      if(tool->IsEnabled())
        tool->SetNormalBitmap(bm);
      else
        tool->SetDisabledBitmap(bm);

      itemChanged = true;
    }
  }

  if(
    itemChanged &&
    doUpdateTarget
  )
    tb->Realize();

  return itemChanged;
}
//--------------------------------------------------------------------------------

bool EsUtilities::toolbarItemLabelAndHintUpdate(wxAuiToolBar* tb, wxWindowID itemId, const wxString& label, const wxString& hint, const wxBitmap& bm /*= wxNullBitmap*/, bool doUpdateTarget /*= true*/)
{
  if(!tb)
    return false;

  wxAuiToolBarItem* tool = tb->FindTool(itemId);
  if(!tool)
    return false;

  wxString existingLbl;
  wxString existingHint;
  bool itemChanged = false;

  existingLbl = tool->GetLabel();
  wxString newLbl = wxStripMenuCodes(label);

  if(existingLbl != newLbl)
  {
    tool->SetLabel(newLbl);
    itemChanged = true;
  }

  existingHint = tool->GetShortHelp();
  wxString newHint = shortHintGet(hint);

  if(existingHint != newHint)
  {
    tool->SetShortHelp(newHint);
    itemChanged = true;
  }

  existingHint = tool->GetLongHelp();
  newHint = longHintGet(hint);

  if(existingHint != newHint)
  {
    tool->SetLongHelp(newHint);
    itemChanged = true;
  }

  if(bm.IsOk())
  {
    if(tb->GetToolEnabled(itemId))
      tool->SetBitmap(bm);
    else
      tool->SetDisabledBitmap(bm);

    itemChanged = true;
  }

  if(
    itemChanged &&
    doUpdateTarget
  )
    tb->Realize();

  return itemChanged;
}
//--------------------------------------------------------------------------------
// 
bool EsUtilities::menuOrToolbarItemLabelAndHintUpdate(wxObject* menuOrToolbar, wxWindowID itemId, const wxString& label, const wxString& hint, const wxBitmap& bm /*= wxNullBitmap*/, bool doUpdateTarget /*= true*/)
{
  if( !menuOrToolbar )
    return false;

  wxMenuBase* mnu = dynamic_cast<wxMenuBase*>( menuOrToolbar );
  if(mnu)
    return menuItemLabelAndHintUpdate(
      mnu,
      itemId,
      label,
      hint,
      bm
    );

  wxToolBarBase* tb = dynamic_cast<wxToolBarBase*>( menuOrToolbar );
  if(tb)
    return toolbarItemLabelAndHintUpdate(
      tb,
      itemId,
      label,
      hint,
      bm,
      doUpdateTarget
    );

  wxAuiToolBar* atb = dynamic_cast<wxAuiToolBar*>(menuOrToolbar);
  if(atb)
    return toolbarItemLabelAndHintUpdate(
      atb,
      itemId,
      label,
      hint,
      bm,
      doUpdateTarget
    );

  return false;
}
//--------------------------------------------------------------------------------

void EsUtilities::toolbarItemsLabelsAndHintsUpdateFromMenu(wxToolBarBase* tb, wxMenuBase* menu)
{
  if(!tb || !menu)
    return;
  
  const wxMenuItemList& items = menu->GetMenuItems();
  bool needRealize = false;
  for(auto const& item : items)
  {
    wxBitmap bm = wxNullBitmap;
    if(!item->IsEnabled())
      bm = item->GetDisabledBitmap();
    else
      bm = item->GetBitmap();

    needRealize |= toolbarItemLabelAndHintUpdate(
      tb,
      item->GetId(),
      item->GetItemLabel(),
      item->GetHelp(),
      bm,
      false
    );
  }

  if(needRealize)
    tb->Realize();
}
//--------------------------------------------------------------------------------
