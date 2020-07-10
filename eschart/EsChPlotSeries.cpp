#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotSeries.h"
//--------------------------------------------------------------------------------

EsChPlotSeries::Ptr EsChPlotSeries::create1d(size_t defSize/* = 0*/)
{
	return ES_MAKE_SHARED(
		EsChPlotSeries,
		EsChPlotSeriesType_1D, 
		defSize
	);
}
//--------------------------------------------------------------------------------

EsChPlotSeries::Ptr EsChPlotSeries::create2d(size_t defSize/* = 0*/)
{
	return ES_MAKE_SHARED(
		EsChPlotSeries,
		EsChPlotSeriesType_2D, 
		defSize
	);
}
//--------------------------------------------------------------------------------

EsChPlotSeries::Ptr EsChPlotSeries::create3d(size_t defSize/* = 0*/)
{
	return ES_MAKE_SHARED(
		EsChPlotSeries,
		EsChPlotSeriesType_3D, 
		defSize
	);
}
//--------------------------------------------------------------------------------

EsChPlotSeries::EsChPlotSeries(EsChPlotSeriesType type, size_t size) :
m_type(type)
{
	minMaxReset();
	internalSizeSet(size);
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::minMaxReset()
{
	for( ulong idx = 0; idx < dimensionGet(); ++idx )
		m_data[idx].minimaxReset();
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::clear()
{
	size_t dim = dimensionGet();
	// special case: 1D series
	if( EsChPlotSeriesType_1D == m_type )
		--dim;

	for(size_t idx = 0; idx < dim; ++idx)
		m_data[idx].clear();

	minMaxReset();
	notifyRecalc();
	notifyRedraw();
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::nameSet(const EsString& name)
{
	m_name = name;
	notifyRedraw();
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::internalSizeSet(size_t size)
{
	size_t dim = dimensionGet();

	// special case: 1D series
	if( EsChPlotSeriesType_1D == m_type )
		--dim;

	for(size_t idx = 0; idx < dim; ++idx)
		m_data[idx].set_count(size);
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::sizeSet(ulong size)
{
	if( sizeGet() != size )
	{
		internalSizeSet(size);

		minMaxUpdate();
		notifyRedraw();
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::dimensionCheck(ulong dim) const
{
	EsNumericCheck::checkRangeInteger(
		0, 
		dimensionGet()-1, 
		dim, 
		_("Series dimension index")
	);
}
//--------------------------------------------------------------------------------

double EsChPlotSeries::minGet(ulong dim) const
{
	dimensionCheck(dim);
	
	size_t dataDim = dim;
	// special case: 1D series
	if( EsChPlotSeriesType_1D == m_type )
	{
		if(0 == dim )
			return 0.;

		--dataDim;
	}

	if(isEmpty())
		return 0.;
	else
		return m_data[dataDim].get_min();
}
//--------------------------------------------------------------------------------

ulong EsChPlotSeries::minPosGet(ulong dim) const
{
	dimensionCheck(dim);

	// special case: 1D series
	if( EsChPlotSeriesType_1D == m_type &&
			0 == dim )
			return 0;
	
	return m_data[dim].get_minIdx();
}
//--------------------------------------------------------------------------------

double EsChPlotSeries::maxGet(ulong dim) const
{	
	dimensionCheck(dim);
	size_t dataDim = dim;
	if( EsChPlotSeriesType_1D == m_type )
	{
		if(0 == dim )
			return static_cast<double>(sizeGet()-1);

		--dataDim;
	}

	if(isEmpty())
		return 0.;
	else
		return m_data[dataDim].get_max();
}
//--------------------------------------------------------------------------------

ulong EsChPlotSeries::maxPosGet(ulong dim) const
{
	dimensionCheck(dim);	
	return m_data[dim].get_maxIdx();
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::minMaxGet(ulong dim, double& min, double& max) const
{
	dimensionCheck(dim);	

	if( isEmpty() )
		min = max = 0.;
	else
	{
		ulong dataDim = dim;
		if( EsChPlotSeriesType_1D == m_type )
		{
			if(	0 == dim )
			{
				min = 0.;
				max = static_cast<double>(sizeGet()-1);
				return;
			}

			--dataDim;
		}

		min = m_data[dataDim].get_min();
		max = m_data[dataDim].get_max();
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::minMaxPosGet(ulong dim, ulong& minPos, ulong& maxPos) const
{
	dimensionCheck(dim);	

	minPos = m_data[dim].get_minIdx();
	maxPos = m_data[dim].get_maxIdx();
}
//--------------------------------------------------------------------------------

double EsChPlotSeries::totalGet(ulong dim) const
{
	dimensionCheck(dim);

	return m_data[dim].get_total();
}
//--------------------------------------------------------------------------------

EsMathArrayReal& EsChPlotSeries::dataGet(ulong dim)
{
	dimensionCheck(dim);
	if(EsChPlotSeriesType_1D == m_type)
	{
		if(0 < dim)
			--dim;
		else
			EsException::Throw(_("Accessing data as vector at dimention 0 is not supported for 1D series"));
	}

	return m_data[dim];
}
//--------------------------------------------------------------------------------

const EsMathArrayReal& EsChPlotSeries::dataGet(ulong dim) const
{
	return const_cast<EsChPlotSeries*>(this)->dataGet(dim);	
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::dataSet(ulong dim, const EsMathArrayReal& data)
{
	dimensionCheck(dim);
	if( EsChPlotSeriesType_1D == m_type )
	{
		if(1 == dim)
			--dim;
		else
			EsException::Throw(_("Setting data as vector at dimention 0 is not supported for 1D series"));
	}

	internalSizeSet(data.get_count());
	m_data[dim] = data;

	minMaxUpdate();
	notifyRedraw();
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::dataIndexCheck(ulong pos) const
{
	size_t size = sizeGet();
	EsNumericCheck::checkRangeInteger(0, (0 == size) ? 0 : size-1, pos, _("Data point index"));
}
//--------------------------------------------------------------------------------

double EsChPlotSeries::dataGetAt(ulong dim, ulong pos) const
{
	dataIndexCheck(pos);
	dimensionCheck(dim);

	// special case: 1D series
	if( EsChPlotSeriesType_1D == m_type )
	{
		if(	0 == dim )
			return pos;

		--dim;
	}

	return m_data[dim].itemGet(pos);
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::dataSetAt(ulong dim, ulong pos, double val)
{
	dataIndexCheck(pos);
	dimensionCheck(dim);

	// special case: 1D series
	if( EsChPlotSeriesType_1D == m_type )
	{
		if(	0 == dim )
			EsException::Throw(_("Setting data at dimension 0 is not supported for 1D series"));

		--dim;
	}

	if( 
		!EsMath::areEqualFloats( 
			m_data[dim].itemGet(pos), 
			val
		) 
	)
	{
		m_data[dim].itemSet(
			pos,
			val
		);

		minMaxUpdate();
		notifyRedraw();
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::pointSetAt(ulong pos, double _1, double _2, double _3 /*= 0.*/)
{
	dataIndexCheck(pos);
	m_data[0].itemSet(
		pos,
		_1
	);
	
	if( EsChPlotSeriesType_1D < m_type )
		m_data[1].itemSet(
			pos,
			_2
		);
	
	if( EsChPlotSeriesType_3D == m_type )
		m_data[2].itemSet(
			pos,
			_3
		);	

	minMaxUpdate();
	notifyRedraw();
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::minMaxUpdate(ulong dim)
{
	size_t dataDim = dim;
	if( EsChPlotSeriesType_1D == m_type )
	{ 
		if(	0 < dim )
			--dataDim;
		else
		{
			m_data[0].minimaxReset();
			return;
		}
	}

	if( !isEmpty() )
		m_data[dataDim].minimaxRecalc();
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::minMaxUpdate()
{
	if( isLocked() )
		m_pendingFlags |= PENDING_RECALC;
	else
	{
		minMaxReset();

		for(size_t dim = 0; dim < dimensionGet(); ++dim)
			minMaxUpdate(dim);
	}
}
//--------------------------------------------------------------------------------

void EsChPlotSeries::onUnlock()
{
	if( m_pendingFlags & PENDING_RECALC )
		minMaxUpdate();

	// call base class
	EsChPlotObject::onUnlock();
}
//--------------------------------------------------------------------------------
