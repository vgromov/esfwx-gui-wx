#ifndef EsAutoUpdaterH
#define EsAutoUpdaterH

#if defined(ES_USE_AUTO_UPDATER)
#if ES_OS == ES_OS_WINDOWS

/// Auto Updater class.
/// Wrapper for WinSparkle.dll ver 0.4
///
class ESCORE_GUI_CLASS EsAutoUpdater
{
protected:
  typedef void (__cdecl *pfn_win_sparkle_set_lang)(const char* lang);
  typedef void (__cdecl *pfn_win_sparkle_init)(void);
  typedef void (__cdecl *pfn_win_sparkle_cleanup)(void);
  typedef void (__cdecl *pfn_win_sparkle_set_appcast_url)(const char *url);
  typedef void (__cdecl *pfn_win_sparkle_set_app_details)(const wchar_t *company_name,
                                               const wchar_t *app_name,
                                               const wchar_t *app_version);
  typedef time_t (__cdecl *pfn_win_sparkle_get_last_check_time)(void);
  typedef void (__cdecl *pfn_win_sparkle_set_automatic_check_for_updates)(int state);
  typedef int (__cdecl *pfn_win_sparkle_get_automatic_check_for_updates)(void);
  typedef void (__cdecl *pfn_win_sparkle_set_update_check_interval)(int interval);
  typedef int (__cdecl *pfn_win_sparkle_get_update_check_interval)(void);
  typedef void (__cdecl *pfn_win_sparkle_check_update_with_ui)(void);
  typedef void (__cdecl *pfn_win_sparkle_check_update_without_ui)(void);

public:
  EsAutoUpdater();
  EsAutoUpdater(
    const EsString& appVendor,
    const EsString& appName,
    const EsString& appVer 
  );
  
  ~EsAutoUpdater();

  /// Return true, if update checker is set-up properly, and is able to check for updates
  bool canCheckUpdate() const;

  /// Set-up GUI winsparkle language id (valid ISO code)
  void langSet(const EsString& lang);

  /// Set-up application details
  void appDetailsSet(
    const EsString& appVendor,
    const EsString& appName,
    const EsString& appVer 
  );

  /// Get update cast URL
  const EsString& updateCastUrlGet() const { return m_updcastUrl; }

  /// Set update cast URL
  void updateCastUrlSet(const EsString& url);

  /// Manually check for updates. Existing appcast URL is used
  void updateCheck(bool withUi = false);

  /// Manually check for updates, using specified appcast URL
  void updateCheck(const EsString& url, bool withUi = false);

  /// Get last update check time. Return zero (invalid) date time, if never checked
  EsDateTime lastCheckTimeGet() const;

  /// Return true, if automatic update is enabled, false otherwise
  bool autoUpdateGet() const;

  /// Enable|disable auto update status
  void autoUpdateSet(bool au);

  /// Get auto update check interval, in seconds
  ulong autoUpdateIntervalGet() const;

  /// Set auto update interval in seconds
  void autoUpdateIntervalSet(ulong sec);

protected:
  void init();
  void cleanup();

protected:
  EsDynamicLibrary::Ptr m_ws;
  // WinSparkle method wrappers
  pfn_win_sparkle_set_lang win_sparkle_set_lang;
  pfn_win_sparkle_init win_sparkle_init;
  pfn_win_sparkle_cleanup win_sparkle_cleanup;
  pfn_win_sparkle_set_appcast_url win_sparkle_set_appcast_url;
  pfn_win_sparkle_set_app_details win_sparkle_set_app_details;
  pfn_win_sparkle_get_last_check_time win_sparkle_get_last_check_time;
  pfn_win_sparkle_set_automatic_check_for_updates win_sparkle_set_automatic_check_for_updates;
  pfn_win_sparkle_get_automatic_check_for_updates win_sparkle_get_automatic_check_for_updates;
  pfn_win_sparkle_set_update_check_interval win_sparkle_set_update_check_interval;
  pfn_win_sparkle_get_update_check_interval win_sparkle_get_update_check_interval;
  pfn_win_sparkle_check_update_with_ui win_sparkle_check_update_with_ui;
  pfn_win_sparkle_check_update_without_ui win_sparkle_check_update_without_ui;

  // Lang ISO id string
  EsString m_lang;

  // App data
  EsString m_appVendor;
  EsString m_appName;
  EsString m_appVer;

  // Updatecast URL
  EsString m_updcastUrl;

  bool m_initialized;
  bool m_setup;

private:
  EsAutoUpdater(const EsAutoUpdater&) ES_REMOVEDECL;
  EsAutoUpdater& operator=(const EsAutoUpdater&) ES_REMOVEDECL;
};

#endif // ES_OS == ES_OS_WINDOWS
#endif // ES_USE_AUTO_UPDATER

//---------------------------------------------------------------------------
#endif
