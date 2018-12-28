# top-level cmakefile for ekosfera frameworkGui
cmake_minimum_required(VERSION 3.11)
project(escore-gui)

# wxWidgets dependencies
set(wxWidgets_USE_LIBS base core aui)
find_package(wxWidgets 3.1.1 REQUIRED)
set(ES_WXWIDGETS_VER_STRING ${wxWidgets_VERSION_STRING} CACHE INTERNAL "" FORCE)

include(${wxWidgets_USE_FILE})

# set-up link directories for wxWidgets
link_directories(
  ${wxWidgets_LIB_DIR}
)

include(escoregui.defs.cmake)

# escore configuration header generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/EsCoreGuiConfig.h.in"
  "${ES_PROJECT_ROOT}/EsCoreGuiConfig.h"
)

# component version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/build.h.in"
  "${PROJECT_SOURCE_DIR}/build.h"
)
    
# optional resource version file generation
#
configure_file(
  "${PROJECT_SOURCE_DIR}/res/version.rc.in"
  "${PROJECT_SOURCE_DIR}/res/version.rc"
)

# documentation generator file  
#
configure_file (
  "${PROJECT_SOURCE_DIR}/Doxyfile.in"
  "${PROJECT_SOURCE_DIR}/Doxyfile"
)
  
set(escoregui_common_SRCS
  ${ES_PROJECT_ROOT}/EsCoreGuiConfig.h
  build.h
  escore-gui.h
  EsReflectionGuiLinks.cpp
  EsReflectionGuiLinks.h
  EsListViewBase.cpp
  EsListViewBase.h
  EsAppLog.cpp
  EsAppLog.h
  EsProgressMonitorView.cpp
  EsProgressMonitorView.h
  EsReflectedClassConfigPane.cpp
  EsReflectedClassConfigPane.h
)
source_group(common FILES ${escoregui_common_SRCS})

set(escoregui_utility_SRCS 
  EsAutoUpdater.cpp
  EsAutoUpdater.h  
  EsCoreGuiUtilities.cpp
  EsCoreGuiUtilities.h
)
source_group(utility FILES ${escoregui_utility_SRCS})

set(escoregui_dlg_SRCS 
  EsDlgBase.cpp
  EsDlgBase.h  
)
source_group(escoregui_dlg FILES ${escoregui_dlg_SRCS})

set(escoregui_locale_SRCS  
  locale/escore-gui.pot
  locale/escore-gui.ru_RU.po
#  locale/escore-gui.de_DE.po
)
source_group(locale FILES ${escoregui_locale_SRCS})
ESI18N_ADD("escore-gui" escoregui_locale_SRCS "${binaryRoot}")
                        
set(escoregui_SRCS         
  ${escoregui_common_SRCS}
  ${escoregui_utility_SRCS}
  ${escoregui_dlg_SRCS}
  ${escoregui_locale_SRCS}
)                       
                
if(WIN32)
  set(escore_RES ${escoregui_RES} res/version.rc)
endif(WIN32)                        
source_group(res FILES ${escoregui_RES})

# set-up link directories for wxWidgets
link_directories(
  ${wxWidgets_LIB_DIR}
)

# targets
set(staticMarker "")
if( NOT ES_BUILD_SHARED_LIBS )
    set(staticMarker "_s")
endif()
set(targetName "escoreguiLib${staticMarker}")
set(${targetName} escoregui_${ESGUI_CORE_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker} CACHE INTERNAL "" FORCE)

ES_SPECIFY_PRECOMPILED_HEADER("escore_gui_pch.h" "escore_gui.cpp" escore_SRCS)
ES_SPECIFY_HEADER_FILES(escoregui_SRCS headerExtensions)

add_library(
  ${${targetName}} 
  ${escoregui_SRCS} 
  ${escoregui_RES}
)
target_link_libraries( ${${targetName}} 
	${escoreLib${staticMarker}}
	${wxWidgets_LIBRARIES} 
)

# customize target properties
#
# export preprocessor defines for dll
if( ES_BUILD_SHARED_LIBS )
  if(BORLAND AND EMBARCADERO)
    set_embt_target(Package)
  endif()

  set_property(
    TARGET ${${targetName}}
    APPEND
    PROPERTY COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}" ESCOREGUI_EXPORTS
  )
endif()

