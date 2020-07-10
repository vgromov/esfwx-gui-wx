#ifndef _es_ch_plot_view_parameters_h_
#define _es_ch_plot_view_parameters_h_

// plot viewport parameters holder class
//
class ESCHART_CLASS EsChPlotView : public EsChPlotObject
{
public:
  typedef std::shared_ptr<EsChPlotView> Ptr;
  typedef std::vector<EsChPlotView::Ptr> Array;

protected:
  EsChPlotView(EsChPlotObject* owner, EsChPlotCoordinateSystem cs, const EsString& title);

public:
  virtual ~EsChPlotView();

  // parameters access
  size_t dimensionGet() const ES_NOTHROW
  {
    return m_cs < EsChPlotCoordinateSystem_CARTESIAN_3D ? 2 : 3;
  }
  EsChPlotCoordinateSystem coordinateSystemGet() const ES_NOTHROW
  {
    return m_cs;
  }
  EsChPlotAxis::Ptr axisGet(size_t dim) const;
  EsChPlotLineParameters::Ptr axesLineParametersGet() const ES_NOTHROW
  {
    return m_axesLineParams;
  }
  EsChPlotLineParameters::Ptr axesGridLineParametersGet() const ES_NOTHROW
  {
    return m_axesGridLineParams;
  }
  EsChPlotTextParameters::Ptr titlesParametersGet() const ES_NOTHROW
  {
    return m_titlesParams;
  }
  EsChPlotTextParameters::Ptr labelsParametersGet() const ES_NOTHROW
  {
    return m_labelsParams;
  }
  
  // axis title manipulation
  const EsString& titleGet() const ES_NOTHROW { return m_title; }
  void titleSet(const EsString& title);
  
  bool isTitleVisible() const ES_NOTHROW { return !m_title.empty(); }
  
  // ticks lengths access
  double majorTicksLengthGet() const ES_NOTHROW { return m_majorTicksLen; }
  double minorTicksLengthGet() const ES_NOTHROW { return m_minorTicksLen; }

  // get world range for the data linked to this view,
  // if no data linked, default 0-1 range is used
  double dataMinGet(size_t dim) const;
  double dataMaxGet(size_t dim) const;
  void dataMinMaxGet(size_t dim, double& min, double& max) const;

  // view window manipulation (kinda viewing through magnifying glass)
  void windowHget(double& min, double& max) const;
  void windowHset(double min, double max);
  void windowVget(double& min, double& max) const;
  void windowVset(double min, double max);
  void windowGet(double& hmin, double& hmax, double& vmin, double& vmax) const;
  void windowSet(double hmin, double hmax, double vmin, double vmax);
  void windowReset();

  double windowHminGet() const ES_NOTHROW { return m_hmin; }
  double windowHmaxGet() const ES_NOTHROW { return m_hmax; }
  double windowVminGet() const ES_NOTHROW { return m_vmin; }
  double windowVmaxGet() const ES_NOTHROW { return m_vmax; }

  // series manipulation
  void seriesAdd(const EsChPlotSeries::Ptr& series, const EsChPlotSeriesRenderer::Ptr& renderer);
  void seriesRemove(const EsChPlotSeries::Ptr& series);
  void seriesRendererSet(const EsChPlotSeries::Ptr& series, const EsChPlotSeriesRenderer::Ptr& renderer);
  EsChPlotSeriesRenderer::Ptr seriesRendererGet(const EsChPlotSeries::Ptr& series) const;
  const EsChPlotSeries::Array& seriesGet() const ES_NOTHROW { return m_series; }
  const EsChPlotSeriesRenderer::Array& renderersGet() const ES_NOTHROW { return m_renderers; }

  // decorations manipulation
  void decorationAdd(const EsChPlotDecorationObject::Ptr& decoration);
  void decorationRemove(const EsChPlotDecorationObject::Ptr& decoration);
  const EsChPlotDecorationObject::Array& decorationsGet() const ES_NOTHROW { return m_decorations; }

protected:
  // internal services
  //
  // direct invalidation of all owned renderers
  void invalidateRenderers();

  // EsChPlotObject overrides
  virtual void onRecalc(const EsChPlotObject& sender);
  virtual void onUnlock();
  virtual bool hitInfoGet(const EsChPlot& plot, double wx, double wy, double toleranceX, double toleranceY, EsChPlotObjectHitInfo& info) const;

  // internal series search, throws exception if not found, otherwise, returns 
  // series index in container
  size_t internalSeriesFind(const EsChPlotSeries::Ptr& series, bool doThrow = true) const;

  // internal window setters
  void internalWindowHset(double min, double max);
  void internalWindowVset(double min, double max);

  // view axes parameters update
  void axesUpdateRange();

  // dimension check
  void dimensionCheck(size_t dim) const;

protected:
  EsChPlotCoordinateSystem m_cs;
  EsChPlotTextParameters::Ptr m_titlesParams;
  EsChPlotTextParameters::Ptr m_labelsParams;
  EsChPlotLineParameters::Ptr m_axesLineParams;
  EsChPlotLineParameters::Ptr m_axesGridLineParams;
  EsChPlotAxis::Ptr m_axes[3];
  EsString m_title;
  // bound series && renderers
  EsChPlotSeries::Array m_series;
  EsChPlotSeriesRenderer::Array m_renderers;
  // optional decorations
  EsChPlotDecorationObject::Array m_decorations;
  // view vindow
  double m_hmin;
  double m_hmax;
  double m_vmin;
  double m_vmax;
  // ticks lengths
  double m_majorTicksLen;
  double m_minorTicksLen;

  friend class EsChPlot;
};

#endif // _es_ch_plot_view_parameters_h_