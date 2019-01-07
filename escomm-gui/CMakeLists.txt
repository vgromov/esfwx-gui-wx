cmake_minimum_required(VERSION 3.10)
project(escomm-gui)

# wxWidgets dependencies
set(wxWidgets_USE_LIBS base core)
find_package(wxWidgets 3.1.1 REQUIRED)

include(${wxWidgets_USE_FILE})

# set-up link directories for wxWidgets
link_directories(
  ${wxWidgets_LIB_DIR}
)

include(escommgui.defs.cmake)

# component version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/build.h.in"
  "${PROJECT_SOURCE_DIR}/build.h"
)
# optional resource version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/res/version.rc.in"
  "${PROJECT_SOURCE_DIR}/res/version.rc"
)

# project top-level build directory
set(escommgui_common_SRCS
  build.h
  escomm-gui.h
  EsCommConfigDlg.cpp
  EsCommConfigDlg.h
  EsChannelIoConfigPane.cpp
  EsChannelIoConfigPane.h
  EsProtocolEcoeConfigPane.cpp
  EsProtocolEcoeConfigPane.h
  EsIoConfigPaneFactory.cpp
  EsIoConfigPaneFactory.h
)
source_group(common FILES ${escommgui_common_SRCS})
						
set(escommgui_ChannelIoEkonnect_SRCS
  EsChannelIoEkonnectConfigPane.cpp
  EsChannelIoEkonnectConfigPane.h
)
source_group(ChannelIoEkonnect FILES ${escommgui_ChannelIoEkonnect_SRCS})						

set(escommgui_ChannelIoSocket_SRCS
  EsChannelIoSocketClientConfigPane.cpp
  EsChannelIoSocketClientConfigPane.h
  EsChannelIoSocketServerConfigPane.cpp
  EsChannelIoSocketServerConfigPane.h
)
source_group(ChannelIoSocket FILES ${escommgui_ChannelIoSocket_SRCS})

set(escommgui_ChannelIoUart_SRCS						
  EsChannelIoUartConfigPane.cpp
  EsChannelIoUartConfigPane.h
)
source_group(ChannelIoUart FILES ${escommgui_ChannelIoUart_SRCS})

set(escommgui_ChannelIoStub_SRCS
  EsChannelIoStubConfigPane.cpp
  EsChannelIoStubConfigPane.h
)
source_group(ChannelIoStub FILES ${escommgui_ChannelIoStub_SRCS})	

set(escommgui_locale_SRCS 
  locale/escomm-gui.pot
  locale/escomm-gui.ru_RU.po
)
source_group(locale FILES ${escommgui_locale_SRCS})
ESI18N_ADD("esshell" escommgui_locale_SRCS "${binaryRoot}")

set(escommgui_SRCS
  ${escommgui_common_SRCS}
  ${escommgui_ChannelIoStub_SRCS}
  ${escommgui_ChannelIoUart_SRCS}
  ${escommgui_ChannelIoEkonnect_SRCS}
  ${escommgui_ChannelIoSocket_SRCS}
  ${escommgui_locale_SRCS}
)					
						
set(escommgui_RES
  res/rescan.xpm
)
source_group(res FILES ${escommgui_RES})
						
if(WIN32)
	set(escommgui_RES ${escommgui_RES} res/version.rc)
endif(WIN32)						
source_group(res FILES ${escommgui_RES})

# targets
set(staticMarker "")
if( NOT ES_BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "escommguiLib${staticMarker}")
set(${targetName} escommgui_${ESGUI_COMM_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker} CACHE INTERNAL "" FORCE)

ES_SPECIFY_PRECOMPILED_HEADER("escomm_gui_pch.h" "escomm_gui.cpp" escommgui_SRCS)
ES_SPECIFY_HEADER_FILES(escommgui_SRCS headerExtensions)

add_library(
  ${${targetName}} 
	${escommgui_SRCS} 
	${escommgui_RES}
)
target_link_libraries(
  ${${targetName}} 
	${wxWidgets_LIBRARIES} 
	${escoreLib${staticMarker}}
	${escommLib${staticMarker}}
	${escoreguiLib${staticMarker}}
)

# Customize target properties
#
# Export preprocessor defines for dll
if( ES_BUILD_SHARED_LIBS )
  if(BORLAND AND EMBARCADERO)
    set_embt_target(Package)
  endif()

  set_property(
    TARGET ${${targetName}}
    APPEND
    PROPERTY COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}" ESCOMMGUI_EXPORTS
  )
endif()

