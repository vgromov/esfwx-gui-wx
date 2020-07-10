#ifndef _es_ch_plot_panel_h_
#define _es_ch_plot_panel_h_

class ESCHART_CLASS EsChPlotPanel : public wxPanel
{
public:
	EsChPlotPanel(wxWindow* parent, wxWindowID id = wxID_ANY, ulong plotRows = 1, ulong plotCols = 1);
	virtual ~EsChPlotPanel();

public:
	EsChPlot::Ptr plotGet();

protected:
	void plotCheckInit();

	// WX handlers
	// 
	void onPaint(wxPaintEvent& evt);
	void onSize(wxSizeEvent& evt);

protected:
	EsChPlot::Ptr m_plot;
	ulong m_initRows;
	ulong m_initCols;
};

#endif // _es_ch_plot_panel_h_ 