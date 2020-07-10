#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotPanel.h"
//--------------------------------------------------------------------------------

EsChPlotPanel::EsChPlotPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, ulong plotRows /*= 1*/, ulong plotCols /*= 1*/) :
wxPanel(parent, id),
m_initRows(plotRows),
m_initCols(plotCols)
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	ES_ASSERT(sizer);
	SetSizer(sizer);

	Layout();
	
	Bind(wxEVT_PAINT, &EsChPlotPanel::onPaint, this);
	Bind(wxEVT_SIZE, &EsChPlotPanel::onSize, this);
}
//--------------------------------------------------------------------------------

void EsChPlotPanel::plotCheckInit()
{
	if(m_plot)
		return;

	m_plot = EsChPlot::create(
		this,
		EsChPlotModeFlag_DEFAULT,
		m_initRows,
		m_initCols
	);
	ES_ASSERT(m_plot);
	ES_ASSERT(m_plot->isOk());
}
//--------------------------------------------------------------------------------

EsChPlotPanel::~EsChPlotPanel()
{
	Unbind(wxEVT_PAINT, &EsChPlotPanel::onPaint, this);
	Unbind(wxEVT_SIZE, &EsChPlotPanel::onSize, this);
}
//--------------------------------------------------------------------------------

EsChPlot::Ptr EsChPlotPanel::plotGet()
{
	plotCheckInit();

	return m_plot;
}
//--------------------------------------------------------------------------------

void EsChPlotPanel::onPaint(wxPaintEvent& WXUNUSED(evt))
{		
	wxPaintDC dc(this);
	wxSize sze = GetClientSize();

	plotCheckInit();
	ES_ASSERT(m_plot);
	ES_ASSERT(m_plot->isOk());

	if( sze != m_plot->sizeGet() )
		m_plot->sizeSet(sze);

	m_plot->draw(dc);
}
//--------------------------------------------------------------------------------

void EsChPlotPanel::onSize(wxSizeEvent& WXUNUSED(evt))
{
	Refresh(false);
}
//--------------------------------------------------------------------------------
