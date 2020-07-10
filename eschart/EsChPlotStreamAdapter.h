#ifndef _es_ch_plot_stream_adapter_h_
#define _es_ch_plot_stream_adapter_h_

class EsChPlotStreamAdapter : public plstream
{
public:
	EsChPlotStreamAdapter(EsChPlot& plot);
	
protected:
	EsChPlot& m_plot;

	ES_NO_DEFAULT_CTR(EsChPlotStreamAdapter);
	ES_NON_COPYABLE(EsChPlotStreamAdapter);
};

#endif // _es_ch_plot_stream_adapter_h_