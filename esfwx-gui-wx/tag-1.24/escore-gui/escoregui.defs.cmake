# Component version definitions
set(component_DESCRIPTION_STRING "ESFWX Core GUI Library for WX backend")
set(component_INTERNAL_NAME "escore-gui")
set(component_ORIGINAL_NAME "escore-gui")
set(component_BUILD_NUM 2)

# Set-up ESCORE binary version naming
set(ESGUI_CORE_VERSION_SUFFIX "${ESGUI_VERSION_SUFFIX}_${component_BUILD_NUM}" CACHE INTERNAL "")

if(WIN32)
set(ESGUICONFIG_USE_AUTO_UPDATER 1 CACHE BOOL "Use Sparkle Online AutoUpdater wrapper class")
endif()

if(ESGUICONFIG_USE_AUTO_UPDATER)
  set(ES_USE_AUTO_UPDATER ${ESGUICONFIG_USE_AUTO_UPDATER})
else()
  set(ES_USE_AUTO_UPDATER 0)
endif()
