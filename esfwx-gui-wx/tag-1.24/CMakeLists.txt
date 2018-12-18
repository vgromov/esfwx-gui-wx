# top-level cmakefile for ekosfera frameworkGui
cmake_minimum_required(VERSION 3.10)
project(frameworkGui)

include(esgui.config.cmake)

# frameworkGui version file generation
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
	
# frameworkGui components
#
add_subdirectory(escore-gui)

if(ESGUICONFIG_USE_COMMGUI)
  add_subdirectory(escomm-gui)
endif()
