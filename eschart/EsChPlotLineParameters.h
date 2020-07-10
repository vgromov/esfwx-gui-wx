#ifndef _es_ch_plot_line_parameters_h_
#define _es_ch_plot_line_parameters_h_

// line parameters encapsulation
//
// line styles
enum EsChPlotLineStyle {
	EsChPlotLineStyle_NONE = 0,
	EsChPlotLineStyle_SOLID,
	EsChPlotLineStyle_SHORT_DASH_SHORT_GAP,
	EsChPlotLineStyle_LONG_DASH_LONG_GAP,
	EsChPlotLineStyle_LONG_DASH_SHORT_GAP,
	EsChPlotLineStyle_5,
	EsChPlotLineStyle_6,
	EsChPlotLineStyle_7,
	EsChPlotLineStyle_8
};

class ESCHART_CLASS EsChPlotLineParameters : public EsChPlotObject
{
public:
	typedef std::shared_ptr<EsChPlotLineParameters> Ptr;

protected:
	EsChPlotLineParameters(EsChPlotObject* owner);

public:
	static EsChPlotLineParameters::Ptr create(EsChPlotObject* owner = 0)
	{
		return EsChPlotLineParameters::Ptr( new EsChPlotLineParameters(owner) );
	}

	EsChPlotLineStyle styleGet() const ES_NOTHROW {	return m_style;	}
	void styleSet(EsChPlotLineStyle style)
	{
		if( m_style != style )
		{
			m_style = style;
			notifyRedraw();
		}
	}

	ulong widthGet() const ES_NOTHROW { return m_width; }
	void widthSet(ulong width)
	{
		if( m_width != width )
		{
			m_width = width;
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
		return EsChPlotLineStyle_NONE == m_style;
	}
	// assignment
	void copyFrom(const EsChPlotLineParameters::Ptr& other);

protected:
	EsChPlotLineStyle m_style;
	ulong m_width;
	wxColor m_color;
};

#endif // _es_ch_plot_line_parameters_h_