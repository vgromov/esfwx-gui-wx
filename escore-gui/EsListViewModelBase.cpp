#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsListViewModelBase.h"
//--------------------------------------------------------------------------------

EsListViewModelBase::EsListViewModelBase() ES_NOTHROW :
m_uuid(EsUUID::generate())
{}
//--------------------------------------------------------------------------------

void EsListViewModelBase::styleSet(const EsDataViewStyle::Ptr& style)
{
  m_style = style;
  refresh();
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::fieldsReset()
{
  m_fldNames.clear();
  m_colTypes.clear();
  m_fldHints.clear();
  m_fldMinWidthHints.clear();
}
//--------------------------------------------------------------------------------

EsListViewModelBase& EsListViewModelBase::fieldAdd(const wxString& name, const wxString& type, ulong minWidthHint /*= 0*/, const wxString& hint /*= wxEmptyString*/)
{
  m_fldNames.push_back(name);
  m_colTypes.push_back(type);
  m_fldMinWidthHints.push_back(minWidthHint);
  m_fldHints.push_back(hint);

  return *this;
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const
{
  const EsVariant& val = fieldDataGet(
    row,
    col
  );

  variant = fieldValueToWxVariant(
    row,
    col,
    val
  );
}
//--------------------------------------------------------------------------------

bool EsListViewModelBase::SetValueByRow(const wxVariant& variant, unsigned row, unsigned col)
{
  fieldDataSet(
    row,
    col,
    fieldValueFromWxVariant(
      row,
      col,
      variant
    )
  );

  return true;
}
//--------------------------------------------------------------------------------

bool EsListViewModelBase::GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr& attr) const
{
  if( !m_style || m_style->isEmpty() )
    return wxDataViewListModel::GetAttrByRow(
      row,
      col,
      attr
    );

  int key = valueStyleKeyGet(
    row,
    col
  );

  wxDataViewItemAttr* pattr = m_style->attrGet(key);
  if( !pattr )
    return false;

  attr = *pattr;
  return true;
}
//--------------------------------------------------------------------------------

wxVariant EsListViewModelBase::fieldValueToWxVariant(ulong recIdx, ulong fieldIdx, const EsVariant& val) const
{
  const wxString& type = fieldTypeGet(fieldIdx);
  if( "string" == type )
    return fieldTextGet(
      recIdx,
      fieldIdx
    );

  return EsUtilities::wxVariantFromEsVariant(
    val
  );
}
//--------------------------------------------------------------------------------

EsVariant EsListViewModelBase::fieldValueFromWxVariant(ulong ES_UNUSED(recIdx), ulong ES_UNUSED(fieldIdx), const wxVariant& wxvar) const
{
  return EsUtilities::esVariantFromWxVariant(wxvar);
}
//--------------------------------------------------------------------------------

EsVariant EsListViewModelBase::doRecordsFind(const EsVariant::Array& fldIndexes, const EsVariant::Array& fldValues) const
{
  EsVariant::Array result;

  size_t recCnt = recordCountGet();
  for( size_t recIdx = 0; recIdx < recCnt; ++recIdx )
  {
    bool recFound = true;

    for( size_t filterIdx = 0; filterIdx < fldIndexes.size(); ++filterIdx )
    {
      ulong fldIdx = fldIndexes[filterIdx].asULong();

      const EsVariant& fldVal = fieldDataGet(
        recIdx,
        fldIdx
      );
      const EsVariant& fltrVal = fldValues[filterIdx];

      if( fldVal != fltrVal )
      {
        recFound = false;
        break;
      }
    }

    if( recFound )
      result.push_back(recIdx);
  }

  if( result.empty() )
    return EsVariant::null();

  return result;
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::onPrependedRec()
{
  wxDataViewItem item(
    wxUIntToPtr(1)
  );

  ItemAdded(
    wxDataViewItem(nullptr),
    item
  );
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::onInsertedRec(ulong before)
{
  wxDataViewItem item(
    wxUIntToPtr(before + 1)
  );

  ItemAdded(
    wxDataViewItem(nullptr),
    item
  );
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::onAppendedRec()
{
  wxDataViewItem item(
    wxUIntToPtr(
      recordCountGet()
    )
  );

  ItemAdded(
    wxDataViewItem(nullptr),
    item
  );
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::onDeletedRec(ulong recIdx)
{
  wxDataViewItem item(
    wxUIntToPtr(recIdx + 1)
  );

  ItemDeleted(
    wxDataViewItem(nullptr),
    item
  );
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::onDeletedRecs(const wxArrayInt& recs)
{
  wxArrayInt sorted = recs;
  sorted.Sort(
    [](int* v1, int* v2) -> int
  {
    return *v2 - *v1;
  }
  );

  wxDataViewItemArray array;
  unsigned int i;
  for( i = 0; i < sorted.GetCount(); i++ )
  {
    wxDataViewItem item(
      wxUIntToPtr(sorted[i] + 1)
    );

    array.Add(item);
  }

  ItemsDeleted(
    wxDataViewItem(nullptr),
    array
  );
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::onRecordChanged(ulong recIdx)
{
  ItemChanged(
    itemFromRecordIdx(recIdx)
  );
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::onValueChanged(ulong recIdx, ulong fldIdx)
{
  ValueChanged(
    itemFromRecordIdx(recIdx),
    fldIdx
  );
}
//--------------------------------------------------------------------------------

unsigned EsListViewModelBase::GetRow(const wxDataViewItem& item) const
{
  return wxPtrToUInt(item.GetID()) - 1;
}
//--------------------------------------------------------------------------------

wxDataViewItem EsListViewModelBase::itemFromRecordIdx(ulong recIdx) const
{
  return wxDataViewItem(
    wxUIntToPtr(recIdx + 1)
  );
}
//--------------------------------------------------------------------------------

bool EsListViewModelBase::HasDefaultCompare() const
{
  return true;
}
//--------------------------------------------------------------------------------

int EsListViewModelBase::Compare(const wxDataViewItem& item1,
  const wxDataViewItem& item2,
  unsigned int WXUNUSED(column),
  bool ascending) const
{
  unsigned int pos1 = wxPtrToUInt(item1.GetID());  // -1 not needed here
  unsigned int pos2 = wxPtrToUInt(item2.GetID());  // -1 not needed here

  if( ascending )
    return pos1 - pos2;
  else
    return pos2 - pos1;
}
//--------------------------------------------------------------------------------

unsigned EsListViewModelBase::GetChildren(const wxDataViewItem& WXUNUSED(item), wxDataViewItemArray& WXUNUSED(children)) const
{
  return 0;  // should we report an error ?
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::recordSet(ulong recIdx, const EsVariant& record)
{
  doRecordSet(
    recIdx,
    record
  );

  onRecordChanged(recIdx);
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::fieldDataSet(ulong recIdx, ulong fldIdx, const EsVariant& val)
{
  doFieldDataSet(
    recIdx,
    fldIdx,
    val
  );

  onValueChanged(
    recIdx,
    fldIdx
  );
}
//--------------------------------------------------------------------------------

EsVariant EsListViewModelBase::recordsFind(const EsVariant& fldIndexes, const EsVariant& fldValues) const
{
  if( isEmpty() )
    return EsVariant::null();

  if( fldIndexes.isEmpty() || fldValues.isEmpty() )
    return EsVariant::null();

  const EsVariant::Array& idxs = fldIndexes.asVariantCollection();
  const EsVariant::Array& vals = fldValues.asVariantCollection();

  if( idxs.size() != vals.size() )
    EsException::Throw(
      _("EsListViewModelBase::recordsFind: Both arguments must be of the same size")
    );

  return doRecordsFind(
    idxs,
    vals
  );
}
//--------------------------------------------------------------------------------

ulong EsListViewModelBase::recordAppend(const EsVariant& record)
{
  ulong newRecIdx = doRecordAppend(record);

  onAppendedRec();

  return newRecIdx;
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::recordDelete(ulong recIdx)
{
  doRecordDelete(recIdx);
  onDeletedRec(recIdx);
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::clear()
{
  BeforeReset();
  doClear();
  AfterReset();
}
//--------------------------------------------------------------------------------

void EsListViewModelBase::refresh()
{
  doRefresh();
}
//--------------------------------------------------------------------------------

wxString EsListViewModelBase::asString() const
{
  if( isEmpty() )
    return wxEmptyString;

  return asString(
    0,
    fieldCountGet() - 1,
    0,
    recordCountGet() - 1
  );
}
//--------------------------------------------------------------------------------

wxString EsListViewModelBase::asString(ulong recIdx) const
{
  return asString(
    0,
    fieldCountGet() - 1,
    recIdx,
    recIdx
  );
}
//--------------------------------------------------------------------------------

wxString EsListViewModelBase::asString(ulong fldStart, ulong fldEnd, ulong recStart, ulong recEnd) const
{
  ES_ASSERT(!isEmpty());
  ES_ASSERT(fldStart <= fldEnd);
  ES_ASSERT(recStart <= recEnd);
  ES_ASSERT(fldStart < fieldCountGet());
  ES_ASSERT(fldEnd < fieldCountGet());
  ES_ASSERT(recStart < recordCountGet());
  ES_ASSERT(recEnd < recordCountGet());

  wxString result;

  for( ulong recIdx = recStart; recIdx <= recEnd; ++recIdx )
  {
    wxString line;
    for( ulong fldIdx = fldStart; fldIdx <= fldEnd; ++fldIdx )
    {
      if( !result.empty() )
        line += esT("||");

      line += fieldTextGet(
        recIdx,
        fldIdx
      );
    }

    result += line + esT("\n\r");
  }

  return result;
}
//--------------------------------------------------------------------------------
