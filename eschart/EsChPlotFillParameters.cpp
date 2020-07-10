#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotFillParameters.h"

EsChPlotFillParameters::EsChPlotFillParameters(EsChPlotObject* owner) :
EsChPlotObject(owner),
m_style(EsChPlotFillStyle_SOLID),
m_color(*wxLIGHT_GREY)
{
}

// assignment
void EsChPlotFillParameters::copyFrom(const EsChPlotFillParameters::Ptr& other)
{
	ES_ASSERT(other);
	ES_ASSERT(this != other.get());
	
	if( m_style != other->styleGet() ||
		m_color != other->colorGet() )
	{
		m_style = other->styleGet();
		m_color = other->colorGet();
		notifyRedraw();
	}
}
