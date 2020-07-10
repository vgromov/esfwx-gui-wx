#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsDataViewStyle.h"
//--------------------------------------------------------------------------------

EsDataViewStyle::Ptr EsDataViewStyle::create()
{
  EsDataViewStyle::Ptr ptr = ES_MAKE_SHARED(EsDataViewStyle);
  ES_ASSERT(ptr);

  return ptr;
}
//--------------------------------------------------------------------------------

EsDataViewStyle& EsDataViewStyle::attrSetDefault(int key)
{
  wxDataViewItemAttr attr;
  m_attrs[key] = attr;

  return *this;
}
//--------------------------------------------------------------------------------

EsDataViewStyle& EsDataViewStyle::attrSet(int key, const wxColour& colText, const wxColour& colBack, bool bold /*= false*/, bool italic /*= false*/, bool strikethrough /*= false*/)
{
  wxDataViewItemAttr attr;
  attr.SetColour(colText);
  attr.SetBackgroundColour(colBack);
  attr.SetBold(bold);
  attr.SetItalic(italic);
  attr.SetStrikethrough(strikethrough);

  m_attrs[key] = attr;

  return *this;
}
//--------------------------------------------------------------------------------

wxDataViewItemAttr* EsDataViewStyle::attrGet(int key) const
{
  auto cit = m_attrs.find(key);
  if( cit != m_attrs.end() )
    return const_cast<wxDataViewItemAttr*>(&(*cit).second);

  return nullptr;
}
//--------------------------------------------------------------------------------

void EsDataViewStyle::attrDelete(int key)
{
  auto it = m_attrs.find(key);
  if( it != m_attrs.end() )
    m_attrs.erase(it);
}
//--------------------------------------------------------------------------------
