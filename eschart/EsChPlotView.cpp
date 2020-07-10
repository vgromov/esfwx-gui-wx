#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotView.h"

// view parameters
//
EsChPlotView::EsChPlotView( EsChPlotObject* owner, 
	EsChPlotCoordinateSystem cs, const EsString& title) :
EsChPlotObject(owner),
m_cs(cs),
m_title(title),
m_hmin(0.),
m_hmax(1.),
m_vmin(0.),
m_vmax(1.),
m_majorTicksLen(2.),
m_minorTicksLen(1.)
{
	m_titlesParams = EsChPlotTextParameters::create(this);
	ES_ASSERT(m_titlesParams);
	m_labelsParams = EsChPlotTextParameters::create(this);
	ES_ASSERT(m_labelsParams);

	m_axesLineParams = EsChPlotLineParameters::create(this);
	ES_ASSERT(m_axesLineParams);

	m_axesGridLineParams = EsChPlotLineParameters::create(this);
	ES_ASSERT(m_axesGridLineParams);
	m_axesGridLineParams->styleSet(EsChPlotLineStyle_SHORT_DASH_SHORT_GAP);
	m_axesGridLineParams->colorSet(*wxLIGHT_GREY);

	m_axes[0] = EsChPlotAxis::Ptr( new EsChPlotAxis(this, cs, 0) );
	ES_ASSERT(m_axes[0]);

	m_axes[1] = EsChPlotAxis::Ptr( new EsChPlotAxis(this, cs, 1) );
	ES_ASSERT(m_axes[1]);

	if( m_cs > EsChPlotCoordinateSystem_POLAR_RAD_2D )
	{
		m_axes[2] = EsChPlotAxis::Ptr( new EsChPlotAxis(this, cs, 2) );
		ES_ASSERT(m_axes[2]);
	}
}

EsChPlotView::~EsChPlotView()
{
	for(size_t idx = 0; idx < m_series.size(); ++idx)
	{
		m_series[idx]->subscriberRemove(this);
		m_renderers[idx]->subscriberRemove(this);
	}

	for(size_t idx = 0; idx < m_decorations.size(); ++idx)
		m_decorations[idx]->subscriberRemove(this);

	for( size_t idx = 0; idx < dimensionGet(); ++idx )
		if( m_axes[idx] )
			m_axes[idx]->subscriberRemove(this);
}

EsChPlotAxis::Ptr EsChPlotView::axisGet(size_t dim) const
{
	ES_ASSERT(dim < dimensionGet());
	return m_axes[dim];
}

// view window (zooming)
void EsChPlotView::windowHget(double& min, double& max) const
{
	min = m_hmin;
	max = m_hmax;
}

void EsChPlotView::internalWindowHset(double min, double max)
{
	if( EsMath::areEqualFloats(min, max) || min > max )
		EsException::Throw(_("Plot view window horizontal minimum should be less than maximum"));

	EsNumericCheck::checkRangeFloat(0., 1., min, _("Plot view window horizontal minimum"));
	EsNumericCheck::checkRangeFloat(0., 1., max, _("Plot view window horizontal maximum"));

	m_hmin = min;
	m_hmax = max;
}

void EsChPlotView::windowHset(double min, double max)
{
	if( !EsMath::areEqualFloats(m_hmin, min) ||
			!EsMath::areEqualFloats(m_hmax, max) )
	{
		internalWindowHset(min, max);
		notifyRedraw();
	}
}

void EsChPlotView::windowVget(double& min, double& max) const
{
	min = m_vmin;
	max = m_vmax;
}

void EsChPlotView::internalWindowVset(double min, double max)
{
	if( EsMath::areEqualFloats(min, max) || min > max )
		EsException::Throw(_("Plot view window vertical minimum should be less than maximum"));

	EsNumericCheck::checkRangeFloat(0., 1., min, _("Plot view window vertical minimum"));
	EsNumericCheck::checkRangeFloat(0., 1., max, _("Plot view window vertical maximum"));

	m_vmin = min;
	m_vmax = max;
}

void EsChPlotView::windowVset(double min, double max)
{
	if( !EsMath::areEqualFloats(m_vmin, min) ||
			!EsMath::areEqualFloats(m_vmax, max) )
	{
		internalWindowVset(min, max);
		notifyRedraw();
	}
}

void EsChPlotView::windowGet(double& hmin, double& hmax, double& vmin, double& vmax) const
{
	windowHget(hmin, hmax);
	windowVget(vmin, vmax);
}

void EsChPlotView::windowSet(double hmin, double hmax, double vmin, double vmax)
{
	if( !EsMath::areEqualFloats(m_hmin, hmin) ||
			!EsMath::areEqualFloats(m_hmax, hmax) ||
			!EsMath::areEqualFloats(m_vmin, vmin) ||
			!EsMath::areEqualFloats(m_vmax, vmax) )
	{
		internalWindowHset(hmin, hmax);
		internalWindowVset(vmin, vmax);
		notifyRedraw();
	}
}

void EsChPlotView::windowReset()
{
	if( !EsMath::areEqualFloats(m_hmin, 0.) ||
			!EsMath::areEqualFloats(m_hmax, 1.) ||
			!EsMath::areEqualFloats(m_vmin, 0.) ||
			!EsMath::areEqualFloats(m_vmax, 1.)	)
	{
		m_hmin = 0.;
		m_hmax = 1.;
		m_vmin = 0.;
		m_vmax = 1.;

		notifyRedraw();
	}
}

// series manipulation
//
// dimension check
void EsChPlotView::dimensionCheck(size_t dim) const
{
	EsNumericCheck::checkRangeInteger(0, dimensionGet()-1, dim,
		_("Dimension index"));
}

// get world range for the data linked to this view,
// if no data linked, default 0-1 range is used
double EsChPlotView::dataMinGet(size_t dim) const
{
	dimensionCheck(dim);
	if( m_series.empty() )
		return 0.;
	else
	{
		size_t idx = 0;
		bool foundMin = false;
		double min = 0.;
		for( size_t idx = 0; idx < m_series.size(); ++idx )
		{
			EsChPlotSeries::Ptr series = m_series[idx];
			ES_ASSERT(series);
			if( !series->isEmpty() )
			{
				double tmp = series->minGet(dim);
				if( !foundMin )
				{
					min = tmp;
					foundMin = true;
				}
				else if(min > tmp)
					min = tmp;
			}
		}

		return min;
	}
}

double EsChPlotView::dataMaxGet(size_t dim) const
{
	dimensionCheck(dim);
	if( m_series.empty() )
		return 1.;
	else
	{
		size_t idx = 0;
		bool foundMax = false;
		double max = 1.;
		for( size_t idx = 0; idx < m_series.size(); ++idx )
		{
			EsChPlotSeries::Ptr series = m_series[idx];
			ES_ASSERT(series);
			if( !series->isEmpty() )
			{
				double tmp = series->maxGet(dim);
				if( !foundMax )
				{
					max = tmp;
					foundMax = true;
				}
				else if(max < tmp)
					max = tmp;
			}
		}

		return max;
	}
}

void EsChPlotView::dataMinMaxGet(size_t dim, double& min, double& max) const
{
	dimensionCheck(dim);
	if( m_series.empty() )
	{
		min = 0.;
		max = 1.;
	}
	else
	{
		size_t idx = 0;
		bool foundMinMax = false;
		min = 0.;
		max = 1.;
		for( size_t idx = 0; idx < m_series.size(); ++idx )
		{
			EsChPlotSeries::Ptr series = m_series[idx];
			ES_ASSERT(series);
			if( !series->isEmpty() )
			{
				double tmpMin = series->minGet(dim);
				double tmpMax = series->maxGet(dim);
				if( !foundMinMax )
				{
					min = tmpMin;
					max = tmpMax;
					foundMinMax = true;
				}
				else
				{
					if(min > tmpMin)
						min = tmpMin;
					if(max < tmpMax)
						max = tmpMax;
				}
			}
		}
	}
}

// view axes parameters update
void EsChPlotView::axesUpdateRange()
{
	for( size_t dim = 0; dim < dimensionGet(); ++dim )
	{
		EsChPlotAxis::Ptr ap = axisGet(dim);
		double min, max;
		dataMinMaxGet(dim, min, max);

		if( ap->isAutoMin() &&
				ap->isAutoMax()	)
		{
			if( EsMath::areEqualFloats(min, max) )
				max = min + 1.;

			ap->internalMinMaxSet(min, max);
		}
		else
		{
			if( ap->isAutoMin() )
				ap->internalMinSet(min);

			if( ap->isAutoMax() )
				ap->internalMaxSet(max);
		}
	}
}

#define ESCH_NO_IDX static_cast<size_t>(-1)
size_t EsChPlotView::internalSeriesFind(const EsChPlotSeries::Ptr& series, bool doThrow /*= true*/) const
{
	size_t result = ESCH_NO_IDX;
	EsChPlotSeries::Array::const_iterator cit = std::find(m_series.begin(), m_series.end(), series);
	if( cit != m_series.end() )
		result = cit - m_series.begin();
	else if( doThrow )
		EsException::Throw(
			_("Series '%s' is not linked to the plot's view '%s'"), 
			series->nameGet(), 
			m_title
		);

	return result;
}

void EsChPlotView::seriesAdd(const EsChPlotSeries::Ptr& series, const EsChPlotSeriesRenderer::Ptr& renderer)
{
	ES_ASSERT(series);
	ES_ASSERT(renderer);
	
	size_t idx = internalSeriesFind(series, false);
	if( idx != ESCH_NO_IDX )
		EsException::Throw(
			_("Series '%s' is already linked to the plot's view '%s'"), 
			series->nameGet(), 
			m_title
		);
	else
	{
		m_series.push_back(series);
		m_renderers.push_back(renderer);
		// link series and renderer view
		renderer->subscriberAdd(this);
		series->subscriberAdd(this);
		renderer->linkSeries( series );

		onRecalc(*this);
		notifyRedraw();
	}
}

void EsChPlotView::seriesRemove(const EsChPlotSeries::Ptr& series)
{
	ES_ASSERT(series);

	size_t idx = internalSeriesFind(series, false);
	if( idx != ESCH_NO_IDX )
	{
		const EsChPlotSeriesRenderer::Ptr& renderer = *(m_renderers.begin()+idx);
		ES_ASSERT(renderer);
		renderer->unlinkSeries();
		renderer->subscriberRemove(this);
		m_renderers.erase(m_renderers.begin()+idx);

		series->subscriberRemove(this);
		m_series.erase(m_series.begin()+idx);

		onRecalc(*this);
		notifyRedraw();
	}
}

void EsChPlotView::seriesRendererSet(const EsChPlotSeries::Ptr& series, const EsChPlotSeriesRenderer::Ptr& renderer)
{
	ES_ASSERT(series);
	ES_ASSERT(renderer);

	size_t idx = internalSeriesFind(series);
	EsChPlotSeriesRenderer::Ptr& oldRenderer = m_renderers[idx];
	ES_ASSERT(oldRenderer);

	oldRenderer->unlinkSeries();
	oldRenderer->subscriberRemove(this);
	oldRenderer = renderer;

	renderer->linkSeries(series);
	renderer->subscriberAdd(this);
	
	notifyRedraw();
}

EsChPlotSeriesRenderer::Ptr EsChPlotView::seriesRendererGet(const EsChPlotSeries::Ptr& series) const
{
	ES_ASSERT(series);

	size_t idx = internalSeriesFind(series);
	EsChPlotSeriesRenderer::Ptr renderer = m_renderers[idx];
	ES_ASSERT(renderer);
	ES_ASSERT(renderer->m_series == series);
	
	return renderer;
}

// decorations manipulation
void EsChPlotView::decorationAdd(const EsChPlotDecorationObject::Ptr& decoration)
{
	ES_ASSERT(decoration);
	EsChPlotDecorationObject::Array::iterator it = std::find(m_decorations.begin(), m_decorations.end(), decoration);
	if( it != m_decorations.end() )
		EsException::Throw(
			_("Decoration object is already linked to the plot's view '%s'"), 
			m_title
		);
	else
	{
		m_decorations.push_back(decoration);
		decoration->subscriberAdd(this);

		notifyRedraw();
	}
}

void EsChPlotView::decorationRemove(const EsChPlotDecorationObject::Ptr& decoration)
{
	ES_ASSERT(decoration);

	EsChPlotDecorationObject::Array::iterator it = std::find(m_decorations.begin(), m_decorations.end(), decoration);
	if( it != m_decorations.end() )
	{
		(*it)->subscriberRemove(this);
		m_decorations.erase(it);
		notifyRedraw();
	}
}

// direct invalidation of all owned renderers
void EsChPlotView::invalidateRenderers()
{
	for(size_t idx = 0; idx < m_renderers.size(); ++idx)
		m_renderers[idx]->invalidate();
}

// EsChPlotObject overrides
void EsChPlotView::onRecalc(const EsChPlotObject& sender)
{
	// update axes minimax first
	if( !isLocked() )
		axesUpdateRange();
	// call base class 
	EsChPlotObject::onRecalc(sender);
}

void EsChPlotView::onUnlock()
{
	if(m_pendingFlags & PENDING_RECALC)
		axesUpdateRange();

	// call base class
	EsChPlotObject::onUnlock();
}

bool EsChPlotView::hitInfoGet(const EsChPlot& plot, double wx, double wy, 
															double toleranceX, double toleranceY, 
															EsChPlotObjectHitInfo& info) const
{
	// check decorations first
	for(int idx = static_cast<int>(m_decorations.size())-1; idx >= 0; --idx)
	{
		EsChPlotDecorationObject::Ptr decoration = m_decorations[idx];
		ES_ASSERT(decoration);
		if( decoration->hitInfoGet(plot, wx, wy, toleranceX, toleranceY, info) )
			return true;
	}

	// check series+series renderers then
	for(int idx = static_cast<int>(m_series.size()-1); idx >= 0; --idx)
	{
		EsChPlotSeries::Ptr series = m_series[idx];
		ES_ASSERT(series);
		EsChPlotSeriesRenderer::Ptr renderer = seriesRendererGet(series);
		ES_ASSERT(renderer);
		if( renderer->hitInfoGet(plot, wx, wy, toleranceX, toleranceY, info) )
			return true;
	}

	// neither series nor decorations were hit, so report 
	// view body was hit (axis && titles hit test is not implemented so far)
	EsChPlotObjectHitInfo result(EsChPlotObjectHitInfo::BODY, this);
	info = result;

	return true;
}