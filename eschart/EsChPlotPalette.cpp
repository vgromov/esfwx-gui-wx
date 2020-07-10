#include "eschart_pch.h"
#pragma hdrstop

#include <plplotP.h>
#include <plstream.h>
#include "EsChPlotStreamAdapter.h"
#include "EsChPlotPalette.h"

// color palette
//
const EsChPlotPaletteDiscrete& EsChPlotPaletteDiscrete::defaultGet()
{
	static EsChPlotPaletteDiscrete s_default;
	if( !s_default.colorCountGet() )
	{
		s_default.colorCountReserve(16);
		s_default.colorAdd(*wxWHITE);
		s_default.colorAdd(*wxBLACK);
		s_default.colorAdd(*wxRED);
		s_default.colorAdd(*wxGREEN);
		s_default.colorAdd(*wxBLUE);
		s_default.colorAdd(*wxLIGHT_GREY);
		s_default.colorAdd(*wxCYAN);
		s_default.colorAdd(*wxYELLOW);
		// 50% transparency
		s_default.colorAdd(s_default.colorGet(STD_WHITE), 0.5);
		s_default.colorAdd(s_default.colorGet(STD_BLACK), 0.5);
		s_default.colorAdd(s_default.colorGet(STD_RED), 0.5);
		s_default.colorAdd(s_default.colorGet(STD_GREEN), 0.5);
		s_default.colorAdd(s_default.colorGet(STD_BLUE), 0.5);
		s_default.colorAdd(s_default.colorGet(STD_LIGHT_GREY), 0.5);
		s_default.colorAdd(s_default.colorGet(STD_CYAN), 0.5);
		s_default.colorAdd(s_default.colorGet(STD_YELLOW), 0.5);
		// 80% transparency
		s_default.colorAdd(s_default.colorGet(STD_WHITE), 0.2);
		s_default.colorAdd(s_default.colorGet(STD_BLACK), 0.2);
		s_default.colorAdd(s_default.colorGet(STD_RED), 0.2);
		s_default.colorAdd(s_default.colorGet(STD_GREEN), 0.2);
		s_default.colorAdd(s_default.colorGet(STD_BLUE), 0.2);
		s_default.colorAdd(s_default.colorGet(STD_LIGHT_GREY), 0.2);
		s_default.colorAdd(s_default.colorGet(STD_CYAN), 0.2);
		s_default.colorAdd(s_default.colorGet(STD_YELLOW), 0.2);
	}

	return s_default;
}

void EsChPlotPaletteDiscrete::clear()
{
	m_r.clear();
	m_g.clear();
	m_b.clear();
	m_a.clear();
}

void EsChPlotPaletteDiscrete::colorCountReserve(size_t reserve)
{
	m_r.reserve(reserve);
	m_g.reserve(reserve);
	m_b.reserve(reserve);
	m_a.reserve(reserve);
}

void EsChPlotPaletteDiscrete::colorAdd(const wxColor& color)
{
	colorAdd(color, static_cast<double>(color.Alpha())/static_cast<double>(wxALPHA_OPAQUE));
}

void EsChPlotPaletteDiscrete::colorAdd(const wxColor& color, double alpha)
{
	m_r.push_back(color.Red());
	m_g.push_back(color.Green());
	m_b.push_back(color.Blue());
	m_a.push_back(alpha);
}

void EsChPlotPaletteDiscrete::colorSet(size_t idx, const wxColor& color)
{
	colorSet(idx, color, static_cast<double>(color.Alpha())/static_cast<double>(wxALPHA_OPAQUE));
}

void EsChPlotPaletteDiscrete::colorSet(size_t idx, const wxColor& color, double alpha)
{
	EsNumericCheck::checkRangeInteger(0, m_r.empty() ? 0 : m_r.size()-1, idx, wxT("Discrete palette color index"));
	m_r[idx] = color.Red();
	m_g[idx] = color.Green();
	m_b[idx] = color.Blue();
	m_a[idx] = alpha;
}

wxColor EsChPlotPaletteDiscrete::colorGet(size_t idx) const
{
	EsNumericCheck::checkRangeInteger(0, m_r.empty() ? 0 : m_r.size()-1, idx, wxT("Discrete palette color index"));
	return wxColor(m_r[idx], m_g[idx], m_b[idx], static_cast<wxColor::ChannelType>(static_cast<double>(wxALPHA_OPAQUE)*m_a[idx]));
}

// internal services
//
void EsChPlotPaletteDiscrete::initFromPlot(EsChPlotStreamAdapter* plot)
{
	ES_ASSERT(plot);
		
	clear();
	PLINT r, g, b;
	PLFLT a;
	size_t idx = 0;
	colorCountReserve(32);
	while(true)
	{
		plot->gcol0a(idx++, r, g, b, a);
		if( 0 > r || 0 > g || 0 > b )
			break;
		
		m_r.push_back(r);
		m_g.push_back(g);
		m_b.push_back(b);
		m_a.push_back(a);
	}
}
