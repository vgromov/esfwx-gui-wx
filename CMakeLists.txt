# top-level cmakefile for esfwx-gui-wx
cmake_minimum_required(VERSION 3.16)
project(esfwx-gui-wx)

include(esgui.config.cmake)

# esfwx-gui version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_SOURCE_DIR}/version.h"
  )

include_directories(	
  ${ES_PROJECT_ROOT}
  ${ES_PROJECT_ROOT}/esfwx
  ${ES_PROJECT_ROOT}/esfwx/compatibility-headers						
  ${ES_PROJECT_ROOT}/esfwx/external
)
	
# esfwx-gui components and dependencies
#
# escore-gui
add_subdirectory(escore-gui)

if(ESGUICONFIG_USE_COMMGUI)
  add_subdirectory(escomm-gui)
endif()

if(ESGUICONFIG_USE_MDIGUI)
  add_subdirectory(esmdi-gui)
endif()

if(ESGUICONFIG_USE_CHART)
  add_subdirectory(eschart)
endif()
