#ifndef _es_ch_plot_fill_parameters_h_
#define _es_ch_plot_fill_parameters_h_

// fill parameters encapsulation
//
// fill styles
enum EsChPlotFillStyle {
	EsChPlotFillStyle_SOLID = 0,
	EsChPlotFillStyle_HORIZONTAL_LINES,
	EsChPlotFillStyle_VERTICAL_LINES,
	EsChPlotFillStyle_RIGHT_45_LINES,
	EsChPlotFillStyle_LEFT_45_LINES,
	EsChPlotFillStyle_RIGHT_30_LINES,
	EsChPlotFillStyle_LEFT_30_LINES,
	EsChPlotFillStyle_CROSS_HATCH,
	EsChPlotFillStyle_DIAGONAL_HATCH,
	EsChPlotFillStyle_NONE
};

class ESCHART_CLASS EsChPlotFillParameters : public EsChPlotObject
{
public:
	typedef std::shared_ptr<EsChPlotFillParameters> Ptr;

protected:
	EsChPlotFillParameters(EsChPlotObject* owner);
	
public:	
	static EsChPlotFillParameters::Ptr create(EsChPlotObject* owner = 0)
	{
		return EsChPlotFillParameters::Ptr(new EsChPlotFillParameters(owner));
	}

	EsChPlotFillStyle styleGet() const
	{
		return m_style;
	}
	void styleSet(EsChPlotFillStyle style)
	{
		if( m_style != style )
		{
			m_style = style;
			notifyRedraw();
		}
	}
	wxColor colorGet() const
	{
		return m_color;
	}
	void colorSet(wxColor color)
	{
		if( m_color != color )
		{
			m_color = color;
			notifyRedraw();
		}
	}	
	// null check
	bool isNone() const
	{
		return EsChPlotFillStyle_NONE == m_style;
	}
	// values assignment
	void copyFrom(const EsChPlotFillParameters::Ptr& other);

protected:
	EsChPlotFillStyle m_style;
	wxColor m_color;
};

#endif // _es_ch_plot_fill_parameters_h_