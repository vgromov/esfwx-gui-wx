#ifndef _es_ch_plot_h_
#define _es_ch_plot_h_

// Forward declaration
class EsChPlotStreamAdapter;

// plot class
// 
// EsChPlotModeFlag_BACKEND_DC: use the standard wxDC backend (always available)
// EsChPlotModeFlag_BACKEND_GC: use the wxGraphicsContext backend (if available)
// EsChPlotModeFlag_DRAWTEXT: use wxWidgets routines to draw text.
enum EsChPlotModeFlag
{
//	EsChPlotModeFlag_BACKEND_DC						= 0x0004,
  EsChPlotModeFlag_BACKEND_GC = 0x0010,
  EsChPlotModeFlag_DRAW_TEXT = 0x0020,
  // default flags set
  EsChPlotModeFlag_DEFAULT = EsChPlotModeFlag_BACKEND_GC |
  EsChPlotModeFlag_DRAW_TEXT,
};

class ESCHART_CLASS EsChPlot : public EsChPlotObject
{
public:
  enum {
    defHitTolerance = 4,
  };
  typedef std::shared_ptr<EsChPlot> Ptr;

protected:
  // plotting session scope class
  class ESCHART_CLASS Session
  {
  public:
    Session(const EsChPlot& plot);
    ~Session();

  protected:
    const EsChPlot& m_plot;

    ES_NON_COPYABLE(Session);
  };
  friend class Session;

protected:
  EsChPlot(wxWindow* container, const wxSize& size, int flags, ulong rows, ulong cols, const wxColor& bgColor);

public:
  ~EsChPlot();

  /// @brief  Create and prepare plot view(s), rows & cols define count of plot views available.
  ///
  /// @param  size    Requested plot view(s) area size.
  /// @param  flags   (Optional) The flags.
  /// @param  rows    (Optional) The rows.
  /// @param  cols    (Optional) The cols.
  /// @param  bgColor (Optional) The background color.
  ///
  /// @returns  An EsChPlot::Ptr.
  ///
  static EsChPlot::Ptr create(const wxSize& size, int flags = EsChPlotModeFlag_DEFAULT, ulong rows = 1, ulong cols = 1, const wxColor& bgColor = *wxWHITE);

  /// @brief  Create and prepare plot view(s), in container window.
  ///         Rows & cols define count of plot views available.
  ///
  /// @param [in]			container If non-null, the container window.
  /// @param          flags     (Optional) The flags.
  /// @param          rows      (Optional) The rows.
  /// @param          cols      (Optional) The cols.
  /// @param          bgColor   (Optional) The background color.
  ///
  /// @returns  An EsChPlot::Ptr.
  ///
  static EsChPlot::Ptr create(wxWindow* container, int flags = EsChPlotModeFlag_DEFAULT, ulong rows = 1, ulong cols = 1, const wxColor& bgColor = *wxWHITE);

  bool isOk() const;

  void sizeSet(const wxSize& size);
  wxSize sizeGet() const;

  wxColor backgroundColorGet() const;
  void backgroundColorSet(const wxColor& col);

  // redo plot
  void refresh() const;

  // draw (with optional stretching) cached canvas image to dc
  void draw(wxDC& dc, const wxPoint& pnt, const wxSize& ext) const;

  // draw cached image unstretched, at specified position 
  void draw(wxDC& dc, const wxPoint& pnt = wxPoint()) const { draw(dc, pnt, sizeGet()); }

  // plot api
  //
  // sub-views manipulation
  ulong columnsCountGet() const ES_NOTHROW { return m_cols; }
  ulong rowsCountGet() const ES_NOTHROW { return m_rows; }
  ulong viewsCountGet() const ES_NOTHROW { return m_cols * m_rows; }

  EsChPlotView::Ptr viewGet(ulong idx) const;
  EsChPlotView::Ptr viewSet(ulong idx, EsChPlotCoordinateSystem cs, const EsString& title = EsString::null());

  void viewClear(ulong idx);
  void viewSelect(ulong idx);

  ulong selectedViewIdxGet() const ES_NOTHROW { return m_activeView; }
  EsChPlotView::Ptr selectedViewGet() const;

  // plot axes and annotations on currently active subpage
  void plotAxesAndTitles() const;

  // low-level plot graphics api. all drawig operations apply to the currently active view
  //
  // text parameters application
  void textParamtersApply(const EsChPlotTextParameters::Ptr& params) const;
  
  // line parameters access
  void lineParametersApply(const EsChPlotLineParameters::Ptr& params) const;
  
  // fill parameters access
  void fillParametersApply(const EsChPlotFillParameters::Ptr& params) const;
  
  // point parameters access
  void pointParametersApply(const EsChPlotPointParameters::Ptr& params) const;

  // color application
  //
  void colorSelect(wxColor color) const;

  // coord mapping stuff
  //
  // retrieve world delta which corresponds to the relative viewport delta, 
  // for the currently active view
  double relativeDeltaToWorldDelta(ulong dim, double delta) const;

  // retrieve world delta which corresponds to the physical device delta, 
  // for the currently active view
  double physicalDeltaToWorldDelta(ulong dim, wxCoord delta) const;

  // retrieve absolute view coordinates delta in normalized relative view coordinates
  double absoluteDeltaToWorldDelta(ulong dim, double delta) const;

  // get currently active view area aspect ratios
  double widthToHeightRatioGet() const;
  double heightToWidthRatioGet() const;

  // calculate standard offset steps for the currently active 
  // view, in relative device coordinates
  double microOffsStepGet(ulong dim) const;
  double macroOffsStepGet(ulong dim) const;

  // convert absolute view coordinate to normalized relative view coordinate
  double absoluteToRelative(ulong dim, double absolute) const;

  // retrieve current scaled font height in relative view coordinates
  double textHeightToRelative(ulong dim, const EsChPlotTextParameters::Ptr& textParams) const;

  // 2d stuff
  //
  // plot text in relative coordinates
  void plotTextRelative(const EsString& txt, double x, double y, double justify = 0., double angle = 0.) const;

  // plot text in data coordinates
  void plotText(const EsString& txt, double x, double y, double justify = 0., double angle = 0.) const;

  // plot arc in view's data window coordinate space
  void plotArc(double majorR, double minorR, double centerX = 0., double centerY = 0., bool fill = false, double startAngle = 0., double endAngle = 360.) const;

  // plot line between two 2d points
  void plotLine(double x1, double y1, double x2, double y2) const;

  // plot continuous 2d polyline
  void plotLine(const EsMathArrayReal& x, const EsMathArrayReal& y, ulong offs = 0, ulong cnt = 0) const;

  // plot h or v 2d line
  void plotLineH(double x, double y, double w) const;
  void plotLineV(double x, double y, double h) const;

  // draw error bars
  void plotErrosX(const EsMathArrayReal& lowErrsX, const EsMathArrayReal& highErrsX, const EsMathArrayReal& y) const;
  void plotErrosY(const EsMathArrayReal& lowErrsY, const EsMathArrayReal& highErrsY, const EsMathArrayReal& x) const;

  // draw point markers
  void plotPoint(double x, double y) const;
  void plotPoints(const EsMathArrayReal& x, const EsMathArrayReal& y, ulong offs = 0, ulong cnt = 0) const;

  // draw filled polygon
  void plotPoly(const EsMathArrayReal& x, const EsMathArrayReal& y, ulong offs = 0, ulong cnt = 0) const;
  void plotPoly(const double* x, const double* y, ulong cnt = 0) const;

  // 3d stuff
  //
  // plot line between two 3d points
  void plotLine(double x1, double y1, double z1, double x2, double y2, double z2) const;

  // plot polyline in 3d
  void plotLine(const EsMathArrayReal& x, const EsMathArrayReal& y, const EsMathArrayReal& z) const;

  // hit test service
  bool hitInfoGet(wxCoord x, wxCoord y, EsChPlotObjectHitInfo& info) const;
  bool hitInfoGet(const wxPoint& pnt, EsChPlotObjectHitInfo& info) const { return hitInfoGet(pnt.x, pnt.y, info); }

protected:
  // internal services
  //
  void doRefresh() const;

  // EsChPlotObject overrides
  virtual void onRecalc(const EsChPlotObject& sender);
  virtual void onRedraw(const EsChPlotObject& sender) const;
  virtual void onUnlock();
  virtual bool hitInfoGet(const EsChPlot& plot, double wx, double wy, double toleranceX, double toleranceY, EsChPlotObjectHitInfo& info) const;

  // misc helpers
  void internalSelectView(ulong idx) const;
  void plotAxesAndTitlesCartesian2d(const EsChPlotView::Ptr& viewParams) const;
  void plotAxesAndTitlesPolar2d(const EsChPlotView::Ptr& viewParams) const;
  void plotPolar2dRadialsDeg(const EsChPlotAxis::Ptr& angularParams, const EsChPlotLineParameters::Ptr& axisLineParams, const EsChPlotLineParameters::Ptr& axisGridParams, double wminX, double wmaxX, double wminY, double wmaxY) const;
  
  // calculate and return view data window coordinates from view parameters
  void calculateViewWindow(const EsChPlotView::Ptr& viewParams, double& wminX, double& wmaxX, double& wminY, double& wmaxY) const;

  // scale offset value in relative coordinates, depending on the 
  // currently active page aspect ratio
  double scaleOffset(ulong dim, double offs) const;

  // checks
  void viewIndexCheck(ulong idx) const;

  // view clearing helper
  void internalViewClear(EsChPlotView::Ptr& view);

  // plplot low-level handlers
  static int exitHandler(const char* msg);

private:
  // optional containing window
  wxWindow* m_container;
  // drawing memory canvas
  wxImage m_image;
  wxBitmap m_canvas;
  mutable wxMemoryDC m_canvasDc;
  EsChPlotStreamAdapter* m_plstream;
  int m_style;
  mutable bool m_needRefresh;
  ulong m_rows;
  ulong m_cols;
  mutable ulong m_activeView;
  EsChPlotView::Array m_views;
  // misc cache
  mutable bool m_lineIsNone;
  mutable bool m_fillIsNone;
  mutable bool m_pointIsNone;
  mutable ulong m_pointStyle;

  ES_NON_COPYABLE(EsChPlot);

  friend class EsChPlotObject;
};

#endif // _es_ch_plot_h_
