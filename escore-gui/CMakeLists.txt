# cmakefile for esfwx gui core
cmake_minimum_required(VERSION 3.16)
project(escore-gui)

# wxWidgets dependencies
find_package(wxWidgets 3.1.3 REQUIRED COMPONENTS base core aui)
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
  EsWxWindowPtr.cpp
  EsWxWindowPtr.h
)
source_group(utility FILES ${escoregui_utility_SRCS})

set(escoregui_data_view_utils_SRCS
  EsMultilineTextRenderer.cpp
  EsMultilineTextRenderer.h
  EsDataViewStyle.cpp
  EsDataViewStyle.h
)
source_group(data_view_utils FILES ${escoregui_data_view_utils_SRCS})

set(escoregui_data_models_SRCS
  EsListViewModelBase.cpp
  EsListViewModelBase.h
  EsListViewModelArray.cpp
  EsListViewModelArray.h
)
source_group(data_models FILES ${escoregui_data_models_SRCS})

set(escoregui_data_views_SRCS
  EsListViewBase.cpp
  EsListViewBase.h
  EsAppLog.cpp
  EsAppLog.h
)
source_group(data_views FILES ${escoregui_data_views_SRCS})

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
  ${escoregui_data_view_utils_SRCS}
  ${escoregui_data_models_SRCS}
  ${escoregui_data_views_SRCS}
  ${escoregui_dlg_SRCS}
  ${escoregui_locale_SRCS}
)                       
                
if(WIN32)
  set(escore_RES ${escoregui_RES} res/version.rc)
endif(WIN32)                        
source_group(res FILES ${escoregui_RES})

# targets
set(staticMarker "")
if( NOT ES_BUILD_SHARED_LIBS )
    set(staticMarker "_s")
endif()
set(targetName "escoreguiLib${staticMarker}")
set(${targetName} "escoregui_${ESCORE_GUI_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker}" CACHE INTERNAL "" FORCE)
set(ESCOREGUI_LIB_TARGET ${${targetName}} CACHE INTERNAL "")

ES_SPECIFY_PRECOMPILED_HEADER("escore_gui_pch.h" "escore_gui.cpp" escore_SRCS)
ES_SPECIFY_HEADER_FILES(escoregui_SRCS ${ES_HEADER_EXTENSIONS})

add_library(
  ${ESCOREGUI_LIB_TARGET} 
  ${escoregui_SRCS} 
  ${escoregui_RES}
)
target_link_libraries( 
  ${ESCOREGUI_LIB_TARGET} 
	${ESCORE_LIB_TARGET}
	${wxWidgets_LIBRARIES} 
)

message(
  STATUS
  "escore-gui: wxWidgets_LIBRARIES=>${wxWidgets_LIBRARIES}"
)

# Customize target properties
#
# Export preprocessor defines for dll
if( ES_BUILD_SHARED_LIBS )
  if(BORLAND AND EMBARCADERO)
    set_embt_target(Package)
  endif()

  set_property(
    TARGET ${ESCOREGUI_LIB_TARGET}
    APPEND
    PROPERTY COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}" ESCOREGUI_EXPORTS
  )
endif()
