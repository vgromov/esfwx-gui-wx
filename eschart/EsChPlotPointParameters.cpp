#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotPointParameters.h"

EsChPlotPointParameters::EsChPlotPointParameters(EsChPlotObject* owner) :
EsChPlotObject(owner),
m_style(EsChPlotPointStyle_SQUARE),
m_size(4),
m_color(*wxBLACK)
{
}

// assignment
void EsChPlotPointParameters::copyFrom(const EsChPlotPointParameters::Ptr& other)
{
	ES_ASSERT(other);
	ES_ASSERT(this != other.get());

	if( m_style != other->styleGet() ||
			m_size != other->sizeGet() ||
			m_color != other->colorGet() )
	{
		m_style = other->styleGet();
		m_size = other->sizeGet();
		m_color = other->colorGet();
		notifyRedraw();
	}
}
