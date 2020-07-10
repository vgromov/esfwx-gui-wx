#ifndef _es_ch_plot_palette_h_
#define _es_ch_plot_palette_h_

// Forward declaration
class EsChPlotStreamAdapter;

// plot palette type
//
enum EsChPlotPalette {
	EsChPlotPalette_DISCRETE,
	EsChPlotPalette_CONTINUOUS,
};

// plot palettes
//
class ESCHART_CLASS EsChPlotPaletteDiscrete
{
public:
	// default palette colors
	enum {
		STD_WHITE,
		STD_BLACK,
		STD_RED,
		STD_GREEN,
		STD_BLUE,
		STD_LIGHT_GREY,
		STD_CYAN,
		STD_YELLOW,
		STD_WHITE_50,
		STD_BLACK_50,
		STD_RED_50,
		STD_GREEN_50,
		STD_BLUE_50,
		STD_LIGHT_GREY_50,
		STD_CYAN_50,
		STD_YELLOW_50,
		STD_WHITE_20,
		STD_BLACK_20,
		STD_RED_20,
		STD_GREEN_20,
		STD_BLUE_20,
		STD_LIGHT_GREY_20,
		STD_CYAN_20,
		STD_YELLOW_20,
	};
	
public:
	// palette api
	void clear();
	size_t colorCountGet() const
	{
		return m_r.size();
	}
	void colorCountReserve(size_t reserve);
	void colorAdd(const wxColor& color);
	void colorAdd(const wxColor& color, double alpha);
	void colorSet(size_t idx, const wxColor& color);
	void colorSet(size_t idx, const wxColor& color, double alpha);
	wxColor colorGet(size_t idx) const;
	// default static palettes
	static const EsChPlotPaletteDiscrete& defaultGet();

protected:
	// internal services
	void initFromPlot(EsChPlotStreamAdapter* plot);
		
protected:
	typedef std::vector<wxInt32> Colors;
	typedef std::vector<double> Alphas;
	Colors m_r;
	Colors m_g;
	Colors m_b;
	Alphas m_a;
	
	friend class EsChPlot;
};


#endif // _es_ch_plot_palette_h_
