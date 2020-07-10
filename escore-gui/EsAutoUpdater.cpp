#include "escore_gui_pch.h"
#pragma hdrstop

#include "EsAutoUpdater.h"
//---------------------------------------------------------------------------

#if defined(ES_USE_AUTO_UPDATER)
#if ES_OS == ES_OS_WINDOWS
//---------------------------------------------------------------------------

EsAutoUpdater::EsAutoUpdater() :
  m_initialized(false),
  m_setup(false),
  win_sparkle_set_lang(nullptr),
  win_sparkle_init(nullptr),
  win_sparkle_cleanup(nullptr),
  win_sparkle_set_appcast_url(nullptr),
  win_sparkle_set_app_details(nullptr),
  win_sparkle_get_last_check_time(nullptr),
  win_sparkle_set_automatic_check_for_updates(nullptr),
  win_sparkle_get_automatic_check_for_updates(nullptr),
  win_sparkle_set_update_check_interval(nullptr),
  win_sparkle_get_update_check_interval(nullptr),
  win_sparkle_check_update_with_ui(nullptr),
  win_sparkle_check_update_without_ui(nullptr)
{
  m_ws = EsDynamicLibrary::load(
#ifdef _WIN64
    esT("winsparkle64.dll"),
#else
    esT("winsparkle.dll"),
#endif
    false
  );

  if(m_ws && m_ws->isOk())
  {
    win_sparkle_set_lang = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_set_lang>(m_ws->procAddrGet(esT("win_sparkle_set_lang"), false));
    win_sparkle_init = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_init>(m_ws->procAddrGet(esT("win_sparkle_init"), false));
    win_sparkle_cleanup = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_cleanup>(m_ws->procAddrGet(esT("win_sparkle_cleanup"), false));
    win_sparkle_set_appcast_url = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_set_appcast_url>(m_ws->procAddrGet(esT("win_sparkle_set_appcast_url"), false));
    win_sparkle_set_app_details = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_set_app_details>(m_ws->procAddrGet(esT("win_sparkle_set_app_details"), false));
    win_sparkle_get_last_check_time = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_get_last_check_time>(m_ws->procAddrGet(esT("win_sparkle_get_last_check_time"), false));
    win_sparkle_set_automatic_check_for_updates = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_set_automatic_check_for_updates>(m_ws->procAddrGet(esT("win_sparkle_set_automatic_check_for_updates"), false));
    win_sparkle_get_automatic_check_for_updates = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_get_automatic_check_for_updates>(m_ws->procAddrGet(esT("win_sparkle_get_automatic_check_for_updates"), false));
    win_sparkle_set_update_check_interval = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_set_update_check_interval>(m_ws->procAddrGet(esT("win_sparkle_set_update_check_interval"), false));
    win_sparkle_get_update_check_interval = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_get_update_check_interval>(m_ws->procAddrGet(esT("win_sparkle_get_update_check_interval"), false));
    win_sparkle_check_update_with_ui = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_check_update_with_ui>(m_ws->procAddrGet(esT("win_sparkle_check_update_with_ui"), false));

    win_sparkle_check_update_without_ui = esCallCastToCall<EsDynamicLibrary::Pfn, pfn_win_sparkle_check_update_without_ui>(m_ws->procAddrGet(esT("win_sparkle_check_update_without_ui"), false));
   
    m_setup =
      win_sparkle_init &&
      win_sparkle_cleanup &&
      win_sparkle_set_appcast_url &&
      win_sparkle_set_app_details &&
      win_sparkle_get_last_check_time &&
      win_sparkle_set_automatic_check_for_updates &&
      win_sparkle_get_automatic_check_for_updates &&
      win_sparkle_set_update_check_interval &&
      win_sparkle_get_update_check_interval &&
      win_sparkle_check_update_with_ui &&
      win_sparkle_check_update_without_ui;
  }
}
//--------------------------------------------------------------------------------

EsAutoUpdater::EsAutoUpdater(
  const EsString& appVendor,
  const EsString& appName,
  const EsString& appVer 
) :
  EsAutoUpdater()
{
  appDetailsSet(
    appVendor, 
    appName, 
    appVer
  );
}
//---------------------------------------------------------------------------

EsAutoUpdater::~EsAutoUpdater()
{
  cleanup();

  m_setup = false;
  win_sparkle_set_lang = nullptr;
  win_sparkle_init = nullptr;
  win_sparkle_cleanup = nullptr;
  win_sparkle_set_appcast_url = nullptr;
  win_sparkle_set_app_details = nullptr;
  win_sparkle_get_last_check_time = nullptr;
  win_sparkle_set_automatic_check_for_updates = nullptr;
  win_sparkle_get_automatic_check_for_updates = nullptr;
  win_sparkle_set_update_check_interval = nullptr;
  win_sparkle_get_update_check_interval = nullptr;
  win_sparkle_check_update_with_ui = nullptr;
  win_sparkle_check_update_without_ui = nullptr;
  m_ws.reset();
}
//---------------------------------------------------------------------------

void EsAutoUpdater::init()
{
  if( m_setup && !m_initialized )
  {
    win_sparkle_set_app_details(
      m_appVendor.c_str(),
      m_appName.c_str(),
      m_appVer.c_str()
    );

    win_sparkle_set_appcast_url(
      EsString::toUtf8(m_updcastUrl).c_str()
    );

    if( win_sparkle_set_lang )
      win_sparkle_set_lang( EsString::toAscii(m_lang).c_str() );

    win_sparkle_init();
    m_initialized = true;
  }
}
//---------------------------------------------------------------------------

void EsAutoUpdater::cleanup()
{
  if( m_setup && m_initialized )
  {
    win_sparkle_cleanup();
    m_initialized = false;
  }
}
//---------------------------------------------------------------------------

bool EsAutoUpdater::canCheckUpdate() const
{
  return m_setup &&
    m_ws &&
    m_ws->isOk();
}
//---------------------------------------------------------------------------

void EsAutoUpdater::langSet(const EsString& lang)
{
  m_lang = lang;

  if( m_setup )
    cleanup();
}
//---------------------------------------------------------------------------

void EsAutoUpdater::appDetailsSet(
  const EsString& appVendor,
  const EsString& appName,
  const EsString& appVer 
)
{
  m_appVendor = appVendor;
  m_appName = appName;
  m_appVer = appVer;

  if( m_setup )
    cleanup();
}
//---------------------------------------------------------------------------

void EsAutoUpdater::updateCastUrlSet(const EsString& url)
{
  m_updcastUrl = url;

  if( m_setup )
    cleanup();
}
//---------------------------------------------------------------------------

void EsAutoUpdater::updateCheck(bool withUi /*= false*/)
{
  if( canCheckUpdate() )
  {
    ES_ASSERT( !m_updcastUrl.empty() );

    init();

    if( withUi )
      win_sparkle_check_update_with_ui();
    else
      win_sparkle_check_update_without_ui();
  }
}
//---------------------------------------------------------------------------

void EsAutoUpdater::updateCheck(const EsString& url, bool withUi /*= false*/)
{
  updateCastUrlSet(url);
  updateCheck(withUi);
}
//---------------------------------------------------------------------------

EsDateTime EsAutoUpdater::lastCheckTimeGet() const
{
  EsDateTime result;
  if( m_setup )
  {
    time_t tt = win_sparkle_get_last_check_time();

    if( -1 < tt )
    {
      struct tm* ptm =
        localtime(&tt);

      result.compose(
        ptm->tm_year + 1900,
        ptm->tm_mon+1,
        ptm->tm_mday,
			  ptm->tm_hour,
        ptm->tm_min,
        ptm->tm_sec,
        0
      );
    }
  }

  return result;
}
//---------------------------------------------------------------------------

bool EsAutoUpdater::autoUpdateGet() const
{
  if( m_setup )
    return 1 == win_sparkle_get_automatic_check_for_updates();

  return false;
}
//---------------------------------------------------------------------------

void EsAutoUpdater::autoUpdateSet(bool au)
{
  if( m_setup )
    win_sparkle_set_automatic_check_for_updates( au ? 1 : 0 );
}
//---------------------------------------------------------------------------

ulong EsAutoUpdater::autoUpdateIntervalGet() const
{
  if( m_setup )
    return win_sparkle_get_update_check_interval();

  return 0;
}
//---------------------------------------------------------------------------

void EsAutoUpdater::autoUpdateIntervalSet(ulong sec)
{
  if( m_setup )
    win_sparkle_set_update_check_interval(sec);
}
//---------------------------------------------------------------------------

#endif // ES_OS == ES_OS_WINDOWS
#endif // ES_USE_AUTO_UPDATER


