#ifndef _es_ch_plot_series_h_
#define _es_ch_plot_series_h_

// chart series interface abstraction
//
enum EsChPlotSeriesType
{
	EsChPlotSeriesType_1D,
	EsChPlotSeriesType_2D,
	EsChPlotSeriesType_3D
};

class ESCHART_CLASS EsChPlotSeries : public EsChPlotObject
{
public:
	typedef std::shared_ptr<EsChPlotSeries> Ptr;
	typedef std::vector<Ptr> Array;

protected:
	EsChPlotSeries(EsChPlotSeriesType type, size_t size);

public:
	// series factory
	static EsChPlotSeries::Ptr create1d(size_t defSize = 0);
	static EsChPlotSeries::Ptr create2d(size_t defSize = 0);
	static EsChPlotSeries::Ptr create3d(size_t defSize = 0);
	// services
	//
	EsChPlotSeriesType typeGet() const
	{
		return m_type;
	}
	ulong dimensionGet() const
	{
		return (m_type < EsChPlotSeriesType_3D) ? 2 : 3;
	}
	// name manipulation
	const EsString& nameGet() const
	{
		return m_name;
	}
	void nameSet(const EsString& name);
	
	// checks
	bool isEmpty() const
	{
		return m_data[0].get_empty();
	}

	// size manipulations
	ulong sizeGet() const { return m_data[0].get_count(); }
	void sizeSet(ulong size);

	void clear();

	// min, max, total access
	double minGet(ulong dim) const;
	ulong minPosGet(ulong dim) const;
	double maxGet(ulong dim) const;
	ulong maxPosGet(ulong dim) const;
	void minMaxGet(ulong dim, double& min, double& max) const;
	void minMaxPosGet(ulong dim, ulong& minPos, ulong& maxPos) const;

	double totalGet(ulong dim) const;

	// data access
	const EsMathArrayReal& dataGet(ulong dim) const;
	EsMathArrayReal& dataGet(ulong dim);
	void dataSet(ulong dim, const EsMathArrayReal& data);
	double dataGetAt(ulong dim, ulong pos) const;
	void dataSetAt(ulong dim, ulong pos, double val);
	void pointSetAt(ulong pos, double _1, double _2, double _3 = 0.);

protected:
	// internal services
	//
	// internal size change
	void internalSizeSet(size_t size);
	// min max reset
	void minMaxReset();
	// update minimax
	void minMaxUpdate(ulong dim);
	void minMaxUpdate();
	// checks
	void dimensionCheck(ulong dim) const;
	void dataIndexCheck(ulong pos) const;
	// base class override
	virtual void onUnlock();
	
protected:
	EsChPlotSeriesType m_type;
	EsString m_name;

	// data && minmax
	EsMathArrayReal m_data[3];

	friend class EsChPlot;
};

#endif // _es_ch_plot_series_h_