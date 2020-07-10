#ifndef _es_ch_plot_text_parameters_h_
#define _es_ch_plot_text_parameters_h_

class ESCHART_CLASS EsChPlotTextParameters : public EsChPlotObject
{
public:
	enum
	{
		minHeight = 1,
		defHeight = 3,
		maxHeight = 5,
	};
	typedef std::shared_ptr<EsChPlotTextParameters> Ptr;

protected:
	EsChPlotTextParameters(EsChPlotObject* owner);

public:
	static EsChPlotTextParameters::Ptr create(EsChPlotObject* owner = 0)
	{
		return EsChPlotTextParameters::Ptr(new EsChPlotTextParameters(owner));
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

	bool italicGet() const
	{
		return m_italic;
	}
	void italicSet(bool italic)
	{
		if( m_italic != italic )
		{
			m_italic = italic;
			notifyRedraw();
		}
	}

	bool boldGet() const
	{
		return m_bold;
	}
	void boldSet(bool bold)
	{
		if( m_bold != bold )
		{
			m_bold = bold;
			notifyRedraw();
		}
	}

	double heightGet() const
	{
		return m_height;
	}
	void heightSet(double height)
	{
		if( !EsMath::areEqualFloats(m_height, height) )
		{
			m_height = height;
			notifyRedraw();
		}
	}

	void copyFrom(const EsChPlotTextParameters::Ptr& other);

protected:
	wxColor m_color;
	double m_height;
	bool m_italic;
	bool m_bold;
};

#endif // _es_ch_plot_text_parameters_h_
