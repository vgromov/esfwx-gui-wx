#ifndef _es_list_view_model_array_h_
#define _es_list_view_model_array_h_

/// Data View Model specialization for integrated indexed array storage
/// data model abstraction. 
/// Data explicitly stored in model object.
///
class ESCORE_GUI_CLASS EsListViewModelArray : public EsListViewModelBase
{
protected:
  EsListViewModelArray() ES_NOTHROW {}

public:
  /// Create and return new data model object
  static EsListViewModelBase::Ptr create();

  /// Return true if there is no data records
  virtual bool isEmpty() const ES_NOTHROW ES_OVERRIDE { return m_data.empty(); }

  /// Return total data records count
  virtual ulong recordCountGet() const ES_NOTHROW ES_OVERRIDE { return m_data.size(); }

  /// Return textual data representation for specific field
  virtual wxString fieldTextGet(ulong recIdx, ulong fldIdx) const ES_OVERRIDE;

  /// Return raw data for specific field
  virtual EsVariant fieldDataGet(ulong recIdx, ulong fldIdx) const ES_OVERRIDE;

  /// Individual record access
  virtual EsVariant recordGet(ulong recIdx) const ES_OVERRIDE;

  /// We're not virtualized data access, we store all data in model itself.
  virtual bool IsVirtualListModel() const ES_OVERRIDE { return false; }

protected:
  /// Clear all data records
  virtual void doClear() ES_OVERRIDE;

  /// Overridable record lookup
  virtual EsVariant doRecordsFind(const EsVariant::Array& fldIndexes, const EsVariant::Array& fldValues) const ES_OVERRIDE;

  /// Rewrite record data at specified record index
  virtual void doRecordSet(ulong recIdx, const EsVariant& record) ES_OVERRIDE;

  /// Set data fiels value in specific record
  virtual void doFieldDataSet(ulong recIdx, ulong fldIdx, const EsVariant& val) ES_OVERRIDE;

  /// Append record to the data storage, return new record index
  virtual ulong doRecordAppend(const EsVariant& record) ES_OVERRIDE;

  /// Remove record at specified index
  virtual void doRecordDelete(ulong recIdx) ES_OVERRIDE;

protected:
  std::vector<EsVariant::Array> m_data;

  ES_NON_COPYABLE(EsListViewModelArray);
};
//--------------------------------------------------------------------------------

#endif //< _es_list_view_model_array_h_
