#ifndef _es_ch_plot_series_renderer_h_
#define _es_ch_plot_series_renderer_h_

// series renderer abstract interface. renderer is used in conjunction with series
// object on 1-to-1 basis, for the sake of performance, so no renderers may be shared
// between different series
//
class ESCHART_CLASS EsChPlotSeriesRenderer : public EsChPlotObject
{
public:
	typedef std::shared_ptr<EsChPlotSeriesRenderer> Ptr;
	typedef std::vector<Ptr> Array;

protected:
	EsChPlotSeriesRenderer();

public:
	virtual ~EsChPlotSeriesRenderer();

protected:
	// EsChPlotObject overrides
	virtual void onRecalc(const EsChPlotObject& sender);
	virtual void onRedraw(const EsChPlotObject& sender) const;
	virtual bool hitInfoGet(const EsChPlot& plot, double wx, double wy, 
		double toleranceX, double toleranceY, 
		EsChPlotObjectHitInfo& info) const;
	
	// render linked series data to the specified plot
	void render(const EsChPlot& plot) const;
	// render-specific service, it's normally used to decimate points before they gets drawn
	// mainly to reduce the graphics engine load. default implementation does nothing
	void optimize(const EsChPlot& plot) const;

	// series linking services
	void linkSeries(const EsChPlotSeries::Ptr& series);
	void unlinkSeries();
	
	// overridable rendering services
	virtual bool isVisible(const EsChPlot& plot) const;
	virtual	void doRender(const EsChPlot& plot) const = 0;
	virtual void doOptimize(const EsChPlot& plot) const {}
	
	// internally set invalid flag
	void invalidate() const
	{
		m_invalid = true;
	}

protected:
	// linked series
	EsChPlotSeries::Ptr m_series;
	// invalidation flag
	mutable bool m_invalid;

	friend class EsChPlotView;
	friend class EsChPlot;
};

// standard renderers
//
// line + polygonal fill with optional point markers
class ESCHART_CLASS EsChPlotSeriesRendererCartesian2d : public EsChPlotSeriesRenderer
{
public:
	enum { 
		cacheChunkSize = 1024,
	};
	typedef std::shared_ptr<EsChPlotSeriesRendererCartesian2d> Ptr;

protected:
	EsChPlotSeriesRendererCartesian2d(const EsChPlotLineParameters::Ptr& line, 
		const EsChPlotFillParameters::Ptr& fill, const EsChPlotPointParameters::Ptr& point);

public:
	static EsChPlotSeriesRendererCartesian2d::Ptr create(
		const EsChPlotLineParameters::Ptr& line = EsChPlotLineParameters::Ptr(), 
		const EsChPlotFillParameters::Ptr& fill = EsChPlotFillParameters::Ptr(),
		const EsChPlotPointParameters::Ptr& point = EsChPlotPointParameters::Ptr());
	// parameters access
	//
	// line
	EsChPlotLineParameters::Ptr lineParametersGet()
	{
		return m_line;
	}
	const EsChPlotLineParameters::Ptr& lineParametersGet() const
	{
		return m_line;
	}
	// fill
	EsChPlotFillParameters::Ptr fillParametersGet()
	{
		return m_fill;
	}
	const EsChPlotFillParameters::Ptr& fillParametersGet() const
	{
		return m_fill;
	}
	// point
	EsChPlotPointParameters::Ptr pointParametersGet()
	{
		return m_point;
	}
	const EsChPlotPointParameters::Ptr& pointParametersGet() const
	{
		return m_point;
	}

protected:
	// overridable render services
	virtual bool isVisible(const EsChPlot& plot) const;
	virtual	void doRender(const EsChPlot& plot) const;
	virtual void doOptimize(const EsChPlot& plot) const;
	virtual bool hitInfoGet(const EsChPlot& plot, double wx, double wy, 
		double toleranceX, double toleranceY, EsChPlotObjectHitInfo& info) const;
	// renderer-specific internals
	void cachePoly(size_t offs, size_t cnt) const;
	void renderPoly(const EsChPlot& plot) const;
	void renderLine(const EsChPlot& plot) const;
	void renderPoints(const EsChPlot& plot) const;
	// specific renderer hit test implementation
	bool hitPoint(const EsChPlot& plot, double wx, double wy, 
		double toleranceX, double toleranceY, size_t& idx) const;
	bool hitPoly(double wx, double wy, 
		double toleranceX, double toleranceY) const;
	bool hitLine(const EsChPlot& plot, double wx, double wy, 
		double toleranceX, double toleranceY) const;
	// cache overflow check
	void checkCacheOverflow() const;

protected:
	EsChPlotLineParameters::Ptr m_line;
	EsChPlotFillParameters::Ptr m_fill;
	EsChPlotPointParameters::Ptr m_point;
	// optimized data cache
	mutable size_t m_cnt;
	mutable EsMathArrayReal m_x;
	mutable EsMathArrayReal m_y;
	// polygonal cache
	mutable size_t m_pcnt;
	mutable EsMathArrayReal m_px;
	mutable EsMathArrayReal m_py;
};

// histogram bars
class ESCHART_CLASS EsChPlotSeriesRendererHistogram2d : public EsChPlotSeriesRenderer
{
public:
	enum { 
		cacheChunkSize = 1024,
	};
	typedef std::shared_ptr<EsChPlotSeriesRendererHistogram2d> Ptr;

protected:
	EsChPlotSeriesRendererHistogram2d(ulong width, wxColor color, double base);

public:
	static EsChPlotSeriesRendererHistogram2d::Ptr create( ulong width, wxColor color, double base = 0.);

	// parameters access
	ulong widthGet() const {	return m_line->widthGet(); }
	void widthSet(ulong width)	{	m_line->widthSet(width); }

	wxColor colorGet() const { return m_line->colorGet();	}
	void colorSet(const wxColor& cl) { m_line->colorSet(cl); }

	EsString colorStringGet() const;
	void colorStringSet(const EsString& str);

protected:
	// overridable render services
	virtual bool isVisible(const EsChPlot& plot) const;
	virtual	void doRender(const EsChPlot& plot) const;
	virtual void doOptimize(const EsChPlot& plot) const;
	virtual bool hitInfoGet(const EsChPlot& plot, double wx, double wy, double toleranceX, double toleranceY, EsChPlotObjectHitInfo& info) const;
	// specific rendering stuff
	void renderBar(const EsChPlot& plot, double x, double y) const;
	// cache overflow check
	void checkCacheOverflow() const;

protected:
	EsChPlotLineParameters::Ptr m_line;
	double m_base;
	// optimized points cache
	mutable ulong m_cnt;
	mutable EsMathArrayReal m_x;
	mutable EsMathArrayReal m_y;
};

#endif // _es_ch_plot_series_renderer_h_
