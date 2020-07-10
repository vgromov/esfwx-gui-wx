#ifndef _es_multiline_text_renderer_h_
#define _es_multiline_text_renderer_h_

// Custom text renderer properly handling multiline text size
//
class ESCORE_GUI_CLASS EsMultilineTextRenderer : public wxDataViewTextRenderer
{
public:
  EsMultilineTextRenderer( const wxString& varianttype = wxDataViewTextRenderer::GetDefaultType(), wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int align = wxDVR_DEFAULT_ALIGNMENT );

  virtual bool SetValue( const wxVariant& value ) ES_OVERRIDE;
  virtual wxSize GetSize() const ES_OVERRIDE;

protected:
  bool m_multiline;
};

#endif //<
