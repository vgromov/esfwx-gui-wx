#ifndef _es_list_view_model_base_h_
#define _es_list_view_model_base_h_

/// Data View base data model abstraction
///
class ESCORE_GUI_CLASS EsListViewModelBase : public wxDataViewListModel
{
public:
  typedef wxObjectDataPtr<EsListViewModelBase> Ptr;

protected:
  EsListViewModelBase() ES_NOTHROW;

public:
  /// Custom data styling support
  void styleSet(const EsDataViewStyle::Ptr& style);
  EsDataViewStyle::Ptr styleGet() ES_NOTHROW { return m_style; }

  /// Model object uuid itself. Used solely for model-view events handling
  EsString modelUuidGet() const ES_NOTHROW { return m_uuid.asString(false); }

  /// Return true if there is no data records
  virtual bool isEmpty() const ES_NOTHROW = 0;

  /// Return total data records count
  virtual ulong recordCountGet() const ES_NOTHROW = 0;

  /// Clear all fields metadata
  void fieldsReset();

  /// Append field metadata, and return reference to the model object, to 
  /// augment chained fieldAdd calls
  ///
  virtual EsListViewModelBase& fieldAdd(const wxString& name, const wxString& type, ulong minWidthHint = 0, const wxString& hint = wxEmptyString);

  /// Return total data fields count
  ulong fieldCountGet() const ES_NOTHROW { return m_fldNames.size(); }

  /// Return field name, if any
  const wxString& fieldNameGet(ulong fldIdx) const { return m_fldNames[fldIdx]; }

  /// Return field mind width hint, if any
  int fieldMinWidthGet(ulong fldIdx) const { return m_fldMinWidthHints[fldIdx]; }

  /// Return field hint, if any
  const wxString& fieldHintGet(ulong fldIdx) const { return m_fldHints[fldIdx]; }

  /// Return field type string
  const wxString& fieldTypeGet(ulong col) const { return m_colTypes[col]; }

  /// Return textual data representation for specific field
  virtual wxString fieldTextGet(ulong recIdx, ulong fldIdx) const = 0;

  /// Return raw data for specific field
  virtual EsVariant fieldDataGet(ulong recIdx, ulong fldIdx) const = 0;

  /// Set raw data for specific field
  void fieldDataSet(ulong recIdx, ulong fldIdx, const EsVariant& val);

  /// Find record(s) by one or more field values
  /// @param   [in]  - Collection of the field indexes to look up, or the single index value
  /// @param   [in]  - Collection of the field values to look up, or the single value. Must have the same size as 
  ///                  the field indexes collection
  /// @return        - Collection of the found record indexes, or empty variant, if no records were found
  ///
  EsVariant recordsFind(const EsVariant& fldIndexes, const EsVariant& fldValues) const;

  /// Individual record access
  virtual EsVariant recordGet(ulong recIdx) const = 0;

  /// Rewrite record data at specified record index
  void recordSet(ulong recIdx, const EsVariant& record);

  /// Append record to the end of container, return newly added record index
  ulong recordAppend(const EsVariant& record);

  /// Remove record at specified index
  void recordDelete(ulong recIdx);

  /// Remove all data records from model
  void clear();

  /// Refresh - notify control(s) that data should be re-read from model.
  void refresh();

  /// Get all existing data in container as string
  virtual wxString asString() const;

  /// Get record as string
  virtual wxString asString(ulong recIdx) const;

  /// Get only selected portion of existing data as string
  virtual wxString asString(ulong fldStart, ulong fldEnd, ulong recStart, ulong recEnd) const;

  /// wxDataViewListModel API overrides
  ///

  /// Return count of coulmns (fields) assigned to model metadata
  virtual unsigned int GetColumnCount() const ES_OVERRIDE { return fieldCountGet(); }

  /// Return type of specific column in terma of WX view model
  virtual wxString GetColumnType(unsigned col) const ES_OVERRIDE { return fieldTypeGet(col); }

  /// Returns the number of rows
  virtual unsigned int GetCount() const ES_OVERRIDE { return recordCountGet(); }

  /// Convert to/from row/wxDataViewItem
  virtual unsigned GetRow(const wxDataViewItem& item) const ES_OVERRIDE;

  /// Get field value and convert it to wxVariant
  virtual void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const ES_OVERRIDE;

  /// Assign value from variant to specific field in record
  virtual bool SetValueByRow(const wxVariant& variant, unsigned row, unsigned col) ES_OVERRIDE;

  /// Allow custom attributing of model values
  virtual bool GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr& attr) const ES_OVERRIDE;

  /// Compare based on index
  virtual int Compare(
    const wxDataViewItem& item1,
    const wxDataViewItem& item2,
    unsigned int column,
    bool ascending
  ) const wxOVERRIDE;

  virtual bool HasDefaultCompare() const wxOVERRIDE;

  /// Base functionality
  virtual unsigned int GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const wxOVERRIDE;

  /// We're virtual list
  virtual bool IsVirtualListModel() const ES_OVERRIDE { return true; }

protected:
  /// Virtual list API call-backs
  ///
  void onPrependedRec();
  void onInsertedRec(ulong beforeRecIdx);
  void onAppendedRec();
  void onDeletedRec(ulong recIdx);
  void onDeletedRecs(const wxArrayInt& recs);
  void onRecordChanged(ulong recIdx);
  void onValueChanged(ulong recIdx, ulong fldIdx);

  /// Return wxDataViewItem from record idx
  wxDataViewItem itemFromRecordIdx(ulong recIdx) const;

  /// Convert EsVariant field value to wxVariant and back. Use direct convertion by default, see EsUtilities from escoregui
  ///
  virtual wxVariant fieldValueToWxVariant(ulong recIdx, ulong fieldIdx, const EsVariant& val) const;
  virtual EsVariant fieldValueFromWxVariant(ulong recIdx, ulong fieldIdx, const wxVariant& wxvar) const;

  /// Clear all data records
  virtual void doClear() = 0;

  /// Perform some action on data refresh request. Do nothing by default
  virtual void doRefresh() {}

  /// Overridable record lookup
  virtual EsVariant doRecordsFind(const EsVariant::Array& fldIndexes, const EsVariant::Array& fldValues) const;

  /// Rewrite record data at specified record index
  virtual void doRecordSet(ulong recIdx, const EsVariant& record) = 0;

  /// Set data fiels value in specific record
  virtual void doFieldDataSet(ulong recIdx, ulong fldIdx, const EsVariant& val) = 0;

  /// Append record to the data storage, return new record index
  virtual ulong doRecordAppend(const EsVariant& record) = 0;

  /// Remove record at specified index
  virtual void doRecordDelete(ulong recIdx) = 0;

  /// Return record styling key from record and field index
  virtual int valueStyleKeyGet(unsigned int row, unsigned int col) const { return wxNOT_FOUND; }

protected:
  EsDataViewStyle::Ptr m_style;
  EsUUID m_uuid;
  wxArrayString m_fldNames;
  wxArrayString m_colTypes;
  wxArrayString m_fldHints;
  wxArrayInt m_fldMinWidthHints;

  ES_NON_COPYABLE(EsListViewModelBase);
};
//--------------------------------------------------------------------------------

#endif //< _es_list_view_model_base_h_
