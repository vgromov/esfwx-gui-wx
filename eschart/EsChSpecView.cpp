#include "eschart_pch.h"
#pragma hdrstop

#include "EsChSpecView.h"

#include <numeric>
#include <algorithm>
//--------------------------------------------------------------------------------

#include "res/spec-wnd-add.xpm"
#include "res/spec-wnd-rename.xpm"
#include "res/spec-wnd-del.xpm"
#include "res/spec-autoscale.xpm"
#include "res/spec-zoom-in.xpm"
#include "res/spec-zoom-out.xpm"
#include "res/spec-clear.xpm"
//--------------------------------------------------------------------------------

#define STR_DEFAULT_MAX_INFO							_("Max: -\nin channel: -")
#define STR_FMT_MAX_INFO									_("Max: %d\nin channel: %d")
#define STR_FMT_SUM												_("Sum: %d")
#define STR_WINDOW_INDEX									_("Window index")
#define STR_NO_DATA												esT("--")
//--------------------------------------------------------------------------------

EsChSpecView::Lock::Lock(EsChSpecView& specView) :
m_view(specView),
m_plotLock(m_view.m_plotPanel->plotGet())
{
	++m_view.m_lock;
}
//--------------------------------------------------------------------------------

EsChSpecView::Lock::~Lock()
{
	if( m_view.m_lock )
		--m_view.m_lock;

	if( 0 == m_view.m_lock )
		m_view.update();
}
//--------------------------------------------------------------------------------

EsChSpecView::Window::Window(EsChSpecView& owner) :
wxPanel(owner.m_sidePanel),
m_owner(owner),
m_box(0),
m_grid(0),
m_start(0),
m_end(0),
m_lblCl(0),
m_cl(0),
m_lblOpacity(0),
m_opacity(0),
m_suppressMarkerMove(0),
m_handlingLink(false),
m_settingStart(false),
m_settingEnd(false),
m_locked(0),
m_lblSum(0),
m_lblMax(0),
m_left(0),
m_right(0),
m_sum(0),
m_max(0),
m_maxIdx(0)
{
	// initialize spectrometer window controls
	//
	wxSizerFlags spinCtlFlags = wxSizerFlags().Proportion(1).Expand();
	wxSizerFlags lblFlags = wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL);
	wxSizerFlags lblInfoFlags = wxSizerFlags().Border().Expand();
	
	m_box = new wxStaticBoxSizer(wxVERTICAL, this);
	ES_ASSERT(m_box);

	m_grid = new wxFlexGridSizer(2);
	ES_ASSERT(m_grid);
	
	m_grid->AddGrowableCol(1, 1);
	m_box->Add(
		m_grid, 
		wxSizerFlags().Expand().Border()
	);

	// start-stop margins
	wxStaticText* lbl = new wxStaticText(
		this, 
		wxID_ANY, 
		_("Start:").as<wxString>()
	);
	ES_ASSERT(lbl);

	m_grid->Add(lbl, lblFlags);
	m_start = new wxSpinCtrl(
		this, 
		wxID_ANY, 
		wxEmptyString, 
		wxDefaultPosition, 
		wxSize(64, wxDefaultCoord)
	);
	ES_ASSERT(m_start);
	
	m_grid->Add(
		m_start, 
		spinCtlFlags
	);
	
	lbl = new wxStaticText(
		this, 
		wxID_ANY, 
		_("End:").as<wxString>()
	);
	ES_ASSERT(lbl);
	
	m_grid->Add(lbl, lblFlags);
  m_end = new wxSpinCtrl(this);
	ES_ASSERT(m_end);
	
	m_grid->Add(
		m_end, 
		spinCtlFlags
	);
	
	// color settings
	m_lblCl = new wxStaticText(
		this, 
		wxID_ANY, 
		_("Color:").as<wxString>()
	);
	ES_ASSERT(m_lblCl);
	
	m_grid->Add(m_lblCl, lblFlags);
	m_cl = new wxColourPickerCtrl(
		this, 
		wxID_ANY
	);
	ES_ASSERT(m_cl);
	
	m_grid->Add(
		m_cl, 
		wxSizerFlags().Proportion(1).Expand()
	);
	// opacity settings (for area only)
	m_lblOpacity = new wxStaticText(
		this, 
		wxID_ANY, 
		_("Opacity:").as<wxString>()
	);
	ES_ASSERT(m_lblOpacity);
	
	m_grid->Add(
		m_lblOpacity, 
		lblFlags
	);
	m_opacity = new wxSpinCtrl(this);
	ES_ASSERT(m_opacity);
	
	m_opacity->SetRange(0, 255);
	m_grid->Add(
		m_opacity, 
		spinCtlFlags
	);

	// editing lock
	m_locked = new wxCheckBox(
		this, 
		wxID_ANY, 
		_("Window is locked").as<wxString>()
	);
	ES_ASSERT(m_locked);
	
	m_box->Add(
		m_locked, 
		wxSizerFlags().Expand()
	);
	m_box->AddSpacer(EsChSpecView::spacerWidth);
	
	// info labels
	m_lblSum = new wxStaticText(
		this, 
		wxID_ANY, 
		EsString::format( 
			STR_FMT_SUM, 
			m_sum 
		).as<wxString>()
	);
	ES_ASSERT(m_lblSum);
	
	m_box->Add(m_lblSum, lblInfoFlags);
	m_lblMax = new wxStaticText(
		this, 
		wxID_ANY, 
		STR_DEFAULT_MAX_INFO.as<wxString>()
	);
	ES_ASSERT(m_lblMax);
	
	m_box->Add(
		m_lblMax, 
		lblInfoFlags
	);
	
	// create and attach range marker to the m_chart
	m_markerArea = EsChPlotAreaMarker::createVertical();
	ES_ASSERT(m_markerArea);
	
	// initialize color to default value
	colorSet(*wxStockGDI::GetColour(wxStockGDI::COLOUR_GREEN));
	
	// initialize opacity to default value
	opacitySet(defOpacity);
	
	// attach area marker to m_plot
	EsChPlotView::Ptr view = m_owner.m_plotPanel->plotGet()->viewGet(0);
	view->decorationAdd( m_markerArea );
	
	// bind events
	m_start->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onStartChanged, this);
	m_end->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onEndChanged, this);
	m_locked->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &Window::onLockChanged, this);
	m_cl->Bind(wxEVT_COMMAND_COLOURPICKER_CHANGED, &Window::onColorChanged, this);
	m_opacity->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onOpacityChanged, this);
	
	// finalize
	//
	SetSizer(m_box);

	// add itself to the sidebar & layout
	m_owner.m_sidePanel->GetSizer()->Add(
		this, 
		wxSizerFlags().Expand()
	);
	updateLayout();
	onNewSpecSize(m_owner.specResolutionGet());
}
//--------------------------------------------------------------------------------

EsChSpecView::Window::~Window()
{
	// detach area marker from m_plot
	EsChPlotView::Ptr view =
		m_owner.m_plotPanel->plotGet()->viewGet(0);
	view->decorationRemove( m_markerArea );

	// unbind events
	m_start->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onStartChanged, this);
	m_end->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onEndChanged, this);
	m_locked->Unbind(wxEVT_COMMAND_CHECKBOX_CLICKED, &Window::onLockChanged, this);		
	m_cl->Unbind(wxEVT_COMMAND_COLOURPICKER_CHANGED, &Window::onColorChanged, this);
	m_opacity->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onOpacityChanged, this);
	
	// remove itself from the sidepanel and layout it
	m_owner.m_sidePanel->GetSizer()->Detach(this);
	m_owner.Layout();
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::settingsLoad(const EsStreamIntf::Ptr& in, const EsString& key)
{
	ES_ASSERT(in);
	ES_ASSERT(!key.empty());

	EsStreamContextExistingScope ctx(
		in,
		key
	);

	if(!ctx.isOk())
		return;

	const EsString& name = in->valueRead(
		esT("name"), 
		EsString::null()
	).asString();
	
	if( !name.empty() )
		nameSet( name );
	
	ulong start, end;
	start = in->valueRead(
		esT("start"),
		0UL
	).asULong();
	end = in->valueRead(
		esT("end"), 
		0UL
	).asULong();
	
	rangeSet(
		start, 
		end
	);

	wxColor cl;
	cl.Set( 
		in->valueRead(
			esT("color"), 
			esVT("green")
		).asString().as<wxString>()
	);
	
	colorSet( cl );
	
	opacitySet( 
		in->valueRead(
			esT("opacity"), 
			defOpacity
		).asByte() 
	);
	
	lockedSet( 
		in->valueRead(
			esT("locked"), 
			false
		).asBool() 
	);
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::settingsSave(const EsStreamIntf::Ptr& out, const EsString& key)
{
	ES_ASSERT(out);
	ES_ASSERT(!key.empty());

	EsStreamContextExistingOrNewScope ctx(
		out,
		key
	);

	if(!ctx.isOk())
		return;

	out->valueWrite(
		esT("name"), 
		nameGet()
	);
  out->valueWrite(
		esT("start"), 
		startGet()
	);
	out->valueWrite(
		esT("end"), 
		endGet()
	);
	
	out->valueWrite(
		esT("color"), 
		colorGet().GetRGBA()
	);
	out->valueWrite(
		esT("opacity"), 
		opacityGet()
	);
	out->valueWrite(
		esT("locked"), 
		lockedGet()
	);
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::infoUpdate(bool full /*= false*/)
{
	if( full )
		fullRecalcWindowData();
	
	m_lblSum->SetLabel( 
		wxString::Format(
			STR_FMT_SUM.as<wxString>(), 
			m_sum
		) 
	);
	
	if( 0 != m_sum )
		m_lblMax->SetLabel( 
			wxString::Format(
				STR_FMT_MAX_INFO.as<wxString>(), 
				m_max, 
				m_maxIdx
			) 
		);
	else
		m_lblMax->SetLabel( 
			STR_DEFAULT_MAX_INFO.as<wxString>()
		);
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::reset()
{
	m_sum = 0;
	m_max = 0;
	m_maxIdx = 0;

	infoUpdate();
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::onPulse(size_t chnl, size_t chnlVal)
{
	if( chnl >= m_left && chnl <= m_right )
	{
		++m_sum;
	
		if( chnlVal > m_max )
		{
			m_max = chnlVal;
			m_maxIdx = chnl;
		}
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::onNewSpecSize(size_t size)
{
	reset();
	if( endGet() >= size )
		endSet(size-1);

	m_start->SetRange(0, size-1);
	m_end->SetRange(0, size-1);
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::fullRecalcWindowData()
{
	m_sum = 0;
	m_max = 0;
	m_maxIdx = 0;

	const EsMathArrayReal& y = m_owner.m_data->dataGet(1);
	
	if( !y.get_empty() )
	{
		ES_ASSERT( m_left < y.get_count() );
		ES_ASSERT( m_right < y.get_count() );

		for(size_t idx = m_left; idx <= m_right; ++idx)
		{
			size_t val = y.itemGet(idx);
			m_sum += val;
			if( m_max < val )
			{
				m_max = val;
				m_maxIdx = idx;
			}
		}
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::SetLabel(const wxString& lbl)
{
	// check for uniqueness
	if( 
		!m_owner.isExistingWindowName(
			lbl.t_str(), 
			true
		) 
	)
	{
		wxPanel::SetLabel(lbl);
	
		m_box->GetStaticBox()->SetLabel(lbl);
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::startSet(ulong start)
{
	m_settingStart = true;
	doSetStart(start);
	m_settingStart = false;
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::doSetStart(size_t start)
{
	if( 
		start < m_owner.specResolutionGet() &&
		m_owner.linkHandle( 		//< notify owner, for optional window link pre-handling
			*this, 
			true, 
			EsChSpecView::startChanged, 
			start
		) 
	)
	{
		++m_suppressMarkerMove;
		if( m_right < start && !m_settingEnd)
			doSetEnd(start);

		m_left = start;
		m_start->SetValue(start);
		--m_suppressMarkerMove;

		if( 0 == m_suppressMarkerMove )
		{
			m_markerArea->startEndSet(m_left+1, m_right+1);
			// if we're moving with non-zero spectrum data - do full recalc
			infoUpdate(true);
		}
		// notify owner, for optional window post-link handling
		m_handlingLink = true;
		m_owner.linkHandle(*this, false, EsChSpecView::startChanged, start);
		m_handlingLink = false;
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::endSet(ulong end)
{
	m_settingEnd = true;
	doSetEnd(end);
	m_settingEnd = false;
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::doSetEnd(size_t end)
{
	if( end < m_owner.specResolutionGet() &&
		// notify owner, for optional window link pre-handling
		m_owner.linkHandle(*this, true, EsChSpecView::endChanged, end) )
	{
		++m_suppressMarkerMove;
		if( end < m_left && !m_settingStart )
			doSetStart(end);

		m_right = end;
		m_end->SetValue(end);
		--m_suppressMarkerMove;

		if( 0 == m_suppressMarkerMove )
		{
			m_markerArea->startEndSet(m_left+1, m_right+1);
			// if we're moving with non-zero spectrum data - do full recalc
			infoUpdate(true);
		}
		// notify owner, for optional window post-link handling
		m_handlingLink = true;
		m_owner.linkHandle(
			*this, 
			false, 
			EsChSpecView::endChanged, 
			end
		);
		m_handlingLink = false;
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::lockedSet(bool locked)
{
	m_start->Enable(!locked);
	m_end->Enable(!locked);
	m_cl->Enable(!locked);
	m_opacity->Enable(!locked);
	m_locked->SetValue(locked);
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::colorSet(const wxColor& cl)
{
	m_cl->SetColour(cl);
	m_markerArea->lineParametersGet()->colorSet(cl);
	m_markerArea->fillParametersGet()->colorSet(
		wxColor(
			cl.Red(), 
			cl.Green(), 
			cl.Blue(), 
			opacityGet()
		)
	);
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::opacitySet(uint8_t opacity)
{
	m_opacity->SetValue(opacity);
	wxColor bg = m_markerArea->fillParametersGet()->colorGet();
	m_markerArea->fillParametersGet()->colorSet(
		wxColor(
			bg.Red(), 
			bg.Green(), 
			bg.Blue(), 
			opacity
		)
	);
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::enableStartEndEditing(bool enable/* = true*/)
{
	m_start->Enable(enable);
	m_end->Enable(enable);	
	if(enable)
		lockedSet(false);	
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::enableColorEditing(bool enable /*= true*/)
{
	m_cl->Enable(enable);
	if(enable)
		lockedSet(false);	
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::enableOpacityEditing(bool enable /*= true*/)
{
	m_opacity->Enable(enable);
	if(enable)
		lockedSet(false);	
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::updateLayout()
{
	Layout();
	m_owner.m_sidePanel->FitInside();
	m_owner.Layout();
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::enableLocking(bool enable /*= true*/)
{
	m_box->Show(m_locked, enable);
	if( !enable )
		lockedSet(false);
	updateLayout();
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::showColorEditor(bool show /*= true*/)
{
	m_grid->Show( m_lblCl, show );
	m_grid->Show( m_cl, show );
	updateLayout();
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::showOpacityEditor(bool show /*= true*/)
{
	m_grid->Show( m_lblOpacity, show );
	m_grid->Show( m_opacity, show );
	updateLayout();
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::onStartChanged(wxSpinEvent& evt)
{
	startSet(evt.GetValue());
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::onEndChanged(wxSpinEvent& evt)
{
	endSet(evt.GetValue());
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::onLockChanged(wxCommandEvent& evt)
{
	lockedSet(evt.IsChecked());
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::onColorChanged(wxColourPickerEvent& evt)
{
	colorSet( evt.GetColour() );
}
//--------------------------------------------------------------------------------

void EsChSpecView::Window::onOpacityChanged(wxSpinEvent& evt)
{
	opacitySet( evt.GetValue() );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsChSpecView::EsChSpecView(wxWindow* parent, size_t minSpecRes, size_t maxSpecRes, size_t defSpecRes) :
wxPanel(parent),
m_lock(0),
m_needUpdateInfo(false),
m_needAdjustAxes(false),
m_autoscaleY(true),
m_minSpecRes(minSpecRes),
m_maxSpecRes(maxSpecRes),
m_defSpecRes(defSpecRes),
m_sideBox(0),
m_sidePanel(0),
m_specTb(0),
m_zoomOutConstraint(65535),
m_specResolution(0),
m_windowCtlsShown(false),
m_lblSum(0),
m_lblMax(0),
m_plotPanel(0)
{
	ES_ASSERT(	minSpecRes > 0 && minSpecRes <= maxSpecRes && defSpecRes >= minSpecRes && defSpecRes <= maxSpecRes );
	
	// reserve windows && links vector
	//
	m_windows.reserve(8);
	m_links.reserve(16);
	
	// initialize controls 
	//
	wxBoxSizer* contents = new wxBoxSizer(wxHORIZONTAL);
	ES_ASSERT(contents);
	
	// create plot stuff
	//
	m_plotPanel = new EsChPlotPanel(this);
	ES_ASSERT(m_plotPanel);
	
	m_plotPanel->SetSizeHints(320, 240);
	contents->Add(m_plotPanel, 1, wxEXPAND|wxALL, 5);
	
	EsChPlot::Ptr plot = m_plotPanel->plotGet();
	EsChPlot::Lock lock(plot);
	EsChPlotView::Ptr view = plot->viewSet(0, EsChPlotCoordinateSystem_CARTESIAN_2D);
	ES_ASSERT(view);

	EsChPlotAxis::Ptr axisX = view->axisGet(0);
	axisX->titleSet(_("Channel"));
	EsChPlotAxis::Ptr axisY = view->axisGet(1);
	axisY->titleSet(_("Counts"));
	axisY->minMaxSet(0., minMaxY);
	m_data = EsChPlotSeries::create1d();
	ES_ASSERT(m_data);
	
	m_dataRenderer = EsChPlotSeriesRendererHistogram2d::create(defLineWidth, *wxRED);
	ES_ASSERT(m_dataRenderer);
	
	view->seriesAdd(m_data, m_dataRenderer);
	
	// spectrum|windows side panel
	m_sideBox = new wxBoxSizer(wxVERTICAL);
	ES_ASSERT(m_sideBox);
	
	contents->Add(m_sideBox, 0, wxEXPAND, 5);
	
	// common spectrum info labels
	m_lblSum = new wxStaticText(
		this, 
		wxID_ANY, 
		STR_NO_DATA
	);
	ES_ASSERT(m_lblSum);
	
	m_lblSum->SetSizeHints(100, -1);
	m_sideBox->Add(m_lblSum, 0, wxEXPAND);
	
	m_lblMax = new wxStaticText(
		this, 
		wxID_ANY, 
		STR_DEFAULT_MAX_INFO.as<wxString>()
	);
	ES_ASSERT(m_lblMax);
	
	m_sideBox->Add(m_lblMax, 0, wxEXPAND);
	m_sideBox->AddSpacer(EsChSpecView::spacerWidth);
	
	// spectrum control toolbar
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	ES_ASSERT(box);
	
	m_specTb = new wxAuiToolBar(this);
	ES_ASSERT(m_specTb);
	
	m_specTb->AddTool(
		IDC_SPEC_AUTOSCALE, 
		wxEmptyString, 
		wxBitmap(spec_autoscale_xpm, wxBITMAP_TYPE_XPM),
		_("Auto scale").as<wxString>(), 
		wxITEM_CHECK
	);
	m_specTb->AddTool(
		IDC_SPEC_ZOOMIN, 
		wxEmptyString, 
		wxBitmap(spec_zoom_in_xpm, wxBITMAP_TYPE_XPM),
		_("Zoom in").as<wxString>()
	);
	m_specTb->AddTool(
		IDC_SPEC_ZOOMOUT, 
		wxEmptyString, 
		wxBitmap(spec_zoom_out_xpm, wxBITMAP_TYPE_XPM),
		_("Zoom out").as<wxString>()
	);
	m_specTb->AddSeparator();
	m_specTb->AddTool(
		IDC_SPEC_CLEAR, 
		wxEmptyString, 
		wxBitmap(spec_clear_xpm, wxBITMAP_TYPE_XPM),
		_("Clear spectrum").as<wxString>()
	);
	windowCtlButtonsAdd();
	m_specTb->Realize();
	box->Add(m_specTb, wxSizerFlags().Expand());	
	m_sideBox->Add(box, wxSizerFlags().Expand());
	// create vertically scrollable side panel
	m_sidePanel = new wxScrolledWindow(this);
	ES_ASSERT(m_sidePanel);
	m_sideBox->Add(m_sidePanel, 1, wxEXPAND);
	// set-up scroller
	box = new wxBoxSizer(wxVERTICAL);
	ES_ASSERT(box);
	m_sidePanel->SetSizer(box);
	m_sidePanel->SetScrollRate(0, 5);
	
	// set sizer with contents
	SetSizer(contents);
	Fit();
	
	// set default spectrum size
	specResolutionSet(m_defSpecRes, true);
	
	// final layout
	Layout();
	
	evtsConnect();
}
//--------------------------------------------------------------------------------

EsChSpecView::~EsChSpecView()
{
	evtsDisconnect();
}
//--------------------------------------------------------------------------------

void EsChSpecView::evtsConnect()
{
	// setup control events
	Bind(wxEVT_COMMAND_MENU_SELECTED, &EsChSpecView::onSpecCtl, this, IDC_SPEC_AUTOSCALE, IDC_RENAME_WINDOW);	
	Bind(wxEVT_UPDATE_UI, &EsChSpecView::onUpdateUi, this, IDC_SPEC_AUTOSCALE, IDC_RENAME_WINDOW);		
}
//--------------------------------------------------------------------------------

void EsChSpecView::evtsDisconnect()
{
	// clean control events
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &EsChSpecView::onSpecCtl, this, IDC_SPEC_AUTOSCALE, IDC_RENAME_WINDOW);	
	Unbind(wxEVT_UPDATE_UI, &EsChSpecView::onUpdateUi, this, IDC_SPEC_AUTOSCALE, IDC_RENAME_WINDOW);		
}
//--------------------------------------------------------------------------------

void EsChSpecView::specInfoReset()
{
	// reset accumulated values
	m_data->dataGet(1).set_dirty(true);

	// update spectrum status info
	specInfoUpdate();
}
//--------------------------------------------------------------------------------

void EsChSpecView::specResolutionSet(size_t res, bool force /*= false*/)
{
	if( m_specResolution != res || force )
	{
		m_specResolution = res;
		Lock lock(*this);
		m_data->clear();
		m_data->sizeSet(res);
		// reset generic info
		specInfoReset();
		// reset and adjust all windows
		for(size_t idx = 0; idx < m_windows.size(); ++idx)
		{
			Window::Ptr p = m_windows[idx];
			p->onNewSpecSize(res);
		}
		// set-up pending updage flags
		m_needUpdateInfo = true;
		m_needAdjustAxes = true;
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::specColorSet(wxColour cl, bool force /*= false*/)
{
	if( m_dataRenderer->colorGet() != cl || force )
		m_dataRenderer->colorSet(cl);
}
//--------------------------------------------------------------------------------

void EsChSpecView::specBarWidthSet(size_t width)
{
	if( m_dataRenderer->widthGet() != width )
		m_dataRenderer->widthSet(width);
}
//--------------------------------------------------------------------------------

void EsChSpecView::dataSet( const EsMathArrayReal& data )
{
	Lock lock(*this);
	
	if( m_data->sizeGet() != data.get_count() )
		specResolutionSet(data.get_count());
	m_data->dataSet(1, data);

	// update main minimax and window minimax stuff
	m_needUpdateInfo = true;
	m_needAdjustAxes = true;	
}
//--------------------------------------------------------------------------------

void EsChSpecView::windowCtlButtonsAdd()
{
	if( !m_windowCtlsShown )
	{
		m_specTb->AddSeparator();
		m_specTb->AddTool(
			IDC_ADD_WINDOW, 
			wxEmptyString, 
			wxBitmap(spec_wnd_add_xpm, wxBITMAP_TYPE_XPM),
			_("Add spectrometer window").as<wxString>()
		);

		m_specTb->AddTool(
			IDC_RENAME_WINDOW, 
			wxEmptyString, 
			wxBitmap(spec_wnd_rename_xpm, wxBITMAP_TYPE_XPM),
			_("Rename spectrometer window").as<wxString>()
		);
		
		m_specTb->AddTool(
			IDC_REMOVE_WINDOW, 
			wxEmptyString, 
			wxBitmap(spec_wnd_del_xpm, wxBITMAP_TYPE_XPM),
			_("Remove spectrometer window").as<wxString>()
		);
		
		m_specTb->Realize();
		m_windowCtlsShown = true;
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::windowCtlButtonsRemove()
{
	if( m_windowCtlsShown )
	{
		m_specTb->DeleteTool(IDC_REMOVE_WINDOW);
		m_specTb->DeleteTool(IDC_RENAME_WINDOW);
		m_specTb->DeleteTool(IDC_ADD_WINDOW);
		m_specTb->DeleteByIndex(m_specTb->GetToolCount()-1);	// remove left-over separator
		m_windowCtlsShown = false;
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::windowButtonsShow(bool show)
{
	if( show )	
		windowCtlButtonsAdd();
	else
		windowCtlButtonsRemove();
}
//--------------------------------------------------------------------------------

void EsChSpecView::sidePanelShow(bool show)
{
	ES_ASSERT(m_sideBox);
	m_sideBox->Show(show);
	
	Layout();
}
//--------------------------------------------------------------------------------

int EsChSpecView::windowFindByName(const EsString& name) const
{
	for(size_t idx = 0; idx < m_windows.size(); ++idx)
		if( name.compare( m_windows[idx]->GetLabel().t_str() ) == 0 )
			return idx;
		
	return wxNOT_FOUND;
}
//--------------------------------------------------------------------------------

bool EsChSpecView::isExistingWindowName(const EsString& name, bool doThrow) const
{
	bool result = windowFindByName(name) != wxNOT_FOUND;
	
	if( result && doThrow )
		EsException::Throw(
			_("Window with name '%s' already exists"), 
			name
		);
	
	return result;
}
//--------------------------------------------------------------------------------

EsChSpecView::Window::Ptr EsChSpecView::windowGetAt(ulong idx)
{
	EsNumericCheck::checkRangeInteger(
		0, 
		m_windows.size()-1, 
		idx, 
		STR_WINDOW_INDEX
	);
	
	return m_windows[idx];
}
//--------------------------------------------------------------------------------

const EsChSpecView::Window::Ptr EsChSpecView::windowGetAt(ulong idx) const
{
	EsNumericCheck::checkRangeInteger(
		0, 
		m_windows.size()-1, 
		idx, 
		STR_WINDOW_INDEX
	);		
	
	return m_windows[idx];
}
//--------------------------------------------------------------------------------

EsChSpecView::Window::Ptr EsChSpecView::windowAdd(const EsString& name)
{
	EsChSpecView::Window::Ptr result;
	EsString nm = name;
	if( nm.empty() )
		nm = EsString::format( 
			_("Window %d"), 
			m_windows.size() 
		);

	if( !isExistingWindowName(nm, true) )
	{
		result.reset( new Window(*this) );
		if( result.get() )
		{
			m_windows.push_back( result );
			result->SetLabel( nm.as<wxString>() );
		}
	}

	return result;	
}
//--------------------------------------------------------------------------------

struct IsParticipating
{
	IsParticipating(EsChSpecView::Window::Ptr wnd) :
	m_wnd(wnd)
	{}
	
	bool operator() (const EsChSpecView::WindowLink& link) const
	{
		return link.isParticipating(m_wnd);
	}

	EsChSpecView::Window::Ptr m_wnd;
};
//--------------------------------------------------------------------------------

void EsChSpecView::windowsLinkAdd(Window::Ptr left, Window::Ptr right)
{
	m_links.push_back( WindowLink(left, right) );
}
//--------------------------------------------------------------------------------

void EsChSpecView::windowLinksRemove(Window::Ptr wnd)
{
	// remove all links in which this window is participating
	m_links.erase(
		std::remove_if(
			m_links.begin(), 
			m_links.end(), 
			IsParticipating(wnd) 
		),
		m_links.end()
	);
}
//--------------------------------------------------------------------------------

void EsChSpecView::windowRemove(ulong idx)
{
	EsNumericCheck::checkRangeInteger(0, m_windows.size()-1, idx, STR_WINDOW_INDEX);
	Window::Ptr wnd = m_windows[idx];
	// first, remove all links in which this window is participating
	windowLinksRemove(wnd);
	// last, remove window from collection
	m_windows.erase( m_windows.begin()+idx );
}
//--------------------------------------------------------------------------------

void EsChSpecView::windowRename(ulong idx, const EsString& name)
{
	EsNumericCheck::checkRangeInteger(0, m_windows.size()-1, idx, STR_WINDOW_INDEX);
	// check for existing name with throw
	isExistingWindowName(name, true);

	// remove trailing non-printables, check for empty name
	EsString nm = EsString::trim(name, esT(" \t\n\r"));
	if( !nm.empty() )
		m_windows[idx]->SetLabel(nm.as<wxString>());
	else
		EsException::Throw(_("Window name must not be neither empty, nor containing only non-printable symbols"));
}
//--------------------------------------------------------------------------------

void EsChSpecView::specClear(bool resetScale /*= false*/)
{
	Lock lock(*this);
	EsChPlot::Ptr plot= m_plotPanel->plotGet();
	EsChPlotAxis::Ptr axisY = plot->viewGet(0)->axisGet(1);

	m_data->clear();
	if( resetScale )
		axisY->minMaxSet(0., minMaxY);
	// reset generic info
	specInfoReset();
	// reset all windows
	for(size_t idx = 0; idx < m_windows.size(); ++idx)
		m_windows[idx]->reset();	
	m_needUpdateInfo = true;	
	m_needAdjustAxes = true;
}
//--------------------------------------------------------------------------------

double EsChSpecView::totalGet() const
{
	if(!m_data)
		return 0;

	return m_data->totalGet(0);
}
//--------------------------------------------------------------------------------

double EsChSpecView::maxGet() const
{
	if(!m_data)
		return 0;

	return m_data->maxGet(0);
}
//--------------------------------------------------------------------------------

ulong EsChSpecView::maxChnlGet() const
{
	if(!m_data)
		return 0;

	return m_data->maxPosGet(0);
}
//--------------------------------------------------------------------------------

size_t EsChSpecView::pulsesGet(ulong chnl) const
{
	EsNumericCheck::checkRangeInteger(0, m_specResolution-1, chnl, _("Channel"));
	return static_cast<size_t>(m_data->dataGetAt(1, chnl));
}
//--------------------------------------------------------------------------------

bool EsChSpecView::pulseAdd(ulong ampl)
{
	if( ampl < m_specResolution )
	{
		ulong newVal = pulsesGet(ampl)+1;
		m_data->dataSetAt(
			1, 
			ampl, 
			newVal
		);
		
		// pass received pulse to spectrometer windows
		for( size_t idx = 0; idx < m_windows.size(); ++idx )
			m_windows[idx]->onPulse( 
				ampl, 
				newVal 
			);		
		
		m_needUpdateInfo = true;
		m_needAdjustAxes = true;
		
		return true;
	}
	
	return false;
}
//--------------------------------------------------------------------------------

void EsChSpecView::specInfoUpdate()
{
	double total = totalGet();
	m_lblSum->SetLabel( 
		wxString::Format(
			STR_FMT_SUM.as<wxString>(), 
			total
		) 
	);
	if( !EsMath::areEqualFloats(0., total) )
		m_lblMax->SetLabel( 
			wxString::Format(
				STR_FMT_MAX_INFO.as<wxString>(), 
				maxGet(), 
				maxChnlGet()
			) 
		);
	else
		m_lblMax->SetLabel( 
			STR_DEFAULT_MAX_INFO.as<wxString>()
		);
}
//--------------------------------------------------------------------------------

EsString::Array EsChSpecView::allWindowNamesGet() const
{
	EsString::Array result;
	result.reserve(wndCountReserve);

	for(size_t idx = 0; idx < m_windows.size(); ++idx)
		result.push_back( 
			m_windows[idx]->nameGet()
		);
			
	return result;
}
//--------------------------------------------------------------------------------

void EsChSpecView::update()
{
	// update spectrum && windows info
	if( m_needUpdateInfo )
	{
		specInfoUpdate();
		for(size_t idx = 0; idx < m_windows.size(); ++idx)
			m_windows[idx]->infoUpdate(true);
		m_needUpdateInfo = false;
	}
	
	if( m_needAdjustAxes )
	{	
		EsChPlot::Ptr plot = m_plotPanel->plotGet();
		EsChPlotAxis::Ptr axisY = plot->viewGet(0)->axisGet(1);
		EsChPlot::Lock lock(plot);

		plot->viewGet(0)->axisGet(0)->minMaxSet(0, m_specResolution-1);

		// start autoscaling with default scale
		if( m_autoscaleY )
			axisY->minMaxSet(0., minMaxY);
	
		// adjust axes, if needed
		double max = maxGet();
		if( 
			max > minMaxY && 
			m_autoscaleY 
		)
		{
			const EsVariant::Array& maxDivModf = EsMath::modf(max / minMaxY).asVariantCollection();
			double newMax = (maxDivModf[0].asDouble() +
				(
					EsMath::areEqualFloats(
						0., 
						maxDivModf[1].asDouble() 
					) ? 
						1 : 0
				)
			) * minMaxY;
			
			axisY->minMaxSet(
				0., 
				newMax
			);
		}
		
		m_needAdjustAxes = false;
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::autoScaleSetY()
{
	if( !m_autoscaleY )
	{
		m_autoscaleY = true;
		m_needAdjustAxes = true;
		update();		
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::scaleSetY(double scale)
{
	if( scale < minMaxY )
		scale = minMaxY;
		
	m_autoscaleY = false;
	m_needAdjustAxes = true;		

	EsChPlot::Ptr plot = m_plotPanel->plotGet();
	EsChPlotAxis::Ptr axisY = plot->viewGet(0)->axisGet(1);
	axisY->minMaxSet(0., scale);	

	update();
}
//--------------------------------------------------------------------------------

double EsChSpecView::scaleGetY() const
{
	return 0.;

	EsChPlot::Ptr plot = m_plotPanel->plotGet();
	EsChPlotAxis::Ptr axisY = plot->viewGet(0)->axisGet(1);
	axisY->maxGet();
}
//--------------------------------------------------------------------------------

void EsChSpecView::zoomOutConstraintSet(double constraint)
{
	if( constraint <= minMaxY )
	 constraint = minMaxY;
	
	m_zoomOutConstraint = constraint;
}
//--------------------------------------------------------------------------------

int EsChSpecView::windowLinkFind(Window& wnd, bool right) const
{
	for( size_t idx = 0; idx < m_links.size(); ++idx )
	{
		if( right )
		{
			if( m_links[idx].isParticipatingRight(wnd) )
				return idx;
		}
		else
		{
			if( m_links[idx].isParticipatingLeft(wnd) )
				return idx;
		}
	}
	
	return wxNOT_FOUND;
}
//--------------------------------------------------------------------------------

bool EsChSpecView::linkHandle(Window& wnd, bool pre, LinkUpdateCode code, size_t val)
{
	int idx;
	if( pre )
	{
		//// check if we actually may change window border
		//if( startChanged == code )
		//	// find link where this window is right
		//	idx = findLink(wnd, true);
		//else
		//	// find link where this window is left
		//	idx = findLink(wnd, false);
		//
		//if( wxNOT_FOUND != idx )
		//{
		//}
	}
	else
	{
		// update other window
		if( startChanged == code )
			// find link where this window is right
			idx = windowLinkFind(wnd, true);			
		else
			// find link where this window is left
			idx = windowLinkFind(wnd, false);
		
		if( wxNOT_FOUND != idx )
		{
			const WindowLink& link = m_links[idx];
			if( startChanged == code )
			{
				// change left window's right border. avoid recursion
				if( !link.m_left->m_handlingLink )
					link.m_left->endSet(val);
			}
			else
			{
				// change right window's left border
				if( !link.m_right->m_handlingLink )
					link.m_right->startSet(val);
			}
		}
	}
	
	return true;	
}
//--------------------------------------------------------------------------------

void EsChSpecView::windowRename()
{
	wxSingleChoiceDialog dlg(
		this, 
		_("Select window to rename:").as<wxString>(), 
		_("Rename window").as<wxString>(), 
		EsUtilities::esStringArrayToWxArrayString( 
			allWindowNamesGet() 
		)
	);
	
	if( dlg.ShowModal() == wxID_OK )
	{
		int sel = dlg.GetSelection();
		if( sel > wxNOT_FOUND )
		{
			EsString name = m_windows[sel]->nameGet();
			wxTextEntryDialog te(
				this, 
				_("Enter new window name:").as<wxString>(), 
				_("Enter new name").as<wxString>(), 
				name.as<wxString>()
			);

			if( te.ShowModal() == wxID_OK )
			{
				name = te.GetValue().t_str();

				windowRename(
					sel, 
					name
				);
			}
		}
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::windowRemove()
{
	wxSingleChoiceDialog dlg(
		this, 
		_("Select window to remove:").as<wxString>(), 
		_("Remove window").as<wxString>(), 
		EsUtilities::esStringArrayToWxArrayString(
			allWindowNamesGet()
		)
	);

	if( dlg.ShowModal() == wxID_OK )
	{
		int sel = dlg.GetSelection();
		if( sel > wxNOT_FOUND )
			windowRemove(sel);
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::onSpecCtl(wxCommandEvent& evt)
{
	switch( evt.GetId() )
	{
	case IDC_SPEC_AUTOSCALE:
		if( !autoScaleGetY() )
			autoScaleSetY();
		else
			scaleSetY( scaleGetY() );
		break;
	case IDC_SPEC_ZOOMIN:
		scaleSetY( scaleGetY() - 100. );
		break;
	case IDC_SPEC_ZOOMOUT:
		scaleSetY( scaleGetY() + 100. );
		break;
	case IDC_SPEC_CLEAR:
		specClear();
		break;
	case IDC_ADD_WINDOW:
		windowAdd(EsString::null());
		break;
	case IDC_REMOVE_WINDOW:
		windowRemove();
		break;
	case IDC_RENAME_WINDOW:
		windowRename();
		break;	
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::onUpdateUi(wxUpdateUIEvent& evt)
{
	switch( evt.GetId() )
	{
	case IDC_SPEC_AUTOSCALE:
		evt.Check( autoScaleGetY() );
		break;
	case IDC_SPEC_ZOOMIN:
		evt.Enable( scaleGetY() > EsChSpecView::minMaxY );
		break;
	case IDC_SPEC_ZOOMOUT:
		evt.Enable( scaleGetY() < zoomOutConstraintGet() );
		break;
	case IDC_SPEC_CLEAR:
		evt.Enable( 0 != totalGet() );
		break;
	case IDC_RENAME_WINDOW:
	case IDC_REMOVE_WINDOW:
		evt.Enable( !m_windows.empty() );
		break;
	}
}
//--------------------------------------------------------------------------------

void EsChSpecView::settingsSave(const EsStreamIntf::Ptr& out, const EsString& key)
{
	ES_ASSERT(out);

	EsStreamContextExistingOrNewScope ctx(
		out,
		key
	);

	if(!ctx.isOk())
		return;

	// save spectrometer windows
	ulong cnt = windowsCountGet();
	out->valueWrite(
		esT("wndCnt"),
		cnt
	);

	for( size_t idx = 0; idx < cnt; ++idx )
		m_windows[idx]->settingsSave(
			out, 
			EsString::format(
				wxT("wnd_%d"), 
				idx
			)
		);

	out->valueWrite(
		esT("specRes"), 
		specResolutionGet() 
	);
	
	// Appearance settings
	out->valueWrite(
		esT("specColor"), 
		m_dataRenderer->colorGet().GetRGBA()
	);
	
	out->valueWrite(
		esT("specBarWidth"), 
		m_dataRenderer->widthGet()
	);
}
//--------------------------------------------------------------------------------

void EsChSpecView::settingsLoad(const EsStreamIntf::Ptr& in, const EsString& key)
{
	ES_ASSERT(in);

	EsStreamContextExistingScope ctx(
		in,
		key
	);

	// Spectrometer settings
	ulong res = in->valueRead(
		esT("specRes"), 
		m_defSpecRes
	).asULong();
	
	// validate resolution
	if( 
		res < m_minSpecRes || 
		res > m_maxSpecRes 
	)
		res = m_defSpecRes;

	specResolutionSet(
		res, 
		true
	);
	
	// m_chart settings
	wxColor col = in->valueRead(
			esT("specColor"),
			wxTheColourDatabase->Find(wxT("BLUE")).GetRGBA()
		).asULong();

	specColorSet(col, true);
	specBarWidthSet( 
		in->valueRead(
			esT("specBarWidth"), 
			defLineWidth
		).asULong()
	);

	// Load spectrometer windows
	ulong cnt = in->valueRead(
		esT("wndCnt"),
		0UL
	).asULong();

	for(ulong idx = 0; idx < cnt; ++idx)
	{
		Window::Ptr pwnd = windowAdd(EsString::null());
		ES_ASSERT(pwnd);

		pwnd->settingsLoad(
			in,
			EsString::format(
				esT("wnd_%d"),
				idx
			)
		);
	}
}
//--------------------------------------------------------------------------------

wxBitmap EsChSpecView::getAsBitmap(const wxSize& size) const
{
	wxMemoryDC mdc;
	wxBitmap bmp(size, 32);
	ES_ASSERT(bmp.IsOk());
	mdc.SelectObject(bmp);
	
	m_plotPanel->plotGet()->draw(mdc, wxPoint(), size);
	mdc.SelectObject(wxNullBitmap);
	return bmp; 
}
//--------------------------------------------------------------------------------
