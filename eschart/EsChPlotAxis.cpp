#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotAxis.h"

const double EsChPlotAxis::defaultMin = 0.;
const double EsChPlotAxis::defaultMax = 1.;

// plot axis parameters
//
EsChPlotAxis::EsChPlotAxis(EsChPlotObject* owner, EsChPlotCoordinateSystem csId, ulong dim, const EsString& title /*= EsString::null()*/) :
EsChPlotObject(owner),
m_csId(csId),
m_dim(dim),
m_flags(EsChPlotAxis_DEFAULT_FLAGS),
m_customOrigin(0),
m_majorTickStep(0),
m_minorTicksPerMajorTick(0),
m_title(title),
m_min(defaultMin),
m_max(defaultMax)
{
	// init some values for angular coordinate systems
	if( EsChPlot_AXIS_POLAR_PHI == m_dim )
	{
		switch(m_csId)
		{
		case EsChPlotCoordinateSystem_POLAR_DEG_2D:
			m_majorTickStep = 15.;
			m_max = 360.;
			break;
		case EsChPlotCoordinateSystem_POLAR_RAD_2D:
			m_majorTickStep = M_PI*0.125;
			m_max = M_PI*2.;
			break;
		}	
	}

	if( m_title.empty() )
	{
		switch(m_csId)
		{
		case EsChPlotCoordinateSystem_CARTESIAN_3D:
			if( EsChPlot_AXIS_CARTESIAN_Z == dim )
				m_title = wxT("Z");
		case EsChPlotCoordinateSystem_CARTESIAN_2D:
			if( EsChPlot_AXIS_CARTESIAN_X == dim )
				m_title = wxT("X");
			else if(EsChPlot_AXIS_CARTESIAN_Y == dim)
				m_title = wxT("Y");
			break;
		case EsChPlotCoordinateSystem_POLAR_DEG_2D:
		case EsChPlotCoordinateSystem_POLAR_RAD_2D:
			if( EsChPlot_AXIS_POLAR_R == dim )
				m_title = wxT("r");
			else if(EsChPlot_AXIS_POLAR_PHI == dim)
				m_title = EsString::format(
					_("phi (%s)"), 
					(EsChPlotCoordinateSystem_POLAR_DEG_2D == m_csId) ? 
						_("Deg") : 
						_("Rad")
				);
			break;
		}
	}
}

void EsChPlotAxis::copyFrom(const EsChPlotAxis::Ptr& other)
{
	ES_ASSERT(other);
	ES_ASSERT(this != other.get());

	m_csId = other->coordinateSystemGet();
	m_dim = other->dimensionGet();
	m_flags = other->flagsGet();
	m_majorTickStep = other->majorTickStepGet();
	m_minorTicksPerMajorTick = other->minorTicksPerMajorTickGet();
	m_title = other->titleGet();
	m_min = other->minGet();
	m_max = other->maxGet();

	notifyRedraw();
}

void EsChPlotAxis::majorTickStepSet(double step)
{
	// check if step > 0
	if( EsMath::areEqualFloats(0., step) || step < 0. )
		EsException::Throw(_("Step value must be greater than 0"));
	m_majorTickStep = step;
	notifyRedraw();
}

void EsChPlotAxis::minorTicksPerMajorTickSet(ulong cnt)
{
	m_minorTicksPerMajorTick = cnt;
	notifyRedraw();
}

void EsChPlotAxis::internalMinMaxSet(double min, double max)
{
	switch(m_csId)
	{
	case EsChPlotCoordinateSystem_POLAR_DEG_2D:
	case EsChPlotCoordinateSystem_POLAR_RAD_2D:
		EsException::Throw(_("Setting minimum && maximum is not supported for polar plot axes"));
		break;
	default:
		// check min < max
		if( EsMath::areEqualFloats(min, max) || min > max )
			EsException::Throw(_("Axis minimum should be less than maximum"));
		m_min = min;
		m_max = max;
		break;
	}
}

void EsChPlotAxis::minMaxSet(double min, double max)
{
	if( !EsMath::areEqualFloats(m_min, min) ||
			!EsMath::areEqualFloats(m_max, max) )
	{
		internalMinMaxSet(min, max);
		m_flags &= ~(EsChPlotAxis_AUTO_MIN|EsChPlotAxis_AUTO_MAX);
		notifyRedraw();
	}
}

void EsChPlotAxis::minSet(double min)
{
	if( !EsMath::areEqualFloats(m_min, min) )
	{
		internalMinSet(min);
		m_flags &= ~EsChPlotAxis_AUTO_MIN;
		notifyRedraw();
	}
}

void EsChPlotAxis::internalMinSet(double min)
{
	switch(m_csId)
	{
	case EsChPlotCoordinateSystem_POLAR_DEG_2D:
	case EsChPlotCoordinateSystem_POLAR_RAD_2D:
		EsException::Throw(_("Setting minimum is not supported for polar plot axes"));
		break;
	default:
		// check min < max
		EsNumericCheck::checkRangeFloat(-DBL_MAX, m_max, min, _("Axis minimum"));
		m_min = min;
		break;
	}
}

void EsChPlotAxis::maxSet(double max)
{
	if( !EsMath::areEqualFloats(m_max, max) )
	{
		internalMaxSet(max);
		m_flags &= ~EsChPlotAxis_AUTO_MAX;
		notifyRedraw();
	}
}

void EsChPlotAxis::internalMaxSet(double max)
{
	switch(m_csId)
	{
	case EsChPlotCoordinateSystem_POLAR_DEG_2D:
	case EsChPlotCoordinateSystem_POLAR_RAD_2D:
		if( EsChPlot_AXIS_POLAR_PHI == m_dim )
			EsException::Throw(_("Setting maximum is not supported for polar plot angle"));
	default:
		// check max > min
		EsNumericCheck::checkRangeFloat(m_min, DBL_MAX, max, _("Axis maximum"));
		m_max = max;
		break;
	}
}

void EsChPlotAxis::autoMinSet(bool autoMin)
{
	if( autoMin != isAutoMin() )
	{
		if( autoMin )
			m_flags |= EsChPlotAxis_AUTO_MIN;
		else
			m_flags &= ~EsChPlotAxis_AUTO_MIN;

		notifyRecalc();
		notifyRedraw();
	}
}

void EsChPlotAxis::autoMaxSet(bool autoMax)
{
	if( autoMax != isAutoMax() )
	{
		if( autoMax )
			m_flags |= EsChPlotAxis_AUTO_MAX;
		else
			m_flags &= ~EsChPlotAxis_AUTO_MAX;

		notifyRecalc();
		notifyRedraw();
	}
}

void EsChPlotAxis::autoMinMaxSet(bool autoMin, bool autoMax)
{
	if( autoMin != isAutoMin() ||
			autoMax != isAutoMax() )
	{
		if( autoMin )
			m_flags |= EsChPlotAxis_AUTO_MIN;
		else
			m_flags &= ~EsChPlotAxis_AUTO_MIN;

		if( autoMax )
			m_flags |= EsChPlotAxis_AUTO_MAX;
		else
			m_flags &= ~EsChPlotAxis_AUTO_MAX;

		notifyRecalc();
		notifyRedraw();
	}
}
	
std::string EsChPlotAxis::optsFromFlags(ulong flags)
{
	std::string result;
	
	if( flags & EsChPlotAxis_AXIS_VISIBLE )
		result += 'a';
	if( flags & EsChPlotAxis_FRAME_VISIBLE )
		result += "bc";
	if( flags & EsChPlotAxis_LABELS_FMT_DATETIME )
		result += 'd';
	if( flags & EsChPlotAxis_LABELS_FMT_FIXEDPOINT )
		result += 'f';
	if( flags & EsChPlotAxis_GRID_AT_MAJOR_TICKS )
		result += 'g';
	if( flags & EsChPlotAxis_GRID_AT_MINOR_TICKS )
		result += 'h';
	if( flags & EsChPlotAxis_TICK_MARKS_OUTSIDE )
		result += 'i';
	if( flags & EsChPlotAxis_LOGARITHMIC )
		result += 'l';
	if( flags & EsChPlotAxis_LABELS_VISIBLE )
	{
		result += 'n';
		if( flags & EsChPlotAxis_LABELS_AT_OPPOSITE_LOCATION )
		result += 'm';
	}
	if( flags & EsChPlotAxis_TICKS_VISIBLE )
	{
		result += 't';
		if( flags & EsChPlotAxis_TICKS_MINOR_VISIBLE )
			result += 's';
	}
	if( flags & EsChPlotAxis_LABELS_FMT_CUSTOM )
		result += 'o';

	return result;
}
