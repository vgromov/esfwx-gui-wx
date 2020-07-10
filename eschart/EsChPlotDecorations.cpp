#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotDecorations.h"
//--------------------------------------------------------------------------------

EsChPlotLineMarker::EsChPlotLineMarker(bool vertical) :
m_vertical(vertical),
m_pos(0)
{
	m_line = EsChPlotLineParameters::create(this);
}
//--------------------------------------------------------------------------------

EsChPlotLineMarker::Ptr EsChPlotLineMarker::createVertical()
{
	return EsChPlotLineMarker::Ptr( new EsChPlotLineMarker(true) );
}
//--------------------------------------------------------------------------------

EsChPlotLineMarker::Ptr EsChPlotLineMarker::createHorizontal()
{
	return EsChPlotLineMarker::Ptr( new EsChPlotLineMarker(false) );
}
//--------------------------------------------------------------------------------

void EsChPlotLineMarker::positionSet(double pos)
{
	if(m_pos != pos)
	{
		m_pos = pos;
		notifyRedraw();
	}
}
//--------------------------------------------------------------------------------

void EsChPlotLineMarker::render(const EsChPlot& plot) const
{
	EsChPlotView::Ptr view = plot.selectedViewGet();
	ES_ASSERT(view);

	EsChPlotAxis::Ptr axis = view->axisGet( m_vertical ? 1 : 0 );
	ES_ASSERT(axis);

	double x0, x1, y0, y1;

	if( m_vertical )
	{
		x0 = x1 = m_pos;
		axis->minMaxGet(y0, y1);
	}
	else
	{
		y0 = y1 = m_pos;
		axis->minMaxGet(x0, x1);
	}

	plot.lineParametersApply(m_line);
	plot.plotLine(x0, y0, x1, y1);
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChPlotAreaMarker::EsChPlotAreaMarker(bool vertical) :
m_vertical(vertical),
m_start(0),
m_end(0)
{
	m_line = EsChPlotLineParameters::create(this);
	m_fill = EsChPlotFillParameters::create(this);
}
//--------------------------------------------------------------------------------

EsChPlotAreaMarker::Ptr EsChPlotAreaMarker::createVertical()
{
	return EsChPlotAreaMarker::Ptr( new EsChPlotAreaMarker(true) );
}
//--------------------------------------------------------------------------------

EsChPlotAreaMarker::Ptr EsChPlotAreaMarker::createHorizontal()
{
	return EsChPlotAreaMarker::Ptr( new EsChPlotAreaMarker(false) );
}
//--------------------------------------------------------------------------------

void EsChPlotAreaMarker::startSet(double start)
{
	EsNumericCheck::checkRangeFloat(
		-std::numeric_limits<double>::max(),
		m_end, 
		start, 
		_("Area marker start")
	);

	if(m_start != start)
	{
		m_start = start;
		notifyRedraw();
	}
}
//--------------------------------------------------------------------------------

void EsChPlotAreaMarker::endSet(double end)
{
	EsNumericCheck::checkRangeFloat(
		m_start, 
		std::numeric_limits<double>::max(), 
		end, 
		_("Area marker end")
	);

	if(m_end != end)
	{
		m_end = end;
		notifyRedraw();
	}
}
//--------------------------------------------------------------------------------

void EsChPlotAreaMarker::startEndSet(double start, double end)
{
	if( start <= end )
	{
		if( m_start != start ||
				m_end != end )
		{
			m_start = start;
			m_end = end;
			notifyRedraw();
		}
	}
	else
		EsException::Throw(_("Area marker start must not be greater than the end"));
}
//--------------------------------------------------------------------------------

void EsChPlotAreaMarker::render(const EsChPlot& plot) const
{
	EsChPlotView::Ptr view = plot.selectedViewGet();
	ES_ASSERT(view);

	EsChPlotAxis::Ptr axis = view->axisGet( m_vertical ? 1 : 0 );
	ES_ASSERT(axis);

	double x[4];
	double y[4];

	if( m_vertical )
	{
		x[0] = x[1] = m_start;
		x[2] = x[3] = m_end;

		axis->minMaxGet(y[0], y[1]);
		y[2] = y[1];
		y[3] = y[0];
	}
	else
	{
		y[0] = y[3] = m_start;
		y[1] = y[2] = m_end;

		axis->minMaxGet(x[0], x[2]);
		x[1] = x[0];
		x[3] = x[2];
	}

	plot.fillParametersApply(m_fill);
	plot.plotPoly(
		x, 
		y,
		4
	);

	plot.lineParametersApply(m_line);
	if( m_vertical )
	{
		plot.plotLine(
			x[0], 
			y[0], 
			x[1], 
			y[1]
		);
		
		plot.plotLine(
			x[2], 
			y[2], 
			x[3], 
			y[3]
		);
	}
	else
	{
		plot.plotLine(
			x[1], 
			y[1], 
			x[2], 
			y[2]
		);
		
		plot.plotLine(
			x[0], 
			y[0], 
			x[3], 
			y[3]
		);
	}
}