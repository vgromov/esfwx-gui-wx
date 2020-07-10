#include "eschart_pch.h"
#pragma hdrstop

#include "EsChPlotObject.h"

// lock object
EsChPlotSeries::Lock::Lock(const EsChPlotObject::Ptr& obj) :
m_obj(obj)
{
	ES_ASSERT(m_obj);
	m_obj->lock();
}

EsChPlotSeries::Lock::~Lock()
{
	m_obj->unlock();
}

// plot object base implementation
EsChPlotObject::EsChPlotObject(EsChPlotObject* object) :
m_lock(0),
m_pendingFlags(0)
{
	if( object )
		subscriberAdd(object);
}

// notifier interface
void EsChPlotObject::notifyRecalc(const EsChPlotObject& obj) const
{
	if( isLocked() )
		m_pendingFlags |= PENDING_RECALC;
	else
		for(Subscribers::const_iterator cit = m_subscribers.begin(); cit != m_subscribers.end(); ++cit)
			(*cit).first->onRecalc( obj );
}

void EsChPlotObject::notifyRedraw(const EsChPlotObject& obj) const
{
	if( isLocked() )
		m_pendingFlags |= PENDING_REDRAW;
	else
		for(Subscribers::const_iterator cit = m_subscribers.begin(); cit != m_subscribers.end(); ++cit)
			(*cit).first->onRedraw( obj );
}

// notificatoin handlers
void EsChPlotObject::onRecalc(const EsChPlotObject& sender)
{
	if( isLocked() )
		m_pendingFlags |= PENDING_RECALC;
	else // delegate further by default
		notifyRecalc(sender);
}

void EsChPlotObject::onRedraw(const EsChPlotObject& sender) const
{
	if( isLocked() )
		m_pendingFlags |= PENDING_REDRAW;
	else // delegate further by default
		notifyRedraw(sender);
}

// plot linkage. managed from the plot object itself
void EsChPlotObject::subscriberAdd(EsChPlotObject* plot)
{
	ES_ASSERT(plot);
	Subscribers::iterator it = m_subscribers.find(plot);
	if( it != m_subscribers.end() )
		++(*it).second;
	else
		m_subscribers[plot] = 1;
}

void EsChPlotObject::subscriberRemove(EsChPlotObject* obj)
{
	ES_ASSERT(obj);
	Subscribers::iterator it = m_subscribers.find(obj);
	if( it != m_subscribers.end() )
	{
		size_t useCnt = (*it).second;
		ES_ASSERT( 0 < useCnt );
		--useCnt;
		if( 0 == useCnt )
		{
			ES_ASSERT((*it).first);
			m_subscribers.erase(it);
		}
	}
}

// lock handling
void EsChPlotObject::lock()
{
	++m_lock;
}

void EsChPlotObject::unlock()
{
	if( m_lock > 0 )
		--m_lock;

	// perform pending series update operations
	// notify linked plots of series change
	if( 0 == m_lock &&
			0 != m_pendingFlags )
	{
		onUnlock();
		m_pendingFlags = 0;
	}
}

void EsChPlotObject::onUnlock()
{
	if( m_pendingFlags & PENDING_RECALC )
		notifyRecalc();
	if( m_pendingFlags & PENDING_REDRAW )
		notifyRedraw();
}

// hit testing interface. declared public, though
// should not be called directly unless you know what you're
// doing. default method does nothing, returning false
bool EsChPlotObject::hitInfoGet(const EsChPlot& WXUNUSED(plot), 
																double WXUNUSED(wx), double WXUNUSED(wy), 
																double WXUNUSED(toleranceX), double WXUNUSED(toleranceY),
																EsChPlotObjectHitInfo& WXUNUSED(info)) const
{
	return false;
}

// hit info helper class
//
EsChPlotObjectHitInfo::EsChPlotObjectHitInfo(EsChPlotObjectHitInfo::Code code/* = NONE*/, 
																						 const EsChPlotObject* obj/* = 0*/, 
																						 const EsVariant& optional/* = EsVariant::s_null*/) :
m_code(code),
m_obj(obj),
m_optional(optional)
{
}

EsChPlotObjectHitInfo::EsChPlotObjectHitInfo(const EsChPlotObjectHitInfo& other)
{
	copy(other);
}

const EsChPlotObjectHitInfo& EsChPlotObjectHitInfo::operator=(const EsChPlotObjectHitInfo& other)
{
	copy(other);
	return *this;
}

void EsChPlotObjectHitInfo::copy(const EsChPlotObjectHitInfo& other)
{
	m_code = other.m_code;
	m_obj = other.m_obj;
	m_optional = other.m_optional;
}

EsString EsChPlotObjectHitInfo::codeStringGet(size_t code)
{
	switch(code)
	{
	case NONE:
		return wxT("NONE");
	case BODY:
		return wxT("BODY");
	case POINT:
		return wxT("POINT");
	case BORDER:
		return wxT("BORDER");
	}

	return wxT("Unknown code");
}
