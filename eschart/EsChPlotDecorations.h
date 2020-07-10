#ifndef _es_ch_plot_decorations_h_
#define _es_ch_plot_decorations_h_

// standard plot decoration objects
//
// line marker object
class ESCHART_CLASS EsChPlotLineMarker : public EsChPlotDecorationObject
{
public:
	typedef std::shared_ptr<EsChPlotLineMarker> Ptr;

protected:
	EsChPlotLineMarker(bool vertical);

public:
	static EsChPlotLineMarker::Ptr createVertical();
	static EsChPlotLineMarker::Ptr createHorizontal();

	bool isVertical() const
	{
		return m_vertical;
	}
	double positionGet() const
	{
		return m_pos;
	}
	void positionSet(double pos);
	EsChPlotLineParameters::Ptr lineParametersGet() const
	{
		return m_line;
	}

protected:
	void render(const EsChPlot& plot) const;

protected:
	bool m_vertical;
	double m_pos;
	EsChPlotLineParameters::Ptr m_line;
};

// area marker object
class ESCHART_CLASS EsChPlotAreaMarker : public EsChPlotDecorationObject
{
public:
	typedef std::shared_ptr<EsChPlotAreaMarker> Ptr;

protected:
	EsChPlotAreaMarker(bool vertical);

public:
	static EsChPlotAreaMarker::Ptr createVertical();
	static EsChPlotAreaMarker::Ptr createHorizontal();

	bool isVertical() const
	{
		return m_vertical;
	}
	double startGet() const
	{
		return m_start;
	}
	void startSet(double start);
	double endGet() const
	{
		return m_end;
	}
	void endSet(double end);
	void startEndGet(double& start, double& end) const
	{
		start = m_start;
		end = m_end;
	}
	void startEndSet(double start, double end);
	EsChPlotLineParameters::Ptr lineParametersGet() const
	{
		return m_line;
	}
	EsChPlotFillParameters::Ptr fillParametersGet() const
	{
		return m_fill;
	}

protected:
	void render(const EsChPlot& plot) const;

protected:
	bool m_vertical;
	double m_start;
	double m_end;
	EsChPlotLineParameters::Ptr m_line;
	EsChPlotFillParameters::Ptr m_fill;
};

#endif // _es_ch_plot_decorations_h_
