# cmakefile for eschart component
cmake_minimum_required(VERSION 3.16)
project(eschart)

include(eschart.defs.cmake)

#plplot dependencies
include(FetchContent)
FetchContent_Declare(
  plplot
  GIT_REPOSITORY https://git.code.sf.net/p/plplot/plplot
  GIT_TAG        plplot-5.15.0
)

FetchContent_GetProperties(plplot)
set(PLplotOutputRoot ${binaryRoot}/plplot)

if(NOT plplot_POPULATED)
  FetchContent_Populate(plplot)
  
  # Patch downloaded sources
  execute_process(
    COMMAND git-apply "${PROJECT_SOURCE_DIR}/plplot.patch"
    WORKING_DIRECTORY "${plplot_SOURCE_DIR}"
  )
  
  # Execute generator on PLplot
  execute_process(
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
    -DCMAKE_INSTALL_PREFIX=${PLplotOutputRoot}
    -DDEFAULT_NO_DEVICES=ON
    -DBUILD_SHARED_LIBS=OFF
    -DENABLE_DYNDRIVERS=ON
    -DWITH_CSA=ON
    -S "${plplot_SOURCE_DIR}"
    -B "${plplot_BINARY_DIR}"
  )

  # Execute build and install on PLplot
  execute_process(
    COMMAND ${CMAKE_COMMAND}
    --build "${plplot_BINARY_DIR}"
  )

  execute_process(
    COMMAND ${CMAKE_COMMAND}
    --build "${plplot_BINARY_DIR}" --target INSTALL
  )

  # Copy WX driver declaration file to our source tree
  configure_file(
    ${plplot_SOURCE_DIR}/drivers/wxwidgets.h
    ${PROJECT_SOURCE_DIR}/PLwxwidgets.h
    COPYONLY
  )
  configure_file(
    ${plplot_SOURCE_DIR}/drivers/wxwidgets_comms.h
    ${PROJECT_SOURCE_DIR}/wxwidgets_comms.h
    COPYONLY
  )

endif()

# wxWidgets dependencies
find_package(wxWidgets 3.1.3 REQUIRED COMPONENTS base core)
set(ES_WXWIDGETS_VER_STRING ${wxWidgets_VERSION_STRING} CACHE INTERNAL "" FORCE)

# Required local PLplot
#
# Where to look for PLplot
set(PLplot_ROOT ${PLplotOutputRoot} CACHE STRING "" FORCE)
# Location of FindPLplot.cmake
list(APPEND CMAKE_MODULE_PATH ${plplot_SOURCE_DIR}/cmake)
find_package(PLplot REQUIRED)
set(ES_PLPLOT_VER_STRING ${PLplot_VERSION} CACHE INTERNAL "" FORCE)

#ES_CMAKEVARS_DUMP()

include(${wxWidgets_USE_FILE})

# set-up link directories for wxWidgets && PLplot
link_directories(
  ${wxWidgets_LIB_DIR}
  ${PLplot_LIBRARY_DIRS}
)

include_directories(
  ${PLplot_INCLUDE_DIRS}
)

# escore configuration header generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/EsChartConfig.h.in"
  "${ES_PROJECT_ROOT}/EsChartConfig.h"
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
#configure_file (
#  "${PROJECT_SOURCE_DIR}/Doxyfile.in"
#  "${PROJECT_SOURCE_DIR}/Doxyfile"
#)

# project source files
set(eschart_core_SRCS
  ${ES_PROJECT_ROOT}/EsChartConfig.h
  build.h
  eschart.h
  EsChEvtDefs.h
  EsChPlotObject.h
  EsChPlotObject.cpp
  EsChPlot.cpp
  EsChPlot.h
  EsChPlotAxis.cpp
  EsChPlotAxis.h
  EsChPlotSeries.h
  EsChPlotSeries.cpp
  EsChPlotStreamAdapter.h
  EsChPlotStreamAdapter.cpp
)
source_group(Core FILES ${eschart_core_SRCS})

set(eschart_utils_SRCS
  EsChPlotGeometry.h
  EsChPlotGeometry.cpp
  EsChPlotLineParameters.cpp
  EsChPlotLineParameters.h
  EsChPlotFillParameters.cpp
  EsChPlotFillParameters.h
  EsChPlotPointParameters.cpp
  EsChPlotPointParameters.h
  EsChPlotTextParameters.cpp
  EsChPlotTextParameters.h
  EsChPlotPalette.cpp
  EsChPlotPalette.h
  EsChPlotSeriesRenderer.h
  EsChPlotSeriesRenderer.cpp
  EsChPlotDecorations.h
  EsChPlotDecorations.cpp
)
source_group(Utilities FILES ${eschart_utils_SRCS})
						
# set(plwxdriver_SRCS
# plwxdriver/wxwidgets.cpp
# plwxdriver/wxwidgets.h
# #plwxdriver/wxwidgets_agg.cpp
# #plwxdriver/wxwidgets_app.cpp
# plwxdriver/wxwidgets_dc.cpp
# plwxdriver/wxwidgets_gc.cpp
# )
# source_group(wxplplot_driver FILES ${plwxdriver_SRCS})	

set(eschart_ui_SRCS
  EsChPlotView.h
  EsChPlotView.cpp
  EsChPlotPanel.cpp
  EsChPlotPanel.h
  EsChSpecView.cpp
  EsChSpecView.h
)
source_group(UI FILES ${eschart_ui_SRCS})						

# set(eschart_locale_SRCS
# locale/eschart.pot
# locale/eschart.ru_RU.po						
# )
# source_group(locale FILES ${eschart_locale_SRCS})
# I18N_ADD("esshell" eschart_locale_SRCS "${binaryRoot}")

if( ES_BUILD_SHARED_LIBS )

  if(WIN32)
    set(eschart_RES ${eschart_RES} res/version.rc)
  endif(WIN32)						

  source_group(res FILES ${eschart_RES})
 
endif()

set(eschart_SRCS
  ${eschart_core_SRCS}
  ${eschart_utils_SRCS}
#  ${plwxdriver_SRCS}
  ${eschart_ui_SRCS}
)	

# targets
set(staticMarker "")
if( NOT ES_BUILD_SHARED_LIBS )
  set(staticMarker "_s")
endif()
set(targetName "eschartLib${staticMarker}")
set(${targetName} eschart_${ESCHART_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker} CACHE INTERNAL "" FORCE)
set(ESCHART_LIB_TARGET ${${targetName}} CACHE INTERNAL "")
message(
  STATUS
  "eschart targetName=>${targetName}:${ESCHART_LIB_TARGET}"
)

ES_SPECIFY_PRECOMPILED_HEADER("eschart_pch.h" "EsChMain.cpp" eschart_SRCS)
ES_SPECIFY_HEADER_FILES(eschart_SRCS ${ES_HEADER_EXTENSIONS})

add_library(${ESCHART_LIB_TARGET}
  ${eschart_SRCS}
  ${eschart_RES}
)

target_link_libraries(${ESCHART_LIB_TARGET}
	${ESCORE_LIB_TARGET}
  ${ESMATH_LIB_TARGET}
	${ESCOREGUI_LIB_TARGET}
  ${wxWidgets_LIBRARIES}
  ${PLplot_LIBRARIES}
  ${PLplot_EXTRA_LIBRARIES}
)

# Customize target properties
#
# Export preprocessor defines for dll
if( ES_BUILD_SHARED_LIBS )
  if(BORLAND AND EMBARCADERO)
    set_embt_target(Package)
  endif()

  set_property(
    TARGET ${ESCHART_LIB_TARGET}
    APPEND
    PROPERTY COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}" ESCHART_EXPORTS
  )
endif()
