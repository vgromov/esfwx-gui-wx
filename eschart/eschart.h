#ifndef _es_chart_h_
#define _es_chart_h_

#ifndef _es_core_h_
#	error escore.h header file must be included prior to this header!
#endif

#ifndef _es_math_h_
#	error "esmath.h must be included prior to this header!"
#endif

#ifndef _es_core_gui_h_
#	error "escore-gui.h must be included prior to this header!"
#endif

#ifdef ES_DYNAMIC_LINKAGE
#	ifdef ESCHART_EXPORTS
#		define ESCHART_CLASS	            ES_EXPORT_SPEC
#		define ESCHART_FUNC(type, decl)	  ES_FUNCEXPORT_SPEC(type, decl)
#		define ESCHART_DATA(type, decl)	  ES_DATAEXPORT_SPEC(type, decl)
#	else
#		define ESCHART_CLASS	            ES_IMPORT_SPEC
#		define ESCHART_FUNC(type, decl)	  ES_FUNCIMPORT_SPEC(type, decl)
#		define ESCHART_DATA(type, decl)	  ES_DATAIMPORT_SPEC(type, decl)
#	endif
#else
#	define ESCHART_CLASS
#	define ESCHART_FUNC(type, decl)     type decl
#	define ESCHART_DATA(type, decl)     type decl
#endif

#include <eschart/EsChEvtDefs.h>

#include <eschart/EsChPlotGeometry.h>
#include <eschart/EsChPlotObject.h>
#include <eschart/EsChPlotPalette.h>
#include <eschart/EsChPlotLineParameters.h>
#include <eschart/EsChPlotFillParameters.h>
#include <eschart/EsChPlotPointParameters.h>
#include <eschart/EsChPlotTextParameters.h>
#include <eschart/EsChPlotAxis.h>
#include <eschart/EsChPlotSeries.h>
#include <eschart/EsChPlotSeriesRenderer.h>
#include <eschart/EsChPlotDecorations.h>
#include <eschart/EsChPlotView.h>
#include <eschart/EsChPlot.h>

// ESCHART UI
#include <eschart/EsChPlotPanel.h>
#include <eschart/EsChSpecView.h>

#endif //< _es_chart_h_
