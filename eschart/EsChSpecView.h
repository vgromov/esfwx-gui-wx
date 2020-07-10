#ifndef _es_spec_view_h_
#define _es_spec_view_h_

#include <wx/statline.h> 
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>

// spectrum view pane
//
class ESCHART_CLASS EsChSpecView : public wxPanel
{
protected:
	// internal linkage handling codes
	enum LinkUpdateCode {
		startChanged,
		endChanged
	};
	
public:
	enum {
		IDC_SPEC_AUTOSCALE = wxID_HIGHEST + 8000,
		IDC_SPEC_ZOOMIN,
		IDC_SPEC_ZOOMOUT,
		IDC_SPEC_CLEAR,
		IDC_ADD_WINDOW,
		IDC_REMOVE_WINDOW,
		IDC_RENAME_WINDOW,
		// misc constatnts
		defLineWidth				= 2,
		maxLineWidth				= 10,
		minMaxY							= 100,
		spacerWidth					= 2,
		wndCountReserve			= 16
	};
	
	class ESCHART_CLASS Lock
	{
	public:
		Lock(EsChSpecView& specView);
		~Lock();

	private:
		EsChSpecView& m_view;
		EsChPlot::Lock m_plotLock;

		ES_NON_COPYABLE(Lock);
	};
	friend class Lock;

	// spectrometer window
	class ESCHART_CLASS Window : public wxPanel
	{
	public:
		enum { 
			defOpacity = 64
		};
		typedef std::shared_ptr<Window> Ptr;
	
	protected:
		// hidden functionality
		Window(EsChSpecView& owner);
		// internal services
		void fullRecalcWindowData();
		// re-layout this window and sidepanel which is owning it
		void updateLayout();

	public:
		virtual ~Window();	

		// serialization
		void settingsLoad(const EsStreamIntf::Ptr& in, const EsString& key);
		void settingsSave(const EsStreamIntf::Ptr& out, const EsString& key);
		
		// misc public services
		void reset();
		void onPulse(size_t chnl, size_t chnlVal);
		void onNewSpecSize(size_t size);
		void infoUpdate(bool full = false);

		// window properties access
		//
		// name
		EsString nameGet() const { return GetLabel().t_str(); }
		void nameSet(const EsString& name)
		{
			if( name != nameGet() )
				SetLabel( name.as<wxString>() );
		}
		
		// borders change
		ulong startGet() const ES_NOTHROW {	return m_left; }
		void startSet(ulong start);
		
		ulong endGet() const ES_NOTHROW {	return m_right;	}
		void endSet(ulong end);
		
		void rangeSet(ulong start, ulong end)
		{
			startSet(start);
			endSet(end);
		}
		
		// lock editing
		bool lockedGet() const { return m_locked->GetValue();	}
		void lockedSet(bool locked);
		
		// color change
		wxColor colorGet() const { return m_cl->GetColour(); }
		void colorSet(const wxColor& cl);
		
		// opacity change
		uint8_t opacityGet() const { return m_opacity->GetValue(); }
		void opacitySet(uint8_t opacity);

		// misc getters
		ulong sumGet() const ES_NOTHROW { return m_sum; }

		// enable|disable editing
		void enableStartEndEditing(bool enable = true);
		void enableColorEditing(bool enable = true);
		void enableOpacityEditing(bool enable = true);
		void enableLocking(bool enable = true);
		
		// show|hide editing controls
		void showColorEditor(bool show = true);
		void showOpacityEditor(bool show = true);
		
		// base class overrides
		virtual void SetLabel(const wxString& lbl);
		
	protected:
		// event handlers
		void onStartChanged(wxSpinEvent& evt);
		void onEndChanged(wxSpinEvent& evt);
		void onColorChanged(wxColourPickerEvent& evt);
		void onOpacityChanged(wxSpinEvent& evt);
		void onLockChanged(wxCommandEvent& evt);

		// internal services
		void doSetStart(size_t end);
		void doSetEnd(size_t end);
		
		ES_NON_COPYABLE(Window);

	protected:
		EsChSpecView& m_owner;
		// window controls
		wxStaticBoxSizer* m_box;		
		wxFlexGridSizer* m_grid;
		wxSpinCtrl* m_start;
		wxSpinCtrl* m_end;
		wxStaticText* m_lblCl;
		wxColourPickerCtrl* m_cl;
		wxStaticText* m_lblOpacity;
		wxSpinCtrl* m_opacity;
		wxCheckBox* m_locked;
		// chart range marker
		EsChPlotAreaMarker::Ptr m_markerArea;
		// window info labels
		wxStaticText* m_lblSum;
		wxStaticText* m_lblMax;
		wxStaticText* m_lblMaxChnl;

		// internal data cache
		ulong m_left;
		ulong m_right;
		ulong m_sum;			// sum of pulses in window
		ulong m_max;			// maximum value in window
		ulong m_maxIdx;		// window channel where max value is
		// marker movement recursive lock
		int m_suppressMarkerMove;
		// link handling recursion lock
		bool m_handlingLink;
		// setters recursion locks
		bool m_settingStart;
		bool m_settingEnd;

		friend class EsChSpecView;
	};
	typedef std::vector< EsChSpecView::Window::Ptr > WindowsT;

	// internal window linkage stuff
	class WindowLink
	{
	protected:
		WindowLink(Window::Ptr left, Window::Ptr right) :
		m_left(left),
		m_right(right)
		{
			ES_ASSERT(m_left);
			ES_ASSERT(m_right);
			ES_ASSERT(m_left != m_right);
		}
				
	public:	
		bool isParticipating(Window::Ptr wnd) const
		{
			return isParticipating(*(wnd.get()));
		}
		
		bool isParticipating(Window& wnd) const
		{
			return m_left.get() == &wnd || m_right.get() == &wnd;
		}		

		bool isParticipatingLeft(Window& wnd) const
		{
			return m_left.get() == &wnd;
		}		

		bool isParticipatingRight(Window& wnd) const
		{
			return m_right.get() == &wnd;
		}		
		
		bool operator== (const WindowLink& other) const
		{
			return m_left == other.m_left && m_right == other.m_right;
		}
	
		Window::Ptr m_left;
		Window::Ptr m_right;
		friend class EsChSpecView;
	};
	typedef std::vector<WindowLink> WindowLinksT;
	friend class Window;
	
public:	
	EsChSpecView(wxWindow* parent, size_t minSpecRes, size_t maxSpecRes, size_t defSpecRes);
	virtual ~EsChSpecView();

	// access side panel sizer. use with caution
	wxBoxSizer* sidePanelGet()
	{
		return m_sideBox;
	}
	// show|hide spectrometer side panel
	void sidePanelShow(bool show);
	bool sidePanelIsShown() const
	{
		return m_sideBox->IsShown(static_cast<size_t>(0));
	}
	// restrictions access
	size_t minSpecResGet() const ES_NOTHROW { return m_minSpecRes; }
	size_t maxSpecResGet() const ES_NOTHROW { return m_maxSpecRes; }
	size_t defSpecResGet() const ES_NOTHROW { return m_defSpecRes; }

	// spectrum stats access
	double totalGet() const;
	double maxGet() const;
	ulong maxChnlGet() const;

	// spectrum data access
	size_t pulsesGet(ulong chnl) const;
	bool pulseAdd(ulong ampl);
	void dataSet( const EsMathArrayReal& data );
	
	// clear spectrometer data. if resetScale is true, 
	// reset spectrum chart scaling to default as well
	void specClear(bool resetScale = false);	
	
	// spectrometer parameter setter helpers
	void specResolutionSet(size_t sr, bool force = false);
	ulong specResolutionGet() const ES_NOTHROW { return m_specResolution; }

	void specColorSet(wxColor cl, bool force = false);
	wxColor specColorGet() const { return m_dataRenderer->colorGet(); }
	
	void specBarWidthSet(size_t width);
	size_t specBarWidthGet() const { return m_dataRenderer->widthGet(); }

	// window management
	void windowButtonsShow(bool show);
	bool windowButtonsAreShown() const	ES_NOTHROW { return m_windowCtlsShown; }

	Window::Ptr windowAdd(const EsString& name);
	void windowRename();
	void windowRename(ulong idx, const EsString& name);
	
	void windowRemove();
	void windowRemove(ulong idx);	
	
	ulong windowsCountGet() const ES_NOTHROW {	return m_windows.size(); }

	Window::Ptr windowGetAt(ulong idx);
	const Window::Ptr windowGetAt(ulong idx) const;

	// windows linkage manipulation
	//
	
	// add link between left and right
	void windowsLinkAdd(Window::Ptr left, Window::Ptr right);

	// remove all links in which wnd is participating
	void windowLinksRemove(Window::Ptr wnd);
	
	// return true if there already is window with this name
	bool isExistingWindowName(const EsString& name, bool doThrow = false) const;
	
	// return index of the window with specified name, wxNOT_FOUND otherwise
	int windowFindByName(const EsString& name) const;
	
	// return all existing window names in array
	EsString::Array allWindowNamesGet() const;	
	
	// update spectrum stats && axes, if corresponding needXXX flags are set. 
	// Locker object calls this method automatically on unlock
	void update();
	
	// scale manipulation
	void autoScaleSetY();
	bool autoScaleGetY() const ES_NOTHROW {	return m_autoscaleY; }

	void scaleSetY(double scale);
	double scaleGetY() const;
	
	void zoomOutConstraintSet(double constraint);
	double zoomOutConstraintGet() const ES_NOTHROW { return m_zoomOutConstraint; }

	// settings serialization
	void settingsSave(const EsStreamIntf::Ptr& out, const EsString& key);
	void settingsLoad(const EsStreamIntf::Ptr& in, const EsString& key);
	
	// render to bitmap
	wxBitmap getAsBitmap(const wxSize& size) const;
	
protected:
	// internal services
	//
	void evtsConnect();
	void evtsDisconnect();

	// reset generic spectrum information
	void specInfoReset();

	// update spectrum status info
	void specInfoUpdate();

	// window link search. if not found, return wxNOT_FOUND
	int windowLinkFind(Window& wnd, bool right) const;

	// window link handling
	bool linkHandle(Window& wnd, bool pre, LinkUpdateCode code, size_t val);
	void windowCtlButtonsAdd();
	void windowCtlButtonsRemove();

	// event handlers
	//
	void onSpecCtl(wxCommandEvent& evt);
	void onUpdateUi(wxUpdateUIEvent& evt);	
	
protected:
	// nested lock counter
	long m_lock;
	// pending update flags
	bool m_needUpdateInfo;
	bool m_needAdjustAxes;
	// autoscale flags
	bool m_autoscaleY;
	// spectrometer settings
	ulong m_minSpecRes;
	ulong m_maxSpecRes;
	ulong m_defSpecRes;
	ulong m_specResolution;
	// spectrometer windows and window links
	WindowsT m_windows;
	WindowLinksT m_links;
	bool m_windowCtlsShown;
	// spectrometer main panel

	// spectrometer side panel
	wxBoxSizer* m_sideBox;
	wxScrolledWindow* m_sidePanel;
	// zoom && windows management buttons
	wxAuiToolBar* m_specTb;
	double m_zoomOutConstraint;
	// spectrometer common info
	wxStaticText* m_lblSum;
	wxStaticText* m_lblMax;
	// spectrometer data & renderer
	EsChPlotSeries::Ptr m_data;
	EsChPlotSeriesRendererHistogram2d::Ptr m_dataRenderer;
	// plot panel instance
	EsChPlotPanel* m_plotPanel;
};

#endif // _es_spec_view_h_
