#include "escore_gui_pch.h"
#pragma hdrstop

#include <wx/dataview.h>
#include "EsMultilineTextRenderer.h"
//--------------------------------------------------------------------------------

EsMultilineTextRenderer::EsMultilineTextRenderer( const wxString& varianttype /*= wxDataViewTextRenderer::GetDefaultType()*/, wxDataViewCellMode mode /*= wxDATAVIEW_CELL_INERT*/, int align /*= wxDVR_DEFAULT_ALIGNMENT */ ) :
wxDataViewTextRenderer(
  varianttype,
  mode,
  align
),
m_multiline( false )
{
}
//--------------------------------------------------------------------------------

bool EsMultilineTextRenderer::SetValue( const wxVariant& value )
{
  if( wxDataViewTextRenderer::SetValue(value) )
  {
    m_multiline = wxString::npos != m_text.find(wxT('\n'));
    return true;
  }

  m_multiline = false;
  return false;
}
//--------------------------------------------------------------------------------

wxSize EsMultilineTextRenderer::GetSize() const
{
  wxSize result;

  if( m_multiline )
  {
    wxDataViewCtrl* view = GetView();
    ES_ASSERT(view);

    wxClientDC dc(view);

    if ( GetAttr().HasFont() )
      dc.SetFont(GetAttr().GetEffectiveFont(view->GetFont()));

    result = dc.GetMultiLineTextExtent(m_text);
  }
  else
    result = wxDataViewTextRenderer::GetSize();

  //ES_DEBUG_TRACE(
  //  esT("EsMultilineTextRenderer::GetSize -> x: %d, y: %d"),
  //  result.x,
  //  result.y
  //);

  return result;
}
//--------------------------------------------------------------------------------
