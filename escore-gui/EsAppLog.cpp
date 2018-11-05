#include "escore_gui_pch.h"
#pragma hdrstop

#include <wx/tokenzr.h>
#include <wx/app.h>
#include <wx/file.h>

#include "EsListViewBase.h"
#include "EsAppLog.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsAppLog::EvtHandler::EvtHandler(EsAppLog& owner) :
EsEventSubscriber(ES_EVTC_APPLOG),
m_owner(owner)
{
  activeSet(false);
}
//--------------------------------------------------------------------------------

void EsAppLog::EvtHandler::notify(const EsEventIntf::Ptr& evt)
{
  ES_ASSERT(evt);

  const EsString& cat = evt->categoryGet();
  if(ES_EVTC_APPLOG == cat)
  {
    const EsVariant::Array& payload = evt->payloadGet().asVariantCollection();

    m_owner.onLogEntryAdd(
      evt->timestampGet(),
      as_<EsAppLogLevel>(evt->idGet()),         //< Level
      payload[0].asString(),                    //< Log message
      payload[1]                                //< Additional log payload
    );
  }
  else if(ES_EVTC_I18N == cat && EsEvtI18n::LocaleChanged == as_<EsEvtI18n>(evt->idGet()))
  {
    m_owner.onI18nLocaleChanged(
      evt->payloadGet().asString()
    );
  }
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// Local app log model implementation
//
class EsAppLogModel : public EsListViewModel
{
public:
  static EsListViewModel::Ptr create()
  {
    EsListViewModel::Ptr obj(new EsAppLogModel);

    return obj;
  }
  
  // Base class override
  virtual wxString fieldTextGet(ulong recIdx, ulong idx) const
  {
    if(0 == idx) //< Timestamp column
    {
      EsDateTime ts = EsDateTime::fromVariant(
        m_data[recIdx][idx]
      );

      return ts.asString(esT("%Y%m%dT%H%M%S.%l")).c_str();
    }
    else if(1 == idx) //< Level column
    {
      EsAppLogLevel lvl = as_<EsAppLogLevel>(m_data[recIdx][idx].asULong());

      return EsAppLog::levelStringGet(lvl);
    }
    else
      return EsListViewModel::fieldTextGet(recIdx, idx);
  }
};
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsString::const_pointer EsAppLog::levelStringGet(EsAppLogLevel lvl)
{
  switch(lvl)
  {
  case EsAppLogLevel::Info:
    return esT("INF");
  case EsAppLogLevel::Warning:
    return esT("WRN");
  case EsAppLogLevel::Error:
    return esT("ERR");
  case EsAppLogLevel::Debug:
    return esT("DBG");
  case EsAppLogLevel::Custom:
    return esT("USR");
  }

  // Pacify compiler
  return esT("???");
}
//--------------------------------------------------------------------------------

EsAppLog::EsAppLog(wxWindow *parent, int logDepth /*= MAX_LOG_DEPTH*/) :
EsListViewBase(
  parent,
  wxID_ANY
),
m_handler(*this),
m_logDepth(
    wxMax(
      as_<int>(MIN_LOG_DEPTH), 
      wxMin(
        as_<int>(MAX_LOG_DEPTH), 
        logDepth
      )
    ) 
),
m_doFileLogging(false),
#ifdef ES_DEBUG
  m_debugData(true)
#else
  m_debugData(false)
#endif

{
  m_style.attrSet(
    as_<int>(EsAppLogLevel::Error),
    *wxBLACK,
    wxColour(255, 128, 128),
    wxNullFont
  ).
  attrSet(
    as_<int>(EsAppLogLevel::Warning), 
    *wxBLACK,
    wxColour(255, 200, 0),
    wxNullFont
  ).
  attrSet(
    as_<int>(EsAppLogLevel::Debug),
    *wxBLACK,
    wxColour(128, 128, 128),
    wxNullFont
  ).
  attrSet(
    as_<int>(EsAppLogLevel::Custom),
    *wxBLACK,
    wxColour(128, 128, 255),
    wxNullFont
  );

  m_tmrRefresh.SetOwner(
    this,
    escoreguiID_APPLOG_REFRESH
  );

  // Set-up model
  EsListViewModel::Ptr model = EsAppLogModel::create();
  ES_ASSERT(model);
  
  model->fieldAdd(
    _i("Timestamp"),
    100
  ).
  fieldAdd(
    _i("Level"),
    60
  ).
  fieldAdd(
    _i("Message"),
    300
  ).
  fieldAdd(
    _i("Extra"),
    300
  );
  modelSet(model);

  m_tmrRefresh.Start(2000);
  m_handler.activeSet(true);

  Bind(
    wxEVT_TIMER, 
    &EsAppLog::onRefresh,
    this, 
    escoreguiID_APPLOG_REFRESH
  );
}
//--------------------------------------------------------------------------------

EsAppLog::~EsAppLog()
{
  m_handler.activeSet(false);
  m_tmrRefresh.Stop();

	Unbind(
    wxEVT_TIMER, 
    &EsAppLog::onRefresh, 
    this, 
    escoreguiID_APPLOG_REFRESH
  );

  if( m_flog.IsOpened() )
    m_flog.Close();
}
//--------------------------------------------------------------------------------

void EsAppLog::enableFileLogging(bool enable)
{
	if( enable )
	{
    if(!m_doFileLogging)
    {
      m_flog.Create(
        constructNewLogFileName().c_str(),
        true
      );
    }
	}
	else if( m_doFileLogging && m_flog.IsOpened() )
		m_flog.Close();
	
	m_doFileLogging = enable;
}
//--------------------------------------------------------------------------------

void EsAppLog::onI18nLocaleChanged(const EsString& loc)
{
  EsListViewBase::onI18nLocaleChanged(loc);
  // TODO:
}
//--------------------------------------------------------------------------------

void EsAppLog::onRefresh(wxTimerEvent& WXUNUSED(evt))
{
  refresh();
}
//--------------------------------------------------------------------------------

void EsAppLog::onLogEntryAdd(const EsDateTime& ts, EsAppLogLevel lvl, const EsString& msg, const EsVariant& payload)
{
  ES_ASSERT(m_model);

  const EsVariant::Array rec = {
    EsDateTime::toVariant(ts),
    as_<ulong>(lvl),
    msg,
    payload
  };

  ulong newIdx = m_model->recordAppend( rec );

  if(m_flog.IsOpened())
    m_flog.Write(
      m_model->asString(newIdx),
      wxConvUTF8
    );
	
	if(m_model->recordCountGet() > m_logDepth )
    m_model->recordDelete(0);

	m_needRefresh = true;
}
//--------------------------------------------------------------------------------

void EsAppLog::clear()
{
  if(m_flog.IsOpened())
    m_flog.Close();

  // Create new log file
  if(m_doFileLogging)
  {
    m_flog.Create(
      constructNewLogFileName().c_str(),
      true
    );
  }

  EsListViewBase::clear();
}
//--------------------------------------------------------------------------------

EsString EsAppLog::constructNewLogFileName()
{
  EsString fname = wxTheApp->GetAppName().t_str();
  fname += EsDateTime::now().asString();

	const EsPath& fn = EsPath::create(
    EsPath::stdDocsGet(),
    EsPath::makeSafe(
      fname
    ),
		esT("log")
  );
								
	return fn.pathGet();
}
//--------------------------------------------------------------------------------

int EsAppLog::keyFromItemGet(long item) const
{
  if(m_model && static_cast<ulong>(item) < m_model->recordCountGet())
    return m_model->fieldDataGet(item, 1).asInt();

  return -1;
}
//--------------------------------------------------------------------------------

void EsAppLog::settingsLoad(const EsStreamIntf::Ptr& in)
{
  ES_ASSERT(in);

  EsStreamContextExistingScope scope(in, esT("APPLOG"));
  if(scope.isOk())
  {
    // Read column width values
    for(long idx = 0; idx < GetColumnCount(); ++idx)
    {
      const EsString& colKey = EsString::format(
        esT("COL_%d"),
        idx
      );

      SetColumnWidth(
        idx, 
        in->valueRead(
          colKey,
          GetColumnWidth(idx)
        ).asInt()
      );
    }

    m_debugData = in->valueRead(
      esT("DBG_DATA"), 
#ifdef _DEBUG
      true
#else		
      false
#endif
    ).asBool();

    enableFileLogging(
      in->valueRead(
        esT("LOG_TO_FILE"), 
        false
      ).asBool()
    );
  }
}
//--------------------------------------------------------------------------------

void EsAppLog::settingsSave(const EsStreamIntf::Ptr& out)
{
  ES_ASSERT(out);

  EsStreamContextExistingOrNewScope scope(out, esT("APPLOG"));
  if(scope.isOk())
  {
    // Write column width values	
    for(long idx = 0; idx < GetColumnCount(); ++idx)
    {
      const EsString& colKey = EsString::format(
        esT("COL_%d"),
        idx
      );

      out->valueWrite(
        colKey, 
        GetColumnWidth(idx)
      );
    }

    out->valueWrite(
      esT("LOG_TO_FILE"),
      isFileLoggingEnabled()
    );
  }
}
//--------------------------------------------------------------------------------

void EsAppLog::onClear(wxCommandEvent& ES_UNUSED(evt))
{
	clear();
}
//--------------------------------------------------------------------------------

void EsAppLog::onLogToFile(wxCommandEvent& ES_UNUSED(evt))
{
	enableFileLogging( 
    !isFileLoggingEnabled() 
  );
}
//--------------------------------------------------------------------------------

void EsAppLog::onUpdateUi(wxUpdateUIEvent& evt)
{
	switch(evt.GetId())
	{
	case escoreguiID_ACTIONS_CLEAR_LOG:
		evt.Enable( m_model && !m_model->isEmpty() );
		break;
	case escoreguiD_ACTIONS_LOG_TO_FILE:
		evt.Check( isFileLoggingEnabled() );
		break;
	}
}
//--------------------------------------------------------------------------------

void EsAppLog::ctxMenuInit()
{
  m_pmnu->Append(
    escoreguiID_ACTIONS_CLEAR_LOG,
    _("Clear log"),
    _("Clear application information log")
  );
  m_pmnu->AppendCheckItem(
    escoreguiD_ACTIONS_LOG_TO_FILE,
    _("Log to file"),
    _("Toggle application information logging to file")
  );

  const wxSize& appmnuimgsze = EsUtilities::appSizePropertyGet(ES_APP_OPT_TOOLBAR_IMG_WH);

  EsUtilities::menuItemBitmapSet(
    m_pmnu.get(),
    escoreguiID_ACTIONS_CLEAR_LOG,
    appmnuimgsze,
    wxART_OTHER,
    wxART_DELETE
  );

  EsUtilities::menuItemBitmapSet(
    m_pmnu.get(),
    escoreguiD_ACTIONS_LOG_TO_FILE,
    appmnuimgsze,
    wxART_OTHER,
    wxART_FILE_SAVE
  );
}
//--------------------------------------------------------------------------------

void EsAppLog::toolBarInit()
{
  wxMenuPtr pmnu = ctxMenuGet();
  
  EsUtilities::toolbarToolAddFromMenuItem(
    m_ptb,
    pmnu.get(),
    escoreguiID_ACTIONS_CLEAR_LOG,
    wxART_TOOLBAR,
    wxART_DELETE
  );
  EsUtilities::toolbarToolAddFromMenuItem(
    m_ptb,
    pmnu.get(),
    escoreguiD_ACTIONS_LOG_TO_FILE,
    wxART_TOOLBAR,
    wxART_FILE_SAVE
  );
}
//--------------------------------------------------------------------------------

void EsAppLog::evtHandlersSet(wxEvtHandler* subj, bool set)
{
  ES_ASSERT(subj);

  if( set )
  {
    subj->Bind(
      wxEVT_MENU,
      &EsAppLog::onClear,
      this,
      escoreguiID_ACTIONS_CLEAR_LOG
    );
    subj->Bind(
      wxEVT_MENU,
      &EsAppLog::onLogToFile,
      this,
      escoreguiD_ACTIONS_LOG_TO_FILE
    );
    subj->Bind(
      wxEVT_UPDATE_UI,
      &EsAppLog::onUpdateUi,
      this,
      escoreguiID_ACTIONS_CLEAR_LOG,
      escoreguiD_ACTIONS_LOG_TO_FILE
    );
  }
  else
  {
    subj->Unbind(
      wxEVT_MENU,
      &EsAppLog::onClear,
      this,
      escoreguiID_ACTIONS_CLEAR_LOG
    );
    subj->Unbind(
      wxEVT_MENU,
      &EsAppLog::onLogToFile,
      this,
      escoreguiD_ACTIONS_LOG_TO_FILE
    );
    subj->Unbind(
      wxEVT_UPDATE_UI,
      &EsAppLog::onUpdateUi,
      this,
      escoreguiID_ACTIONS_CLEAR_LOG,
      escoreguiD_ACTIONS_LOG_TO_FILE
    );
  }
}
//--------------------------------------------------------------------------------
