#ifndef _es_data_view_style_h_
#define _es_data_view_style_h_

/// EsDataViewStyle - collection of list item style attributes mapped by integer key
///
class ESCORE_GUI_CLASS EsDataViewStyle
{
public:
  typedef std::shared_ptr<EsDataViewStyle> Ptr;

public:
  static EsDataViewStyle::Ptr create();

  bool isEmpty() const ES_NOTHROW { return m_attrs.empty(); }

  EsDataViewStyle& attrSetDefault(int key);
  EsDataViewStyle& attrSet(int key, const wxColour& colText, const wxColour& colBack, bool bold = false, bool italic = false, bool strikethrough = false);

  wxDataViewItemAttr* attrGet(int key) const;
  void attrDelete(int key);

private:
  std::map<int, wxDataViewItemAttr> m_attrs;
};
//--------------------------------------------------------------------------------

#endif //< _es_data_view_style_h_

