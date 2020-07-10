#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsListViewModelArray.h"
//--------------------------------------------------------------------------------

EsListViewModelBase::Ptr EsListViewModelArray::create()
{
  return EsListViewModelBase::Ptr(
    new EsListViewModelArray
  );
}
//--------------------------------------------------------------------------------

wxString EsListViewModelArray::fieldTextGet(ulong recIdx, ulong idx) const
{
  const EsVariant& data = fieldDataGet(recIdx, idx);

  if( data.isString() )
    return data.doInterpretAsString().c_str();
  else if( !data.isEmpty() )
    return data.asString().c_str();

  return wxEmptyString;
}
//--------------------------------------------------------------------------------

EsVariant EsListViewModelArray::fieldDataGet(ulong recIdx, ulong idx) const
{
  return recordGet(recIdx)[idx];
}
//--------------------------------------------------------------------------------

EsVariant EsListViewModelArray::doRecordsFind(const EsVariant::Array& fldIndexes, const EsVariant::Array& fldValues) const
{
  EsVariant::Array result;
  for( size_t recidx = 0; recidx < m_data.size(); ++recidx )
  {
    const EsVariant::Array& rec = m_data[recidx];
    bool recFound = true;

    for( size_t filterIdx = 0; filterIdx < fldIndexes.size(); ++filterIdx )
    {
      ulong fldIdx = fldIndexes[filterIdx].asULong();

      const EsVariant& fldVal = rec[fldIdx];
      const EsVariant& fltrVal = fldValues[filterIdx];

      if( fldVal != fltrVal )
      {
        recFound = false;
        break;
      }
    }

    if( recFound )
      result.push_back(recidx);
  }

  if( result.empty() )
    return EsVariant::null();

  return result;
}
//--------------------------------------------------------------------------------

EsVariant EsListViewModelArray::recordGet(ulong recIdx) const
{
  return m_data[recIdx];
}
//--------------------------------------------------------------------------------

void EsListViewModelArray::doRecordSet(ulong recIdx, const EsVariant& record)
{
  ES_ASSERT(fieldCountGet() == record.countGet());

  m_data[recIdx] = record.doInterpretAsVariantCollection();
}
//--------------------------------------------------------------------------------

void EsListViewModelArray::doFieldDataSet(ulong recIdx, ulong fldIdx, const EsVariant& val)
{
  ES_ASSERT(fieldCountGet() > fldIdx);

  m_data[recIdx][fldIdx] = val;
}
//--------------------------------------------------------------------------------

ulong EsListViewModelArray::doRecordAppend(const EsVariant& record)
{
  ES_ASSERT(fieldCountGet() == record.countGet());
  m_data.push_back(
    record.doInterpretAsVariantCollection()
  );

  return m_data.size() - 1;
}
//--------------------------------------------------------------------------------

void EsListViewModelArray::doRecordDelete(ulong recIdx)
{
  m_data.erase(
    m_data.begin() + recIdx
  );
}
//--------------------------------------------------------------------------------

void EsListViewModelArray::doClear()
{
  m_data.clear();
}
//-------------------------------------------------------------------------------- 
