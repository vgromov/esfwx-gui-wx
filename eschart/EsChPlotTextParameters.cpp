#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotTextParameters.h"

EsChPlotTextParameters::EsChPlotTextParameters(EsChPlotObject* owner) :
EsChPlotObject(owner),
m_color(*wxBLACK),
m_italic(false),
m_bold(false),
m_height(3)
{
}

void EsChPlotTextParameters::copyFrom(const EsChPlotTextParameters::Ptr& other)
{
	ES_ASSERT(other);
	ES_ASSERT(this != other.get());

	if( m_color != other->colorGet() ||
			m_italic != other->italicGet() ||
			m_bold != other->boldGet() ||
			!EsMath::areEqualFloats(m_height, other->heightGet()) )
	{
		m_color = other->colorGet();
		m_italic = other->italicGet();
		m_bold = other->boldGet();
		m_height = other->heightGet();
		notifyRedraw();
	}
}
