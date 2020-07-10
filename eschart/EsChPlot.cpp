#include "eschart_pch.h"
#pragma hdrstop

#include <wx/stdpaths.h>

#include <plplotP.h>
#include <plstream.h>
#include "EsChPlotStreamAdapter.h"
#include "EsChPlot.h"
//--------------------------------------------------------------------------------

static_assert(sizeof(double) == sizeof(PLFLT), "PLplot PLFLT is size of double");
static_assert(sizeof(wxInt32) == sizeof(PLINT), "PLplot PLINT is size of wxInt32");
//--------------------------------------------------------------------------------

// internal plplot global variable for auxiliary files search
extern "C" char* plplotLibDir;
//--------------------------------------------------------------------------------

EsChPlot::EsChPlot(wxWindow* container, const wxSize& size, int flags, ulong rows, ulong cols, const wxColor& bgColor) :
	m_container(container),
	m_plstream(nullptr),
	m_style(flags),
	m_needRefresh(true),
	m_rows(rows),
	m_cols(cols),
	m_activeView(0),
	m_lineIsNone(false),
	m_fillIsNone(false),
	m_pointIsNone(false),
	m_pointStyle(0)
{
	m_plstream = new EsChPlotStreamAdapter(*this);
	ES_ASSERT(m_plstream);

	m_plstream->sexit(EsChPlot::exitHandler);
	m_plstream->sdev("wxwidgets");

	// Adjust size, if it has invalid values
	static const wxSize sc_sizeDefault(640, 480);
	wxSize sizeInit = size;
	if(
		!sizeInit.IsFullySpecified() ||
		sizeInit.GetWidth() > 32768 ||
		sizeInit.GetHeight() > 32768
	)
		sizeInit = sc_sizeDefault;

	m_plstream->spage( 
		0., 
		0., 
		sizeInit.x,
		sizeInit.y,
		0, 
		0 
	);
	
	backgroundColorSet(bgColor);
	
	m_views.resize( m_cols*m_rows );
	m_activeView = 0xFFFFFFFF;
	
	// use freetype, antialized canvas?
	EsString drvopt;
	drvopt += EsString::format(
		esT("hrshsym=%d,text=%d"),
		(m_style & EsChPlotModeFlag_DRAW_TEXT) ? 0 : 1,
		(m_style & EsChPlotModeFlag_DRAW_TEXT) ? 1 : 0
	);

	EsByteString asciiTmp = EsString::toAscii(drvopt);
	m_plstream->setopt(
		"-drvopt", 
		asciiTmp.c_str() 
	);
	
	// Set-up DC for device before calling ->star
	// 
	m_image.Destroy();
	m_canvasDc.SelectObject(wxNullBitmap);
	m_canvas = wxNullBitmap;

	m_canvas.Create(
		sizeInit,
		32
	);
	ES_ASSERT(m_canvas.IsOk());

	m_canvasDc.SelectObject(m_canvas);
	ES_ASSERT(m_canvasDc.IsOk());

	m_plstream->sdevdata(
		reinterpret_cast<void*>(&m_canvasDc)
	);

	// temporarily set-up internal global var for aux font file loading during plstream->star call
	asciiTmp = EsString::toUtf8(
		EsPath::stdAppPathGet()
	);

	plplotLibDir = const_cast<char*>(asciiTmp.c_str());
	m_plstream->star(
		m_cols, 
		m_rows
	);
	plplotLibDir = nullptr;
}
//--------------------------------------------------------------------------------

EsChPlot::~EsChPlot()
{
	wxDELETE( m_plstream );
}
//--------------------------------------------------------------------------------

int EsChPlot::exitHandler(const char* msg)
{
	EsException::Throw(	EsString::fromUtf8(msg).c_str() );

	return 0;
}
//--------------------------------------------------------------------------------

bool EsChPlot::isOk() const
{
	return 0 < m_rows &&
		0 < m_cols &&
		0 != m_plstream &&
		(m_image.IsOk() || m_canvas.IsOk());
}
//--------------------------------------------------------------------------------

EsChPlot::Ptr EsChPlot::create(const wxSize& size, int flags, ulong rows, ulong cols, const wxColor& bgColor )
{
	return ES_MAKE_SHARED(
		EsChPlot,
		nullptr, 
		size, 
		flags, 
		rows, 
		cols, 
		bgColor
	);
}
//--------------------------------------------------------------------------------

EsChPlot::Ptr EsChPlot::create(wxWindow* container, int flags, ulong rows, ulong cols, const wxColor& bgColor)
{
	ES_ASSERT(container);
	return ES_MAKE_UNIQUE(
		EsChPlot,
		container, 
		container->GetClientSize(), 
		flags, 
		rows, 
		cols, 
		bgColor
	);
}
//--------------------------------------------------------------------------------

wxSize EsChPlot::sizeGet() const
{
	if( isOk() )
	{
		ES_ASSERT(m_canvas.IsOk());
		return m_canvas.GetSize();
	}

	return wxSize();
}
//--------------------------------------------------------------------------------

wxColor EsChPlot::backgroundColorGet() const
{
	ES_ASSERT(m_plstream);
	PLINT r, g, b;
	PLFLT a;
	m_plstream->gcolbga(
		r, 
		g, 
		b, 
		a
	);

	wxColor result(
		r,
		g,
		b,
		static_cast<uint8_t>( a * static_cast<double>(wxALPHA_OPAQUE) )
	);

	return result;
}
//--------------------------------------------------------------------------------

void EsChPlot::backgroundColorSet(const wxColor& col)
{
	ES_ASSERT(m_plstream);
	m_plstream->scolbga(
		col.Red(),
		col.Green(),
		col.Blue(),
		static_cast<double>(col.Alpha()) / static_cast<double>(wxALPHA_OPAQUE)
	);
}
//--------------------------------------------------------------------------------

void EsChPlot::sizeSet(const wxSize& size)
{
	ES_ASSERT(isOk());
	/* For the AGG backend it is important to set first the new image buffer
	*       and tell the driver the new size if the buffer size increases and
	*       the other way round if the buffer size decreases. There is no impact
	*       for the other backends. This is kind of hacky, but I have no better
	*       idea in the moment */
	wxSize oldSize = sizeGet();
	if( oldSize != size )
	{
		m_canvasDc.SelectObject(wxNullBitmap);
		m_canvas = wxNullBitmap;
		m_canvas.Create(size, 32);
		ES_ASSERT(m_canvas.IsOk());
		
		m_canvasDc.SelectObject(m_canvas);
		ES_ASSERT(m_canvasDc.IsOk());

		m_plstream->cmd( 
			PLESC_RESIZE, 
			(void*)&size 
		);	

		m_needRefresh = true;
		for(size_t idx = 0; idx < m_views.size(); ++idx)
		{
			EsChPlotView::Ptr view = m_views[idx];
			if(view)
				view->invalidateRenderers();
		}
	}
}
//--------------------------------------------------------------------------------

void EsChPlot::refresh() const
{
	ES_DEBUG_TRACE(esT("EsChPlot::refresh"));
	
	if( isOk() && m_needRefresh )
	{
		EsChPlot::Session plottingSession(*this);
		for(ulong idx = 0; idx < viewsCountGet(); ++idx)
		{
			internalSelectView(idx);
			EsChPlotView::Ptr view = m_views[idx];
			
			if( view )
			{
				plotAxesAndTitles();

				// call data rendering for the current view
				const EsChPlotView::Ptr& view = m_views[m_activeView];
				const EsChPlotSeries::Array& series = view->seriesGet();
				const EsChPlotSeriesRenderer::Array& renderers = view->renderersGet();
				ES_ASSERT(series.size() == renderers.size());

				for( size_t idx = 0; idx < series.size(); ++idx )
					renderers[idx]->render(*this);

				const EsChPlotDecorationObject::Array& decorations = view->decorationsGet();
				for( size_t idx = 0; idx < decorations.size(); ++idx )
					decorations[idx]->render(*this);
			}
		}
		
		//wxLogDebug(wxT(">> full cycle"));
	}
}
//--------------------------------------------------------------------------------

void EsChPlot::draw(wxDC& dc, const wxPoint& pnt, const wxSize& ext) const
{
	refresh();

	if( isOk() && dc.IsOk() )
	{
		wxSize size = sizeGet();
		ES_ASSERT(m_canvasDc.IsOk());
		if( ext != size )
			dc.StretchBlit(pnt, ext, &m_canvasDc, wxPoint(), size);
		else
			dc.Blit(pnt, ext, &m_canvasDc, wxPoint());
	}
}
//--------------------------------------------------------------------------------

EsChPlot::Session::Session(const EsChPlot& plot) :
m_plot(plot)
{
	ES_ASSERT(m_plot.isOk());
	m_plot.m_plstream->bop();
}

EsChPlot::Session::~Session()
{
//	m_plot.m_plstream->eop();
	m_plot.m_needRefresh = false;
}

void EsChPlot::internalSelectView(ulong idx) const
{
	if( m_activeView != idx )
	{
		m_activeView = idx;
		m_plstream->adv(m_activeView+1);
	}
}

// checks 
void EsChPlot::viewIndexCheck(ulong idx) const
{
	EsNumericCheck::checkRangeInteger(
		0, 
		m_views.size()-1, 
		idx, 
		_("View index")
	);
}

EsChPlotView::Ptr EsChPlot::viewGet(ulong idx) const
{
	ES_ASSERT(isOk());
	viewIndexCheck(idx);

	return m_views[idx];
}

void EsChPlot::internalViewClear(EsChPlotView::Ptr& view)
{
	if( view )
	{
		view->subscriberRemove(this);
		view = EsChPlotView::Ptr();
	}
}

EsChPlotView::Ptr EsChPlot::viewSet(ulong idx, EsChPlotCoordinateSystem cs, const EsString& title /*= EsString::null()*/)
{
	ES_ASSERT(isOk());
	viewIndexCheck(idx);
	
	EsChPlotView::Ptr& view = m_views[idx];
	internalViewClear(view);
	view = EsChPlotView::Ptr( new EsChPlotView(this, cs, title) );

	return view;
}

void EsChPlot::viewClear(ulong idx)
{
	ES_ASSERT(isOk());
	viewIndexCheck(idx);
	internalSelectView(idx);
	m_plstream->clear();
	internalViewClear(m_views[idx]);
}

void EsChPlot::viewSelect(ulong idx)
{
	ES_ASSERT(isOk());
	viewIndexCheck(idx);
	internalSelectView(idx);
}

EsChPlotView::Ptr EsChPlot::selectedViewGet() const
{
	ES_ASSERT(isOk());
	viewIndexCheck(m_activeView);
	return m_views[m_activeView];
}

void EsChPlot::doRefresh() const
{
	m_needRefresh = true;
	refresh();
	if( m_container )
	{
		m_container->Refresh(false);	
		m_container->Update();
	}
}

// EsChPlotObject overrides
void EsChPlot::onRecalc(const EsChPlotObject& sender)
{
	if( isLocked() )
		m_pendingFlags |= PENDING_RECALC;
}

void EsChPlot::onRedraw(const EsChPlotObject& sender) const
{
	if( isLocked() )
		m_pendingFlags |= PENDING_REDRAW;
	else
		doRefresh();
}

void EsChPlot::onUnlock()
{
	if( m_pendingFlags & PENDING_RECALC )
		m_needRefresh = true;

	if( m_pendingFlags & PENDING_REDRAW )
		doRefresh();	
}

// calculate and return view data window coordinates from view parameters
void EsChPlot::calculateViewWindow(const EsChPlotView::Ptr& view, double& wminX, double& wmaxX, double& wminY, double& wmaxY) const
{
	if( 2 == view->dimensionGet() )
	{
		const EsChPlotAxis::Ptr& a0 = view->axisGet(0);
		const EsChPlotAxis::Ptr& a1 = view->axisGet(1);
		switch(view->coordinateSystemGet())
		{
		case EsChPlotCoordinateSystem_POLAR_DEG_2D:
		case EsChPlotCoordinateSystem_POLAR_RAD_2D:
			wminX = -a0->maxGet();
			wmaxX = a0->maxGet();
			wminY = wminX;
			wmaxY = wmaxX;
			break;
		default:
			wminX = a0->minGet();
			wmaxX = a0->maxGet();
			wminY = a1->minGet();
			wmaxY = a1->maxGet();
		}
	}
	else
	{
		// todo:
	}
}

// coord mapping stuff
//
// retrieve world delta which corresponds to the relative viewport delta, 
// for the currently active view
double EsChPlot::relativeDeltaToWorldDelta(ulong dim, double delta) const
{
	EsChPlotView::Ptr view = selectedViewGet();
	ES_ASSERT(view);
	EsChPlotAxis::Ptr axis = view->axisGet(dim);
	ES_ASSERT(axis);
	
	double viewExtw = abs(axis->maxGet()-axis->minGet());
	double xmind, xmaxd, ymind, ymaxd;
	m_plstream->gvpd(xmind, xmaxd, ymind, ymaxd);
	
	double viewExtd = (0 == dim) ? xmaxd-xmind : ymaxd-ymind;
	if( EsMath::areEqualFloats(viewExtd, 0.) )
		viewExtd = 1.;

	return (delta * viewExtw)/viewExtd;
}

// retrieve world delta which corresponds to the physical device delta, 
// for the currently active view
double EsChPlot::physicalDeltaToWorldDelta(ulong dim, wxCoord delta) const
{
	double d = (0 == dim) ? /*abs(*/plP_pcdcx(delta)-plP_pcdcx(0)/*)*/ :
		/*abs(*/plP_pcdcy(delta)-plP_pcdcy(0)/*)*/;

	return relativeDeltaToWorldDelta(dim, d);
}

// retrieve absolute view coordinates delta in normalized relative view coordinates
double EsChPlot::absoluteDeltaToWorldDelta(ulong dim, double delta) const
{
	double d = (0 == dim) ? /*abs(*/plP_mmdcx(delta)-plP_mmdcx(0)/*)*/ :
		/*abs(*/plP_mmdcy(delta)-plP_mmdcy(0)/*)*/;

	return relativeDeltaToWorldDelta(dim, d);
}

// get currently active view area aspect ratios
double EsChPlot::widthToHeightRatioGet() const
{
	ES_ASSERT(isOk());
	double xmin, xmax, ymin, ymax;
	m_plstream->gspa(xmin, xmax, ymin, ymax);

	ES_ASSERT(!EsMath::areEqualFloats(0., abs(ymax-ymin)));
	return abs(xmax-xmin)/abs(ymax-ymin);
}

double EsChPlot::heightToWidthRatioGet() const
{
	ES_ASSERT(isOk());
	double xmin, xmax, ymin, ymax;
	m_plstream->gspa(xmin, xmax, ymin, ymax);

	ES_ASSERT(!EsMath::areEqualFloats(0., abs(xmax-xmin)));
	return abs(ymax-ymin)/abs(xmax-xmin);
}
//--------------------------------------------------------------------------------

double EsChPlot::scaleOffset(ulong dim, double offs) const
{
	ES_ASSERT(isOk());
	double ratio = 0.;
	double extraDivisor = 10.;
	// take number of views into account
	offs *= viewsCountGet();

	switch( dim )
	{
	case 0:
		ratio = heightToWidthRatioGet();
		offs /= static_cast<double>(m_cols);
		extraDivisor *= static_cast<double>(m_cols);
		break;
	case 1:
		ratio = widthToHeightRatioGet();
		offs /= static_cast<double>(m_rows);
		extraDivisor *= static_cast<double>(m_rows);
		break;
	default:
		wxFAIL;
	}

	if( ratio > 1 )
		return offs * (1 + ratio/extraDivisor);
	else
		return offs * ratio;
}
//--------------------------------------------------------------------------------

double EsChPlot::microOffsStepGet(ulong dim) const
{
	static const double sc_offsStepMicro = 0.02;
	return scaleOffset(dim, sc_offsStepMicro);
}
//--------------------------------------------------------------------------------

double EsChPlot::macroOffsStepGet(ulong dim) const
{
	static const double sc_offsStepMacro = 0.04;
	return scaleOffset(dim, sc_offsStepMacro);
}
//--------------------------------------------------------------------------------

double EsChPlot::absoluteToRelative(ulong dim, double absolute) const
{
	ES_ASSERT(isOk());
	PLFLT result;
	if( 0 == dim )
		result = plP_mmdcx(absolute);
	else
		result = plP_mmdcy(absolute);

	ES_DEBUG_TRACE(
		esT("EsChPlot::absoluteToRelative(dim=%d, absolute=%f)->%f"),
		dim,
		absolute,
		result
	);

	return result;
}
//--------------------------------------------------------------------------------

double EsChPlot::textHeightToRelative(ulong dim, const EsChPlotTextParameters::Ptr& textParams) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(textParams);

	return absoluteToRelative(
		dim, 
		textParams->heightGet()
	);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotAxesAndTitlesCartesian2d(const EsChPlotView::Ptr& view) const
{
	const EsChPlotAxis::Ptr& axisX = view->axisGet(0);
	const EsChPlotAxis::Ptr& axisY = view->axisGet(1);
	ulong flagsX = axisX->flagsGet();
	ulong flagsY = axisY->flagsGet();
	// cache offset steps && set-up initial offsets
	double offsMicroX = microOffsStepGet(0);
	double offsMacroX = macroOffsStepGet(0);
	double offsXleft = offsMicroX;
	double offsXright = offsMicroX;
	
	double offsMicroY = microOffsStepGet(1);
	double offsMacroY = macroOffsStepGet(1);
	double offsYbottom = offsMicroY;
	double offsYtop = offsMicroY;

	double wminX, wmaxX, wminY, wmaxY;
	calculateViewWindow(view, wminX, wmaxX, wminY, wmaxY);

	if( view->isTitleVisible() )
		offsYtop += textHeightToRelative(1, view->titlesParametersGet());

	// if we use y labels, add offset to either side of the viewport, depending on 
	// label font size, label orientation and positioning
	if( axisY->areLabelsVisible() )
	{
		double lblYoffs = textHeightToRelative(0, view->labelsParametersGet());
		if( flagsY & EsChPlotAxis_LABELS_AT_OPPOSITE_LOCATION )
			offsXright += lblYoffs;
		else
			offsXleft += lblYoffs;
		offsYtop += offsMacroY;
		offsYbottom += offsMacroY;
	}

	if( axisY->areTicksVisible() )
	{
		double tiksYoffs = absoluteToRelative(0, view->majorTicksLengthGet());
		if( flagsY & EsChPlotAxis_LABELS_AT_OPPOSITE_LOCATION )
			offsXright += tiksYoffs;
		else
			offsXleft += tiksYoffs;
	}

	if( axisY->isTitleVisible() )
		offsXleft += textHeightToRelative(0, view->titlesParametersGet()) + offsMacroX;

	if( axisX->areLabelsVisible() )
	{
		double lblXoffs = textHeightToRelative(1, view->labelsParametersGet());
		if( flagsX & EsChPlotAxis_LABELS_AT_OPPOSITE_LOCATION )
			offsYtop += lblXoffs;
		else
			offsYbottom += lblXoffs;
		offsXleft += offsMacroX;
		offsXright += offsMacroX;
	}

	if( axisX->areTicksVisible() )
	{
		double tiksXoffs = absoluteToRelative(1, view->majorTicksLengthGet());
		if( flagsX & EsChPlotAxis_LABELS_AT_OPPOSITE_LOCATION )
			offsYtop += tiksXoffs;
		else
			offsYbottom += tiksXoffs;
	}

	if( axisX->isTitleVisible() )
		offsYbottom += textHeightToRelative(1, view->titlesParametersGet());

	m_plstream->vpor(offsXleft, 1.-offsXright, offsYbottom, 1.-offsYtop);
	m_plstream->wind(wminX, wmaxX, wminY, wmaxY);

	// draw everything except gridlines
	ulong flagsXtmp = flagsX & ~EsChPlotAxis_GRID_MASK;
	ulong flagsYtmp = flagsY & ~EsChPlotAxis_GRID_MASK;
	std::string optsX = EsChPlotAxis::optsFromFlags(flagsXtmp);
	std::string optsY = EsChPlotAxis::optsFromFlags(flagsYtmp);
	lineParametersApply(view->axesLineParametersGet());
	textParamtersApply(view->labelsParametersGet());
	m_plstream->box(optsX.c_str(), axisX->majorTickStepGet(), axisX->minorTicksPerMajorTickGet(),
		optsY.c_str(), axisY->majorTickStepGet(), axisY->minorTicksPerMajorTickGet());

	// draw grid separately, to support separate grid line styling
	if( (flagsX & EsChPlotAxis_GRID_MASK) ||
			(flagsX & EsChPlotAxis_GRID_MASK) )
	{
		lineParametersApply(view->axesGridLineParametersGet());
		flagsXtmp = flagsX & EsChPlotAxis_GRID_MASK;
		flagsYtmp = flagsX & EsChPlotAxis_GRID_MASK;
		optsX = EsChPlotAxis::optsFromFlags(flagsXtmp);
		optsY = EsChPlotAxis::optsFromFlags(flagsYtmp);
		m_plstream->box(optsX.c_str(), axisX->majorTickStepGet(), axisX->minorTicksPerMajorTickGet(),
			optsY.c_str(), axisY->majorTickStepGet(), axisY->minorTicksPerMajorTickGet());
	}

	const EsString& titleX = axisX->titleGet();
	const EsString& titleY = axisY->titleGet();
	if(	((EsChPlotAxis_TITLE_VISIBLE & flagsX) && !titleX.empty()) ||
		((EsChPlotAxis_TITLE_VISIBLE & flagsY) && !titleY.empty()) ||
		!view->m_title.empty())
	{
		textParamtersApply(view->titlesParametersGet());
		m_plstream->lab(EsString::toUtf8(titleX).c_str(), EsString::toUtf8(titleY).c_str(), EsString::toUtf8(view->titleGet()).c_str());
	}
}

void EsChPlot::plotPolar2dRadialsDeg(const EsChPlotAxis::Ptr& angularParams,
																		 const EsChPlotLineParameters::Ptr& axisLineParams,
																		 const EsChPlotLineParameters::Ptr& axisGridParams,
																		 double wminX, double wmaxX, double wminY, double wmaxY) const
{
	ulong flagsY = angularParams->flagsGet();
	// plot radial lines as angle "axis"
	if( flagsY & EsChPlotAxis_AXIS_VISIBLE )
	{
		double w = wmaxX;
		if( flagsY & EsChPlotAxis_GRID_AT_MAJOR_TICKS )
		{
			lineParametersApply( axisGridParams );
			double astep = angularParams->majorTickStepGet();
			PLINT subticks = angularParams->minorTicksPerMajorTickGet();
			pldtik( 0., angularParams->maxGet(), &astep, &subticks, 1 );
			double a = astep;
			while(a < angularParams->maxGet())
			{
				if( !EsMath::areEqualFloats(a, 90.) &&
						!EsMath::areEqualFloats(a, 180.) &&
						!EsMath::areEqualFloats(a, 270.) &&
						!EsMath::areEqualFloats(a, 0.) )
				{
					double x = wmaxX;
					// rotate point relative to (0,0)
					double arad = (a*M_PI)/ 180.;
					double y = x*sin(arad);
					x = x*cos(arad);
					plotLine(0., 0., x, y);
				}
				a += astep;				
			}
		}
		
		lineParametersApply( axisLineParams );
		plotLine(wminX, 0., wmaxX, 0.);		
		plotLine(0., wminY, 0., wmaxY);
	}
}

void EsChPlot::plotAxesAndTitlesPolar2d(const EsChPlotView::Ptr& view) const
{
	const EsChPlotAxis::Ptr& axisX = view->axisGet(0);
	const EsChPlotAxis::Ptr& axisY = view->axisGet(1);
	ulong flagsX = axisX->flagsGet();
	ulong flagsY = axisY->flagsGet();

	double offsXleft = microOffsStepGet(0);
	double offsXright = microOffsStepGet(0);
	double offsYbottom = microOffsStepGet(1);
	double offsYtop = microOffsStepGet(1);

	double wminX, wmaxX, wminY, wmaxY;
	calculateViewWindow(view, wminX, wmaxX, wminY, wmaxY);

	if( !view->titleGet().empty() )
	{
		offsYtop += macroOffsStepGet(1);
		offsXleft += macroOffsStepGet(0)/2.;
		offsXright += macroOffsStepGet(0)/2.;
	}

	// if we use y labels, add offset to either side of the viewport, depending on 
	// label font size, label orientation and positioning
	if( flagsY & EsChPlotAxis_LABELS_VISIBLE )
	{
		offsXright += macroOffsStepGet(0);
		offsXleft += macroOffsStepGet(0);
		offsYtop += macroOffsStepGet(1);
		offsYbottom += macroOffsStepGet(1);
	}

	//if((flagsY & EsChPlotAxis_TITLE_VISIBLE) &&	!axisY->titleGet().empty())
	//{
	//	offsXleft += sc_offsStepMacro;
	//}

	//if( flagsX & EsChPlotAxis_LABELS_VISIBLE )
	//{
	//	if( flagsX & EsChPlotAxis_LABELS_AT_OPPOSITE_LOCATION )
	//		offsYtop += sc_offsStepMacro;
	//	else
	//		offsYbottom += sc_offsStepMacro;
	//	offsXleft += sc_offsStepMacro;
	//	offsXright += sc_offsStepMacro;
	//}

	//if( (flagsX & EsChPlotAxis_TITLE_VISIBLE) && !axisX->titleGet().empty())
	//	offsYbottom += sc_offsStepMacro;

	//double width = static_cast<double>(sizeGet().x)/static_cast<double>(m_cols);
	//double height = static_cast<double>(sizeGet().y)/static_cast<double>(m_rows);
	//double aspect = height/width;

	//if( aspect > 1. )
	//{
	//	offsXleft *= aspect;
	//	offsXright *= aspect;
	//}
	//double mmleft, mmright, mmtop, mmbottom;
	//m_plstream->gspa(mmleft, mmright, mmbottom, mmtop);
	//double mmwidth = mmright-mmleft, mmheight = mmtop-mmbottom;
	//double mmext = wxMin(mmwidth, mmheight);
	//m_plstream->svpa((mmwidth-mmext)/2., (mmwidth+mmext)/2., (mmheight-mmext)/2., (mmheight+mmext)/2.);
	m_plstream->vpas(offsXleft, 1.-offsXright, offsYbottom, 1.-offsYtop, 1.);
	m_plstream->wind(wminX, wmaxX, wminY, wmaxY);
	
	// plot concentric circles ( r "axis" )
	PLINT subticks = 0;
	if( flagsX & EsChPlotAxis_AXIS_VISIBLE )
	{
		double r;
		if( flagsX & EsChPlotAxis_GRID_AT_MAJOR_TICKS )
		{
			double rstep = axisX->majorTickStepGet();
			pldtik( 0., wmaxX, &rstep, &subticks, 1 );
			lineParametersApply( view->axesGridLineParametersGet() );
			r = rstep;
			while(r < wmaxX)
			{
				plotArc(r, r);
				r += rstep;
			}
		}
		else
			r = wmaxX;
			
		lineParametersApply( view->axesLineParametersGet() );
		plotArc(r, r);
	}
	
	// draw radials & angular ticks
	if( EsChPlotCoordinateSystem_POLAR_DEG_2D == view->coordinateSystemGet() )
		plotPolar2dRadialsDeg(axisY, view->axesLineParametersGet(),
			view->axesGridLineParametersGet(),	wminX, wmaxX, wminY, wmaxY);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotAxesAndTitles() const
{
	ES_ASSERT(isOk());

	const EsChPlotView::Ptr& view = m_views[m_activeView];

	m_plstream->smaj( view->majorTicksLengthGet(), 1.);
	m_plstream->smin( view->minorTicksLengthGet(), 1.);
	m_plstream->sdiplt(view->windowHminGet(), view->windowVminGet(),
		view->windowHmaxGet(), view->windowVmaxGet());
	
	switch( view->coordinateSystemGet() )
	{
	case EsChPlotCoordinateSystem_CARTESIAN_2D: 
		plotAxesAndTitlesCartesian2d(view);
		break;
	case EsChPlotCoordinateSystem_POLAR_DEG_2D:
	case EsChPlotCoordinateSystem_POLAR_RAD_2D:
		plotAxesAndTitlesPolar2d(view);
		break;
	case EsChPlotCoordinateSystem_CARTESIAN_3D:
		break;
	//{
	//	const EsChPlotAxis& axisX = view->axisGet(0);
	//	const EsChPlotAxis& axisY = view->axisGet(1);
	//	const EsChPlotAxis& paramsZ = view->axisGet(2);
	//	// todo: 3d window parameters
	default:
		wxFAIL;
	}
}
//--------------------------------------------------------------------------------

void EsChPlot::colorSelect(wxColor color) const
{
	ES_ASSERT(isOk());
	m_plstream->scol0a(
		1, 
		color.Red(), 
		color.Green(), 
		color.Blue(), 
		static_cast<double>(color.Alpha())/static_cast<double>(wxALPHA_OPAQUE)
	);
	
	m_plstream->col0(1);
}
//--------------------------------------------------------------------------------

void EsChPlot::textParamtersApply(const EsChPlotTextParameters::Ptr& params) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(params);
	colorSelect(params->colorGet());
	m_plstream->sfont(
		-1, 
		params->italicGet() ? PL_FCI_ITALIC : PL_FCI_UPRIGHT,
		params->boldGet() ? PL_FCI_BOLD : PL_FCI_MEDIUM
	);
	
	m_plstream->schr(
		params->heightGet(), 
		1.
	);
}
//--------------------------------------------------------------------------------

void EsChPlot::lineParametersApply(const EsChPlotLineParameters::Ptr& params) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(params);
	if( !params->isNone() )
	{
		colorSelect(params->colorGet());
		m_plstream->lsty(params->styleGet());
		m_plstream->width(params->widthGet());
		m_lineIsNone = false;
	}
	else
		m_lineIsNone = true;
}
//--------------------------------------------------------------------------------

void EsChPlot::fillParametersApply(const EsChPlotFillParameters::Ptr& params) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(params);
	if( !params->isNone() )
	{
		colorSelect(params->colorGet());
		m_plstream->lsty(EsChPlotLineStyle_SOLID);
		m_plstream->psty(params->styleGet());
		m_fillIsNone = false;
	}
	else
		m_fillIsNone = true;
}
//--------------------------------------------------------------------------------

void EsChPlot::pointParametersApply(const EsChPlotPointParameters::Ptr& params) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(params);
	if( !params->isNone() )
	{
		colorSelect(params->colorGet());
		m_plstream->ssym(params->sizeGet(), 1.);
		m_pointStyle = params->styleGet();
		m_pointIsNone = false;
	}
	else
		m_pointIsNone = true;
}
//--------------------------------------------------------------------------------

void EsChPlot::plotTextRelative(const EsString& txt, double x, double y, double justify/* = 0.*/, double angle/* = 0.*/) const
{
	ES_ASSERT(isOk());
	if( !txt.empty() )
	{
		EsChPlotView::Ptr view = selectedViewGet();
		ES_ASSERT(view);

		EsChPlotAxis::Ptr axis = view->axisGet(0);
		ES_ASSERT(axis);

		x = relativeDeltaToWorldDelta(0, x) + axis->minGet();
		axis = view->axisGet(1);
		ES_ASSERT(axis);

		y = relativeDeltaToWorldDelta(1, y) + axis->minGet();
		plotText(
			txt, 
			x, 
			y, 
			justify, 
			angle
		);
	}
}
//--------------------------------------------------------------------------------

void EsChPlot::plotText(const EsString& txt, double x, double y, double justify/* = 0.*/, double angle/* = 0.*/) const
{
	ES_ASSERT(isOk());
	if( !txt.empty() )
	{
		const EsByteString& str = EsString::toUtf8(txt);
		angle = EsMath::degToRad(angle);
		
		m_plstream->ptex(
			x, 
			y, 
			cos(angle), 
			sin(angle), 
			justify, 
			str.c_str()
		);
	}
}
//--------------------------------------------------------------------------------

void EsChPlot::plotArc(double majorR, double minorR, double centerX/* = 0.*/, double centerY/* = 0.*/,
						 bool fill/* = false*/, double startAngle/* = 0.*/, double endAngle/* = 360*/) const
{
	ES_ASSERT(isOk());
	if( 
		!m_lineIsNone || 
		(fill && !m_fillIsNone)
	)
		m_plstream->arc(
			centerX, 
			centerY, 
			majorR, 
			minorR, 
			startAngle, 
			endAngle, 
			0., 
			fill
		);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotLine(double x1, double y1, double x2, double y2) const
{
	ES_ASSERT(isOk());
	if( !m_lineIsNone )
		m_plstream->join(x1, y1, x2, y2);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotLine(const EsMathArrayReal& x, const EsMathArrayReal& y,
												ulong offs/* = 0*/, ulong cnt/* = 0*/) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(x.get_count() == y.get_count());
	
	if( offs < x.get_count() && !m_lineIsNone )
	{
		ulong sze = x.get_count()-offs;
		ulong n = cnt ? 
			wxMin(cnt, sze) : 
			sze;
		
		if( n > 1 )
			m_plstream->line(
				n, 
				x.dataGet()+offs, 
				y.dataGet()+offs
			);
	}
}
//--------------------------------------------------------------------------------

void EsChPlot::plotLineH(double x, double y, double w) const
{
	plotLine(
		x, 
		y, 
		x+w, 
		y
	);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotLineV(double x, double y, double h) const
{
	plotLine(
		x, 
		y, 
		x, 
		y+h
	);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotErrosX(const EsMathArrayReal& lowErrsX, const EsMathArrayReal& highErrsX, const EsMathArrayReal& y) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(lowErrsX.get_count() == y.get_count() && highErrsX.get_count() == y.get_count());
	
	ulong n = y.get_count();
	if( n && !m_lineIsNone)
		m_plstream->errx(
			n, 
			lowErrsX.dataGet(),
			highErrsX.dataGet(),
			y.dataGet()
		);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotErrosY(const EsMathArrayReal& lowErrsY, const EsMathArrayReal& highErrsY, const EsMathArrayReal& x) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(lowErrsY.get_count() == x.get_count() && highErrsY.get_count() == x.get_count());

	ulong n = x.get_count();
	if( n && !m_lineIsNone)
		m_plstream->erry(
			n, 
			lowErrsY.dataGet(),
			highErrsY.dataGet(),
			x.dataGet()
		);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotPoint(double x, double y) const
{
	ES_ASSERT(isOk());
	if( !m_pointIsNone )
		m_plstream->poin(
			1, 
			&x, 
			&y, 
			m_pointStyle
		);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotPoints(const EsMathArrayReal& x, const EsMathArrayReal& y,	ulong offs/* = 0*/, ulong cnt/* = 0*/) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(x.get_count() == y.get_count());

	if( offs < x.get_count() && !m_pointIsNone )
	{
		ulong sze = x.get_count()-offs;
		ulong n = cnt ? wxMin(cnt, sze) : sze;
		if( n > 0 )
			m_plstream->poin(
				n, 
				x.dataGet(),
				y.dataGet(),
				m_pointStyle
			);
	}
}
//--------------------------------------------------------------------------------

void EsChPlot::plotPoly(const EsMathArrayReal& x, const EsMathArrayReal& y,	ulong offs/* = 0*/, ulong cnt/* = 0*/) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(x.get_count() == y.get_count());

	if( offs < x.get_count() && !m_fillIsNone )
	{
		ulong sze = x.get_count()-offs;
		ulong n = cnt ? wxMin(cnt, sze) : sze;
		if( n > 2 )
			m_plstream->fill(
				n, 
				x.dataGet(),
				y.dataGet()
			);
	}
}
//--------------------------------------------------------------------------------

void EsChPlot::plotPoly(const double* x, const double* y, ulong cnt/* = 0*/) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(x);
	ES_ASSERT(y);

	if(m_fillIsNone)
		return;

	if(cnt > 2)
		m_plstream->fill(
			cnt,
			x,
			y
		);
}
//--------------------------------------------------------------------------------

void EsChPlot::plotLine(double x1, double y1, double z1, double x2, double y2, double z2) const
{
	ES_ASSERT(isOk());
	ES_ASSERT( selectedViewGet() );
	ES_ASSERT( 3 == selectedViewGet()->dimensionGet() ); 

	if( !m_lineIsNone )
	{
		double ax[2] = {x1, x2}, ay[2]= {y1, y2}, az[2] = {z1, z2};
		m_plstream->line3(
			2, 
			ax, 
			ay, 
			az
		);
	}
}

// plot polyline in 3d
void EsChPlot::plotLine(const EsMathArrayReal& x, const EsMathArrayReal& y, const EsMathArrayReal& z) const
{
	ES_ASSERT(isOk());
	ES_ASSERT( selectedViewGet() );
	ES_ASSERT( 3 == selectedViewGet()->dimensionGet() ); 
	ES_ASSERT( x.get_count() == y.get_count() && x.get_count() == z.get_count() );

	if( !m_lineIsNone )
	{
		ulong n = x.get_count();
		if( n > 1 )
			m_plstream->line3(
				n, 
				x.dataGet(),
				y.dataGet(),
				z.dataGet()
			);
	}
}

// hit test services
bool EsChPlot::hitInfoGet(wxCoord x, wxCoord y, EsChPlotObjectHitInfo& info) const
{
	ES_ASSERT(isOk());
	double rx, ry;
	// calculate relative page coordinates from physical device coordinates
	rx = plP_pcdcx(x);
	ry = plP_pcdcy(y);
	double wx, wy;
	int viewIdx;
	m_plstream->calc_world(
		rx, 
		ry, 
		wx, 
		wy, 
		viewIdx
	);

	ES_DEBUG_TRACE(
		esT("EsChPlot::hitInfoGet(x=%d, y=%d)"), 
		x, 
		y
	);
	
	ES_DEBUG_TRACE(
		esT("m_plstream->calc_world(rx=%f, ry=%f, wx=%f, wy=%f, viewIdx=%d)"), 
		rx, 
		ry, 
		wx, 
		wy, 
		viewIdx
	);

	if( 0 <= viewIdx )
	{
		viewIndexCheck(viewIdx);
		internalSelectView(viewIdx);	

		double toleranceX = physicalDeltaToWorldDelta(0, defHitTolerance);
		double toleranceY = physicalDeltaToWorldDelta(1, defHitTolerance);
		
		return hitInfoGet(
			*this, 
			wx, 
			wy, 
			toleranceX, 
			toleranceY, 
			info
		);
	}

	return false;
}

bool EsChPlot::hitInfoGet(const EsChPlot& plot, double wx, double wy, double toleranceX, double toleranceY,	EsChPlotObjectHitInfo& info) const
{
	EsChPlotView::Ptr view = selectedViewGet();
	ES_ASSERT(view);

	return view->hitInfoGet(
		plot, 
		wx, 
		wy, 
		toleranceX, 
		toleranceY, 
		info
	);
}
