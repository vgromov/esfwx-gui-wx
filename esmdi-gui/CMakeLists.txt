cmake_minimum_required(VERSION 3.16)
project(esmdiguigui)

# wxWidgets dependencies
find_package(wxWidgets 3.1.3 REQUIRED COMPONENTS base core aui)
set(ES_WXWIDGETS_VER_STRING ${wxWidgets_VERSION_STRING} CACHE INTERNAL "" FORCE)

include(${wxWidgets_USE_FILE})

# set-up link directories for wxWidgets
link_directories(
  ${wxWidgets_LIB_DIR}
)

include(esmdigui.config.cmake)

# esmdiguigui configuration header generation
#
#configure_file (
#  "${PROJECT_SOURCE_DIR}/EsMdiGuiConfig.h.in"
#  "${ES_PROJECT_ROOT}/EsMdiGuiConfig.h"
#)

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

# documentation generator file
#
configure_file (
  "${PROJECT_SOURCE_DIR}/Doxyfile.in"
  "${PROJECT_SOURCE_DIR}/Doxyfile"
 )

include_directories(${ES_PROJECT_ROOT})

set(esmdigui_common_SRCS
  #${ES_PROJECT_ROOT}/EsMdiConfig.h
  build.h
  esmdi-gui.h
)
source_group(common FILES ${esmdigui_common_SRCS})

set(esmdigui_ui_SRCS
  EsOpenedDocumentList.h
  EsOpenedDocumentList.cpp
  EsMdiManagerView.h
  EsMdiManagerView.cpp
)
source_group(UI FILES ${esmdigui_ui_SRCS})

#set(esmdigui_locale_SRCS
#  locale/esmdigui.pot
#  locale/esmdigui.ru_RU.po
#  locale/esmdigui.de_DE.po
#)
#source_group(locale FILES ${esmdigui_locale_SRCS})
#ESI18N_ADD("esmdigui" esmdigui_locale_SRCS "${binaryRoot}")

set(esmdigui_SRCS
  ${esmdigui_common_SRCS}
  ${esmdigui_ui_SRCS}
)

if(WIN32)
    set(esmdigui_RES ${esmdigui_RES} res/version.rc)
endif(WIN32)
source_group(res FILES ${esmdigui_RES})

# targets
set(staticMarker "")
if( NOT ES_BUILD_SHARED_LIBS )
  set(staticMarker "_s")
endif()
set(targetName "esmdiguiLib${staticMarker}")
set(${targetName} "esmdigui_${ESMDI_GUI_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker}" CACHE INTERNAL "" FORCE)
set(ESMDIGUI_LIB_TARGET ${${targetName}} CACHE INTERNAL "")
message(
  STATUS
  "esmdigui targetName=>${targetName}:${ESMDIGUI_LIB_TARGET}"
)

ES_SPECIFY_PRECOMPILED_HEADER("esmdi-guipch.h" "EsMdiGuiMain.cpp" esmdigui_SRCS)
ES_SPECIFY_HEADER_FILES(esmdigui_SRCS ${ES_HEADER_EXTENSIONS})

# MDI GUI library
add_library( 
  ${ESMDIGUI_LIB_TARGET}
  ${esmdigui_SRCS}
  ${esmdigui_RES}
#  ${esmdigui_locale_SRCS}
)

target_link_libraries( 
  ${ESMDIGUI_LIB_TARGET}
	${ESCORE_LIB_TARGET}
  ${ESMDI_LIB_TARGET} 
  ${ESCOREGUI_LIB_TARGET}
	${wxWidgets_LIBRARIES} 
)

# Set-up extra library dependencies
# if(WIN32)
  # set(ESMDIGUI_EXTRA_LIBRARIES version winmm rpcrt4)
# else(WIN32)
  # set(ESMDIGUI_EXTRA_LIBRARIES dl pthread)
# endif(WIN32)
# target_link_libraries( 
  # ${ESMDIGUI_LIB_TARGET} 
  # ${ESMDIGUI_EXTRA_LIBRARIES}
# )
# message(
  # STATUS
  # "ESMDIGUI_EXTRA_LIBRARIES=>${ESMDIGUI_EXTRA_LIBRARIES}"
# )

# Customize target properties
#
# export preprocessor defines for dll
if( ES_BUILD_SHARED_LIBS )
  if(BORLAND AND EMBARCADERO)
    set_embt_target(Package)
  endif()

  set_property(
    TARGET ${ESMDIGUI_LIB_TARGET}
    APPEND
    PROPERTY COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}" ESMDIGUI_EXPORTS
  )
endif()
