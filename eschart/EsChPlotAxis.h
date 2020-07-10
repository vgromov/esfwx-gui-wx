#ifndef _es_ch_plot_axis_h_
#define _es_ch_plot_axis_h_

// plot axis parameters
//
// plot axes appearance flags
enum {
	EsChPlotAxis_AXIS_VISIBLE									= 0x00000001,
	EsChPlotAxis_FRAME_VISIBLE								= 0x00000002,
	EsChPlotAxis_LABELS_FMT_DATETIME					= 0x00000004,
	EsChPlotAxis_LABELS_FMT_FIXEDPOINT				= 0x00000008,
	EsChPlotAxis_GRID_AT_MAJOR_TICKS					= 0x00000010,
	EsChPlotAxis_GRID_AT_MINOR_TICKS					= 0x00000020,
	EsChPlotAxis_TICK_MARKS_OUTSIDE						= 0x00000040,
	EsChPlotAxis_LOGARITHMIC									= 0x00000080,
	EsChPlotAxis_LABELS_VISIBLE								= 0x00000100,
	EsChPlotAxis_LABELS_AT_OPPOSITE_LOCATION  = 0x00000200, 
	EsChPlotAxis_LABELS_FMT_CUSTOM						= 0x00000400,
	EsChPlotAxis_TICKS_VISIBLE								= 0x00000800,
	EsChPlotAxis_TICKS_MINOR_VISIBLE					= 0x00001000,
	EsChPlotAxis_TITLE_VISIBLE								= 0x00004000,
	EsChPlotAxis_AUTO_MIN											= 0x00008000,
	EsChPlotAxis_AUTO_MAX											= 0x00010000,
	// masks
	//
	// grid only
	EsChPlotAxis_GRID_MASK										= EsChPlotAxis_GRID_AT_MAJOR_TICKS|
																							EsChPlotAxis_GRID_AT_MINOR_TICKS,
	// default flags set
	EsChPlotAxis_DEFAULT_FLAGS								= EsChPlotAxis_AXIS_VISIBLE|
																							EsChPlotAxis_FRAME_VISIBLE|
																							EsChPlotAxis_GRID_AT_MAJOR_TICKS|
																							EsChPlotAxis_TICK_MARKS_OUTSIDE|
																							EsChPlotAxis_LABELS_VISIBLE|
																							EsChPlotAxis_TICKS_VISIBLE|
																							EsChPlotAxis_TITLE_VISIBLE|
																							EsChPlotAxis_AUTO_MIN|
																							EsChPlotAxis_AUTO_MAX,
};

// plot coordinate system ids
//
enum EsChPlotCoordinateSystem {
	// 2d systems
	EsChPlotCoordinateSystem_CARTESIAN_2D,
	EsChPlotCoordinateSystem_POLAR_DEG_2D,
	EsChPlotCoordinateSystem_POLAR_RAD_2D,
	// 3d systems
	EsChPlotCoordinateSystem_CARTESIAN_3D,
	EsChPlotCoordinateSystem_SPHERICAL_3D,
	EsChPlotCoordinateSystem_CYLINDER_3D,
	// special value must go last
	EsChPlotCoordinateSystem_END
};

// axis symbolic indexes
//
enum {
	// cartesian
	EsChPlot_AXIS_CARTESIAN_X = 0,
	EsChPlot_AXIS_CARTESIAN_Y = 1,
	EsChPlot_AXIS_CARTESIAN_Z = 2,
	// polar 
	EsChPlot_AXIS_POLAR_R			= 0,
	EsChPlot_AXIS_POLAR_PHI		= 1,
};

class ESCHART_CLASS EsChPlotAxis : public EsChPlotObject
{
public:
	typedef std::shared_ptr<EsChPlotAxis> Ptr;

protected:
	static const double defaultMin;
	static const double defaultMax;

	EsChPlotAxis(EsChPlotObject* owner, EsChPlotCoordinateSystem csId, ulong dim, const EsString& title = EsString::null());

	void copyFrom(const EsChPlotAxis::Ptr& other);

public:
	bool isOk() const
	{
		return m_csId < EsChPlotCoordinateSystem_END &&
					(m_csId < EsChPlotCoordinateSystem_CARTESIAN_3D) ? (m_dim < 2) : (m_dim < 3);
	}
	EsChPlotCoordinateSystem coordinateSystemGet() const
	{
		return m_csId;
	}
	ulong dimensionGet() const
	{
		return m_dim;
	}
	// flags manipulation
	ulong flagsGet() const
	{
		return m_flags;
	}
	bool isAutoMin() const
	{
		return EsChPlotAxis_AUTO_MIN == (m_flags & EsChPlotAxis_AUTO_MIN);
	}
	void autoMinSet(bool autoMin);
	bool isAutoMax() const
	{
		return EsChPlotAxis_AUTO_MAX == (m_flags & EsChPlotAxis_AUTO_MAX);
	}
	void autoMaxSet(bool autoMax);
	void autoMinMaxSet(bool autoMin, bool autoMax);
	// tick manipulation
	bool areLabelsVisible() const
	{
		return EsChPlotAxis_LABELS_VISIBLE == (m_flags & EsChPlotAxis_LABELS_VISIBLE);
	}
	bool areTicksVisible() const
	{
		return EsChPlotAxis_TICKS_VISIBLE == (m_flags & EsChPlotAxis_TICKS_VISIBLE);
	}
	double majorTickStepGet() const
	{
		return m_majorTickStep;
	}
	void majorTickStepSet(double step);
	ulong minorTicksPerMajorTickGet() const
	{
		return m_minorTicksPerMajorTick;
	}
	void minorTicksPerMajorTickSet(ulong cnt);
	// axis title
	const EsString titleGet() const
	{
		return m_title;
	}
	void titleSet(const EsString& title)
	{
		if( m_title != title )
		{
			m_title = title;
			notifyRedraw();
		}
	}
	bool isTitleVisible() const
	{
		return EsChPlotAxis_TITLE_VISIBLE == (m_flags & EsChPlotAxis_TITLE_VISIBLE) &&
			!m_title.empty();
	}
	// minimax manupulation
	double minGet() const
	{
		return m_min;
	}
	double maxGet() const
	{
		return m_max;
	}	
	void minMaxGet(double& min, double& max) const
	{
		min = m_min; max = m_max;
	}
	void minMaxSet(double min, double max);
	void minSet(double min);
	void maxSet(double max);
	
protected:
	void internalMinMaxSet(double min, double max);
	void internalMinSet(double min);
	void internalMaxSet(double max);
	static std::string optsFromFlags(ulong flags);

protected:	
	EsChPlotCoordinateSystem m_csId;
	ulong m_dim;
	ulong m_flags;
	double m_customOrigin;
	double m_majorTickStep;
	ulong m_minorTicksPerMajorTick;
	EsString m_title;
	double m_min;
	double m_max;
	
	friend class EsChPlotView;
	friend class EsChPlot;
};

#endif // _es_ch_plot_axis_h_