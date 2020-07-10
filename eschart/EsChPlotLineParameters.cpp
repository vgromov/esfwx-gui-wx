#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotLineParameters.h"

// plot line parameters
//
EsChPlotLineParameters::EsChPlotLineParameters(EsChPlotObject* owner) :
EsChPlotObject(owner),
m_style(EsChPlotLineStyle_SOLID),
m_width(0),
m_color(*wxBLACK)
{
}

// assignment
void EsChPlotLineParameters::copyFrom(const EsChPlotLineParameters::Ptr& other)
{
	ES_ASSERT(other);
	ES_ASSERT(this != other.get());

	if(	m_style != other->styleGet() ||
			m_width != other->widthGet() ||
			m_color != other->colorGet() )
	{
		m_style = other->styleGet();
		m_width = other->widthGet();
		m_color = other->colorGet();
		notifyRedraw();
	}
}
