#ifndef _es_ch_plot_point_parameters_h_
#define _es_ch_plot_point_parameters_h_

// plot point marker parameters encapsulation
//
enum EsChPlotPointStyle 
{
	EsChPlotPointStyle_SQUARE,
	EsChPlotPointStyle_DOT,
	EsChPlotPointStyle_CROSS,
	EsChPlotPointStyle_STAR,
	EsChPlotPointStyle_CIRCLE,
	EsChPlotPointStyle_DIAG_CROSS,
	EsChPlotPointStyle_TRIANGLE = EsChPlotPointStyle_DIAG_CROSS+2,
	EsChPlotPointStyle_CIRCLE_CROSS,
	EsChPlotPointStyle_CIRCLE_DOT,
	EsChPlotPointStyle_DIAMOND = EsChPlotPointStyle_CIRCLE_DOT+2,
	EsChPlotPointStyle_NONE
};

class ESCHART_CLASS EsChPlotPointParameters : public EsChPlotObject
{
public:
	typedef std::shared_ptr<EsChPlotPointParameters> Ptr;

protected:
	EsChPlotPointParameters(EsChPlotObject* owner);

public:
	static EsChPlotPointParameters::Ptr create(EsChPlotObject* owner = 0)
	{
		return EsChPlotPointParameters::Ptr( new EsChPlotPointParameters(owner) );
	}

	EsChPlotPointStyle styleGet() const
	{
		return m_style;
	}
	void styleSet(EsChPlotPointStyle style)
	{
		if( m_style != style )
		{
			m_style = style;
			notifyRedraw();
		}
	}
	double sizeGet() const
	{
		return m_size;
	}
	void sizeSet(double size)
	{
		if( m_size != size )
		{
			m_size = size;
			notifyRedraw();
		}
	}
	wxColor colorGet() const
	{
		return m_color;
	}
	void colorSet(wxColor color)
	{
		if(	m_color != color )
		{
			m_color = color;
			notifyRedraw();
		}
	}	
	// null check
	bool isNone() const
	{
		return EsChPlotPointStyle_NONE == m_style;
	}
	// assignment
	void copyFrom(const EsChPlotPointParameters::Ptr& other);

protected:
	EsChPlotPointStyle m_style;
	double m_size;
	wxColor m_color;
};

#endif // _es_ch_plot_point_parameters_h_