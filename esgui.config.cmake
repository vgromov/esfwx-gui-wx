# frameworkGui version definitions
set(frameworkGuiVendor			"ECO-Electronics" CACHE INTERNAL "")
set(frameworkGuiVendorShort "ECO-E" CACHE INTERNAL "")
set(frameworkGui						"ECO-E GUI framework" CACHE INTERNAL "")
set(frameworkGuiCopyright	  "Copyright 2009-2019" CACHE INTERNAL "")
set(frameworkGui_VER_MAJOR	1 CACHE INTERNAL "")
set(frameworkGui_VER_MINOR	24 CACHE INTERNAL "")
set(frameworkGui_BUILD_NUM	4 CACHE INTERNAL "")	

set(ESGUICONFIG_USE_COMMGUI 0 CACHE BOOL "Build COMM GUI shared classes. Default=OFF")
set(ESGUICONFIG_USE_MDIGUI 0 CACHE BOOL "Build MDI GUI shared classes. Default=OFF")
set(ESGUICONFIG_USE_CHART 0 CACHE BOOL "Build CHART shared classes. Default=OFF")

set(ESGUI_VERSION_SUFFIX  "v${frameworkGui_VER_MAJOR}_${frameworkGui_VER_MINOR}_${frameworkGui_BUILD_NUM}" CACHE INTERNAL "")
message(
  STATUS
  "ESGUI_VERSION_SUFFIX=>${ESGUI_VERSION_SUFFIX}"
)
