cmake_minimum_required(VERSION 2.8)
project(escomm-gui)

# component version definitions
set(component_DESCRIPTION_STRING "Ekosfera Comm GUI Library")
set(component_INTERNAL_NAME "escomm-gui")
set(component_ORIGINAL_NAME "escomm-gui.dll")
set(component_BUILD_NUM	52)	

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

# wxWidgets dependencies
set(wxWidgets_USE_LIBS base core)
find_package(wxWidgets 2.9 REQUIRED)
include(${wxWidgets_USE_FILE})

# project top-level build directory
set(escommgui_common_SRCS
						build.h
						ekosf-comm-gui.h
						EsCommConfigDlg.cpp
						EsCommConfigDlg.h
						EsCommConfigPane.cpp
						EsCommConfigPane.h
						EsProtocolConfigPane.cpp
						EsProtocolConfigPane.h
						EsChannelIoConfigPaneFactory.cpp
						EsChannelIoConfigPaneFactory.h
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
source_group(EsChannelIoStub FILES ${escommgui_ChannelIoStub_SRCS})	

set(escommgui_locale_SRCS 
						locale/escomm-gui.pot
						locale/escomm-gui.ru_RU.po
						)
source_group(locale FILES ${escommgui_locale_SRCS})
I18N_ADD("esshell" escommgui_locale_SRCS "${binaryRoot}")

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
if( NOT BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "escommguiLib${staticMarker}")
set(${targetName} escommgui_${binarySuffix}${compilerVersion}${staticMarker} CACHE STRING "" FORCE)

SPECIFY_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" escommgui_SRCS)
SPECIFY_HEADER_FILES(escommgui_SRCS headerExtensions)

add_library(${${targetName}} 
	${escommgui_SRCS} 
	${escommgui_RES}
	)
target_link_libraries(${${targetName}} 
	${wxWidgets_LIBRARIES} 
	${escoreLib${staticMarker}}
	${escommLib${staticMarker}}
	${escoreguiLib${staticMarker}}
	)

# customize target properties
#
# export preprocessor defines for dll
if( BUILD_SHARED_LIBS )
	set_target_properties(${${targetName}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						DEFINE_SYMBOL ESCOMMGUI_EXPORTS)
endif()
