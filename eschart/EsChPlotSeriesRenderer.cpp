#include "eschart_pch.h"
#pragma hdrstop

#include <plplotP.h>
#include "EsChPlotSeriesRenderer.h"
//--------------------------------------------------------------------------------

EsChPlotSeriesRenderer::EsChPlotSeriesRenderer() :
EsChPlotObject(),
m_invalid(true)
{}
//--------------------------------------------------------------------------------

EsChPlotSeriesRenderer::~EsChPlotSeriesRenderer()
{
	// just in case
	unlinkSeries();
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRenderer::linkSeries(const EsChPlotSeries::Ptr& series)
{
	ES_ASSERT(series);
	if( m_series != series )
	{
		m_series = series;
		// subscribe ourselves to the series events
		m_series->subscriberAdd(this);
		invalidate();
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRenderer::unlinkSeries()
{
	if( m_series )
	{
		m_series->subscriberRemove(this);
		m_series.reset();
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRenderer::onRecalc(const EsChPlotObject& sender)
{
	if( &sender == m_series.get() )
		invalidate();
	else
		EsChPlotObject::onRecalc(sender);
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRenderer::onRedraw(const EsChPlotObject& sender) const
{
	if( &sender == m_series.get() )
		invalidate();
	else
		EsChPlotObject::onRedraw(sender);
}
//--------------------------------------------------------------------------------

bool EsChPlotSeriesRenderer::isVisible(const EsChPlot& ES_UNUSED(plot)) const
{
	return !m_series->isEmpty();
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRenderer::optimize(const EsChPlot& plot) const 
{
	if( m_invalid &&
			isVisible(plot) )
	{
		doOptimize(plot);
		m_invalid = false;
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRenderer::render(const EsChPlot& plot) const
{
	optimize(plot);

	if( isVisible(plot) )
		doRender(plot);
}
//--------------------------------------------------------------------------------

bool EsChPlotSeriesRenderer::hitInfoGet(const EsChPlot& plot, double wx, double wy, double toleranceX, double toleranceY,	EsChPlotObjectHitInfo& info) const
{
	optimize(plot);	

	return EsChPlotObject::hitInfoGet(
		plot, 
		wx, 
		wy, 
		toleranceX, 
		toleranceY, 
		info
	);
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChPlotSeriesRendererCartesian2d::EsChPlotSeriesRendererCartesian2d(const EsChPlotLineParameters::Ptr& line, const EsChPlotFillParameters::Ptr& fill, const EsChPlotPointParameters::Ptr& point) :
m_cnt(0),
m_x(cacheChunkSize),
m_y(cacheChunkSize)
{
	m_line = EsChPlotLineParameters::create(this);
	m_fill = EsChPlotFillParameters::create(this);
	m_point = EsChPlotPointParameters::create(this);

	if( line )
		m_line->copyFrom(line);

	if( fill )
		m_fill->copyFrom(fill);
	
	if( point )
		m_point->copyFrom(point);
}
//--------------------------------------------------------------------------------

EsChPlotSeriesRendererCartesian2d::Ptr EsChPlotSeriesRendererCartesian2d::create(const EsChPlotLineParameters::Ptr& line, const EsChPlotFillParameters::Ptr& fill, const EsChPlotPointParameters::Ptr& point)
{
	return ES_MAKE_SHARED(
		EsChPlotSeriesRendererCartesian2d,
		line, 
		fill, 
		point
	);
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererCartesian2d::cachePoly(size_t offs, size_t cnt) const
{
	m_pcnt = 0;
	if( m_px.get_empty() )
	{
		m_px.set_count(PL_MAXPOLY);
		m_py.set_count(PL_MAXPOLY);
	}
	
	m_px.itemSet(
		m_pcnt,
		m_x.itemGet(offs)
	);
	
	m_py.itemSet(
		m_pcnt,
		0.
	);
	++m_pcnt;

	size_t end = cnt+offs;
	for(size_t idx = offs; idx < end; ++idx)
	{
		m_px.itemSet(
			m_pcnt,
			m_x.itemGet(idx)
		);
		
		m_py.itemSet(
			m_pcnt++,
			m_y.itemGet(idx)
		);
	}
	m_px.itemSet(
		m_pcnt,
		m_x.itemGet(end-1)
	);
	
	m_py.itemSet(
		m_pcnt++,
		0.
	);
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererCartesian2d::renderPoly(const EsChPlot& plot) const
{
	if( m_fill->isNone() )
		return;

	size_t size = m_cnt;
	if( size > PL_MAXPOLY )
	{
		--size;

		plot.fillParametersApply(m_fill);
		size_t offs = 0;
		size_t n = wxMin(size, PL_MAXPOLY-1);
		
		do
		{
			cachePoly(offs, n);
			plot.plotPoly(
				m_px, 
				m_py, 
				0, 
				m_pcnt
			);

			offs += n;
			n = wxMin(size-offs, PL_MAXPOLY-2);

		} while( offs < size );
	}
	else
	{
		plot.fillParametersApply(m_fill);
		plot.plotPoly(
			m_x, 
			m_y, 
			m_cnt
		);
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererCartesian2d::renderPoints(const EsChPlot& plot) const
{
	if( m_point->isNone() )
		return;

	plot.pointParametersApply(m_point);
	plot.plotPoints(
		m_x, 
		m_y, 
		1, 
		m_cnt-2
	);
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererCartesian2d::renderLine(const EsChPlot& plot) const
{
	if( m_line->isNone() )
		return;

	plot.lineParametersApply(m_line);
	plot.plotLine(m_x, m_y, 1, m_cnt-2);
}
//--------------------------------------------------------------------------------

bool EsChPlotSeriesRendererCartesian2d::isVisible(const EsChPlot& plot) const
{
	return EsChPlotSeriesRenderer::isVisible(plot) &&
		((!m_line->isNone() && 1 < m_series->sizeGet()) || 
		(!m_fill->isNone() && 1 < m_series->sizeGet()) || 
		!m_point->isNone());
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererCartesian2d::checkCacheOverflow() const
{
	size_t curSize = m_x.get_count();
	if( m_cnt >= curSize-1 )
	{
		m_x.set_count( curSize+cacheChunkSize );
		m_y.set_count( curSize+cacheChunkSize );
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererCartesian2d::doOptimize(const EsChPlot& plot) const
{
	m_cnt = 0;

	double lodX = plot.physicalDeltaToWorldDelta(0, 1);
	double lodY = plot.physicalDeltaToWorldDelta(1, 1);

	double prevX = m_series->dataGetAt(0, 0);
	double prevY = m_series->dataGetAt(1, 0);

	// create extra point for polygonal stuff
	m_x.itemSet(
		m_cnt,
		prevX
	);
	
	m_y.itemSet(
		m_cnt++,
		0.
	);

	m_x.itemSet(
		m_cnt,
		prevX
	);
	
	m_y.itemSet(
		m_cnt++,
		prevY
	);

	size_t lastIdx = m_series->sizeGet()-1;
	for(size_t idx = 1; idx < lastIdx; ++idx)
	{
		double curX = m_series->dataGetAt(0, idx);
		double curY = m_series->dataGetAt(1, idx);
		
		if( 
			!EsChPlotGeometry::_2D::Cartesian::pointsInVicinity(
				prevX, 
				prevY, 
				curX, 
				curY, 
				lodX, 
				lodY
			) 
		)
		{
			checkCacheOverflow();

			m_x.itemSet(
				m_cnt,
				curX
			);
			
			m_y.itemSet(
				m_cnt++,
				curY
			);

			prevX = curX;
			prevY = curY;
		}
	}

	checkCacheOverflow();
	m_x.itemSet(
		m_cnt,
		m_series->dataGetAt(
			0, 
			lastIdx
		)
	);
	
	m_y.itemSet(
		m_cnt++,
		m_series->dataGetAt(
			1, 
			lastIdx
		)
	);

	// create extra point for polygonal stuff
	checkCacheOverflow();
	m_x.itemSet(
		m_cnt,
		m_x.itemGet(m_cnt-1)
	);
	
	m_y.itemSet(
		m_cnt++,
		0.
	);
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererCartesian2d::doRender(const EsChPlot& plot) const
{
	renderPoly(plot);
	renderLine(plot);
	renderPoints(plot);
}
//--------------------------------------------------------------------------------

bool EsChPlotSeriesRendererCartesian2d::hitInfoGet(const EsChPlot& plot, double wx, double wy, double toleranceX, double toleranceY, EsChPlotObjectHitInfo& info) const
{
	bool isHit = false;
	EsChPlotObjectHitInfo::Code code;
	EsVariant optional;

	size_t idx;
	if( 
		hitPoint(
			plot, 
			wx, 
			wy, 
			toleranceX, 
			toleranceY, 
			idx
		) 
	)
	{
		code = EsChPlotObjectHitInfo::POINT;

		EsVariant::Array tmp = {
			EsVariant(
				reinterpret_cast<void*>(m_series.get()),
				EsVariant::ACCEPT_POINTER
			),
			static_cast<ulong>(idx)
		};

		optional = tmp;
		isHit = true;
	}
	else if( 
		hitLine(
			plot, 
			wx, 
			wy, 
			toleranceX, 
			toleranceY
		) 
	)
	{
		code = EsChPlotObjectHitInfo::BORDER;
		optional = EsVariant(
			reinterpret_cast<void*>(m_series.get()),
			EsVariant::ACCEPT_POINTER
		);
		isHit = true;
	}
	else if( 
		hitPoly(
			wx, 
			wy, 
			toleranceX, 
			toleranceY
		) 
	)
	{
		code = EsChPlotObjectHitInfo::BODY;
		optional = EsVariant(
			reinterpret_cast<void*>(m_series.get()),
			EsVariant::ACCEPT_POINTER
		);
		isHit = true;
	}

	if( isHit )
	{
		EsChPlotObjectHitInfo result(
			code, 
			this, 
			optional
		);
		
		info = result;
	}

	return isHit;
}
//--------------------------------------------------------------------------------

bool EsChPlotSeriesRendererCartesian2d::hitPoint(const EsChPlot& plot, double wx, double wy, double toleranceX, double toleranceY, size_t& idx) const
{
	if( !m_point->isNone() )
	{
		double markerX = wxMax(
			plot.absoluteDeltaToWorldDelta(
				0, 
				m_point->sizeGet()
			),
			toleranceX
		);
		
		double markerY = wxMax(
			plot.absoluteDeltaToWorldDelta(
				1, 
				m_point->sizeGet()
			),
			toleranceY
		);
		
		double pointTolerance = EsMath::sqrt(markerX*markerX + markerY*markerY);
		
		for(idx = 0; idx < m_cnt; ++idx)
		{
			if( 
				EsChPlotGeometry::_2D::Cartesian::distanceGet(
					m_x.itemGet(idx), 
					m_y.itemGet(idx),	
					wx, 
					wy
				) <= pointTolerance 
			)
				return true;
		}
	}

	return false;
}
//--------------------------------------------------------------------------------

bool EsChPlotSeriesRendererCartesian2d::hitPoly(double wx, double wy, double ES_UNUSED(toleranceX), double ES_UNUSED(toleranceY)) const
{
	if( !m_fill->isNone() )
		return 0 != plP_pointinpolygon(
			m_cnt, 
			m_x.dataGet(), 
			m_y.dataGet(), 
			wx, 
			wy
		);

	return false;
}
//--------------------------------------------------------------------------------

bool EsChPlotSeriesRendererCartesian2d::hitLine(const EsChPlot& plot, double wx, double wy, double toleranceX, double toleranceY) const
{
	if( !m_line->isNone() )
	{
		double lineX2 = wxMax(
			plot.physicalDeltaToWorldDelta(
				0, 
				m_line->widthGet()
			), 
			toleranceX
		) / 2.;

		double lineY2 = wxMax(
			plot.physicalDeltaToWorldDelta(
				1, 
				m_line->widthGet()
			), 
			toleranceY
		) / 2.;

		double x[5], y[5];

		for(size_t idx = 0; idx < m_cnt; idx += 2)
		{
			double cury = m_y.itemGet(idx);

			x[0] = m_x.itemGet(idx)-lineX2;
			y[0] = cury-lineY2;
			x[1] = x[0];
			y[1] = cury+lineY2;

			cury = m_y.itemGet(idx+1);
			x[2] = m_x.itemGet(idx+1)+lineX2;
			y[2] = cury+lineY2;
			x[3] = x[2];
			y[3] = cury-lineY2;
			x[4] = x[0];
			y[4] = y[0];

			if( 
				0 != plP_pointinpolygon(
					5, 
					x, 
					y, 
					wx, 
					wy
				) 
			)
				return true;
		}
	}

	return false;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChPlotSeriesRendererHistogram2d::EsChPlotSeriesRendererHistogram2d(ulong width, wxColor color, double base) :
m_base(base),
m_cnt(0),
m_x(cacheChunkSize),
m_y(cacheChunkSize)
{
	m_line = EsChPlotLineParameters::create(this);
	ES_ASSERT(m_line);
	m_line->colorSet(color);
	m_line->widthSet(width);
}
//--------------------------------------------------------------------------------

EsChPlotSeriesRendererHistogram2d::Ptr EsChPlotSeriesRendererHistogram2d::create( ulong width,	wxColor color, double base /*= 0.*/)
{
	return ES_MAKE_SHARED(
		EsChPlotSeriesRendererHistogram2d,
		width, 
		color, 
		base
	);
}
//--------------------------------------------------------------------------------

bool EsChPlotSeriesRendererHistogram2d::isVisible(const EsChPlot& plot) const
{
	return EsChPlotSeriesRenderer::isVisible(plot) &&
		!m_line->isNone();
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererHistogram2d::doRender(const EsChPlot& plot) const
{
	plot.lineParametersApply(m_line);

	size_t idx = 0; 
	while(idx < m_cnt)
	{
		plot.plotLine(
			m_x.itemGet(idx), 
			m_y.itemGet(idx),
			m_x.itemGet(idx+1),
			m_y.itemGet(idx+1)
		);
		
		idx += 2;
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererHistogram2d::doOptimize(const EsChPlot& plot) const
{
	m_cnt = 0;

	double lodX = plot.physicalDeltaToWorldDelta(0, 1);
	double xCur = m_series->dataGetAt(0, 0);
	double xBase = xCur;
	double yCur = m_series->dataGetAt(1, 0);
	double yMax = yCur;

	m_x.itemSet(
		m_cnt,
		xBase
	);
	m_y.itemSet(
		m_cnt++,
		m_base
	);
	m_x.itemSet(
		m_cnt,
		xBase
	);
	m_y.itemSet(
		m_cnt++,
		yCur
	);

	size_t n = m_series->sizeGet();
	bool lookupOccured = false;

	for(size_t idx = 1; idx < n; ++idx)
	{
		xCur = m_series->dataGetAt(0, idx);
		yCur = m_series->dataGetAt(1, idx);

		if( abs(xCur-xBase) < abs(lodX) &&
				idx < n-1 )
		{
			if(yMax < yCur)
				yMax = yCur;

			lookupOccured = true;
		}
		else
		{
			if( lookupOccured )
			{
				checkCacheOverflow();

				m_x.itemSet(
					m_cnt,
					xBase
				);
				m_y.itemSet(
					m_cnt++,
					m_base
				);
				m_x.itemSet(
					m_cnt,
					xBase
				);
				m_y.itemSet(
					m_cnt++,
					yMax
				);
				
				lookupOccured = false;
			}

			checkCacheOverflow();
			m_x.itemSet(
				m_cnt,
				xCur
			);
			m_y.itemSet(
				m_cnt++,
				m_base
			);
			m_x.itemSet(
				m_cnt,
				xCur
			);
			m_y.itemSet(
				m_cnt++,
				yCur
			);

			xBase = xCur;
			yMax = yCur;
		}
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeriesRendererHistogram2d::checkCacheOverflow() const
{
	size_t curSize = m_x.get_count();
	if( m_cnt >= curSize-2 )
	{
		m_x.set_count( curSize+cacheChunkSize );
		m_y.set_count( curSize+cacheChunkSize );
	}
}
//--------------------------------------------------------------------------------

// renderer hit test implementation
bool EsChPlotSeriesRendererHistogram2d::hitInfoGet(const EsChPlot& plot, double wx, double wy, double toleranceX, double ES_UNUSED(toleranceY), EsChPlotObjectHitInfo& info) const
{
	if(	!m_line->isNone() )
	{
		double lineX2 = wxMax(
			plot.physicalDeltaToWorldDelta(
				0, 
				m_line->widthGet()
			), 
			toleranceX
		) / 2.;

		for(size_t idx = 0; idx < m_cnt; idx += 2)
		{
			double x = m_x.itemGet(idx);
			if( 
				wx >= x-lineX2 && 
				wx <= x+lineX2 && 
				wy >= m_y.itemGet(idx) &&
				wy <= m_y.itemGet(idx+1) 
			)
			{
				EsChPlotObjectHitInfo result(
					EsChPlotObjectHitInfo::BODY, 
					this, 
					EsVariant(
						m_series.get(),
						EsVariant::ACCEPT_POINTER
					)
				);
				
				info = result;
				return true;
			}
		}
	}

	return false;
}
//--------------------------------------------------------------------------------
