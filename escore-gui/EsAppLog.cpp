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
EsEventSubscriber(
  ES_EVTC_APPLOG ES_EVTC_AND
  ES_EVTC_I18N
),
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
  else if(
    ES_EVTC_I18N == cat && 
    EsEvtI18n::LocaleChanged == as_<EsEvtI18n>(evt->idGet())
  )
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
class EsAppLogModel : public EsListViewModelArray
{
public:
  enum {
    ColumnIdxTs,
    ColumnIdxLevel,
    ColumnIdxMessage,
    ColumnIdxDetail
  };

public:
  static EsListViewModelBase::Ptr create()
  {
    EsListViewModelBase::Ptr obj(new EsAppLogModel);

    // Initialize visual attribution
    EsDataViewStyle::Ptr style =  EsDataViewStyle::create();
    ES_ASSERT(style);

    style->attrSet(
      as_<int>(EsAppLogLevel::Error),
      wxTheColourDatabase->Find("BROWN"),
      wxTheColourDatabase->Find("PINK"),
      true
    ).
    attrSet(
      as_<int>(EsAppLogLevel::Warning),
      wxTheColourDatabase->Find("GOLDENROD"),
      wxTheColourDatabase->Find("WHEAT"),
      true
    ).
    attrSet(
      as_<int>(EsAppLogLevel::Debug),
      wxTheColourDatabase->Find("DARK SLATE BLUE"),
      wxTheColourDatabase->Find("LIGHT GREY"),
      false,
      true
    );

    obj->styleSet(style);

    return obj;
  }
  
  // Base class override
  virtual wxString fieldTextGet(ulong recIdx, ulong fldIdx) const ES_OVERRIDE
  {
    if(ColumnIdxTs == fldIdx) //< Timestamp column
    {
      EsDateTime ts = EsDateTime::fromVariant(
        fieldDataGet(
          recIdx, 
          fldIdx
        )
      );

      return ts.asString(esT("%Y%m%dT%H%M%S.%l")).as<wxString>();
    }
    else if(ColumnIdxLevel == fldIdx) //< Level column
    {
      EsAppLogLevel lvl = as_<EsAppLogLevel>(
        fieldDataGet(
          recIdx,
          fldIdx
        ).asULong()
      );

      return EsAppLog::levelStringGet(lvl);
    }
    else
      return EsListViewModelArray::fieldTextGet(
        recIdx, 
        fldIdx
      );
  }

protected:
  // Use log level as visual attribute key
  virtual int valueStyleKeyGet(unsigned int row, unsigned int col) const ES_OVERRIDE
  {
    return fieldDataGet(
      row, 
      ColumnIdxLevel
    ).asInt();
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
  m_tmrRefresh.SetOwner(
    this,
    escoreguiID_APPLOG_REFRESH
  );

  // Set-up model
  EsListViewModelBase::Ptr model = EsAppLogModel::create();
  ES_ASSERT(model);
  
  model->fieldAdd(
    _i("Timestamp"),
    "string",
    100
  ).
  fieldAdd(
    _i("Level"),
    "string",
    60
  ).
  fieldAdd(
    _i("Message"),
    "string",
    300
  ).
  fieldAdd(
    _i("Extra"),
    "string",
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
        constructNewLogFileName().as<wxString>(),
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

  if( m_pmnu )
  {
    EsUtilities::menuItemLabelAndHintUpdate(
      m_pmnu.get(),
      escoreguiID_ACTIONS_CLEAR_LOG,
      _("Clear log").as<wxString>(),
      _("Clear application information log").as<wxString>()
    );

    EsUtilities::menuItemLabelAndHintUpdate(
      m_pmnu.get(),
      escoreguiID_ACTIONS_LOG_TO_FILE,
      _("Log to file").as<wxString>(),
      _("Toggle application information logging to file").as<wxString>() 
    );
  }

  EsUtilities::toolbarItemsLabelsAndHintsUpdateFromMenu(
    m_ptb,
    m_pmnu.get()
  );

  if( !m_model )
    return;

  wxString name;
  unsigned cnt = GetColumnCount();
  for(unsigned idx = 0; idx < cnt; ++idx)
  {
    wxDataViewColumn* col = GetColumn(idx);
    if(!col)
      continue;

    name = m_model->fieldNameGet(idx);
    col->SetTitle(
      EsStringI18n::translate(
        name.t_str()
      ).as<wxString>()
    );
  }
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
      constructNewLogFileName().as<wxString>(),
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

void EsAppLog::doRefresh()
{
  ES_ASSERT(m_model);

  wxDataViewItem sel = GetSelection();
  m_model->refresh();

  if( !sel.IsOk() )
  {
    int cnt = m_model->recordCountGet();
    if( cnt )
    {
      sel = GetItemByRow(
        cnt-1
      );
    }
  }

  if( sel.IsOk() )
    EnsureVisible( 
      sel
    );
}
//--------------------------------------------------------------------------------

void EsAppLog::settingsLoad(const EsStreamIntf::Ptr& in)
{
  ES_ASSERT(in);

  EsStreamContextExistingScope scope(in, esT("APPLOG"));
  if(scope.isOk())
  {
    // Read column width values
    for(unsigned idx = 0; idx < GetColumnCount(); ++idx)
    {
      const EsString& colKey = EsString::format(
        esT("COL_%d"),
        idx
      );

      wxDataViewColumn* col = GetColumn(idx);
      col->SetWidth(
        in->valueRead(
          colKey,
          col->GetMinWidth()
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
    for(unsigned idx = 0; idx < GetColumnCount(); ++idx)
    {
      const EsString& colKey = EsString::format(
        esT("COL_%d"),
        idx
      );

      wxDataViewColumn* col = GetColumn(idx);
      out->valueWrite(
        colKey, 
        std::max(
          col->GetWidth(),
          col->GetMinWidth()
        )
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
	case escoreguiID_ACTIONS_LOG_TO_FILE:
		evt.Check( isFileLoggingEnabled() );
		break;
	}
}
//--------------------------------------------------------------------------------

void EsAppLog::ctxMenuInit()
{
  m_pmnu->Append(
    escoreguiID_ACTIONS_CLEAR_LOG,
    _("Clear log").as<wxString>(),
    _("Clear application information log").as<wxString>()
  );
  m_pmnu->AppendCheckItem(
    escoreguiID_ACTIONS_LOG_TO_FILE,
    _("Log to file").as<wxString>(),
    _("Toggle application information logging to file").as<wxString>()
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
    escoreguiID_ACTIONS_LOG_TO_FILE,
    appmnuimgsze,
    wxART_OTHER,
    wxART_FILE_SAVE
  );
}
//--------------------------------------------------------------------------------

void EsAppLog::toolBarInit()
{
  wxMenuSharedPtr pmnu = ctxMenuGet();
  
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
    escoreguiID_ACTIONS_LOG_TO_FILE,
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
      escoreguiID_ACTIONS_LOG_TO_FILE
    );
    subj->Bind(
      wxEVT_UPDATE_UI,
      &EsAppLog::onUpdateUi,
      this,
      escoreguiID_ACTIONS_CLEAR_LOG,
      escoreguiID_ACTIONS_LOG_TO_FILE
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
      escoreguiID_ACTIONS_LOG_TO_FILE
    );
    subj->Unbind(
      wxEVT_UPDATE_UI,
      &EsAppLog::onUpdateUi,
      this,
      escoreguiID_ACTIONS_CLEAR_LOG,
      escoreguiID_ACTIONS_LOG_TO_FILE
    );
  }
}
//--------------------------------------------------------------------------------
