# top-level cmakefile for ekosfera frameworkGui
cmake_minimum_required(VERSION 3.10)
project(frameworkGui)

set(ESFWX_STANDALONE_BUILD 0 CACHE BOOL "Build GUI framework components as part of some project (Default - 0), or as stand-alone (1)")

# frameworkGui version definitions
set(frameworkGuiVendor			"ECO-Electronics" CACHE INTERNAL "")
set(frameworkGuiVendorShort "ECO-E" CACHE INTERNAL "")
set(frameworkGui						"ECO-E GUI framework" CACHE INTERNAL "")
set(frameworkGuiCopyright	  "Copyright 2009-2018" CACHE INTERNAL "")
set(frameworkGui_VER_MAJOR	1 CACHE INTERNAL "")
set(frameworkGui_VER_MINOR	24 CACHE INTERNAL "")
set(frameworkGui_BUILD_NUM	0 CACHE INTERNAL "")	

# frameworkGui version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_SOURCE_DIR}/version.h"
  )

  # build frameworkGui components in standalone mode
  # otherwise, host project defines which components to build
 if( ${ESFWX_STANDALONE_BUILD} )
  set(BUILD_SHARED_LIBS 0 CACHE BOOL "Build shared GUI framework libraries (Default - 0 i.e. static)")
	include(esframework-macros.cmake)
	if(MSVC)
		# use UNICODE under MSVC
		add_definitions(-DUNICODE -D_UNICODE)
		# suppress msvc security warnings
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS")
	endif()
  
  # global directories
	get_filename_component(root ${PROJECT_BINARY_DIR}/bin REALPATH)
	set(binaryRoot ${root} CACHE PATH "Build binary root" FORCE)
  set(escoregui_CONFIG_DIR ${PROJECT_SOURCE_DIR} CACHE INTERNAL "escore-gui configuration directory" FORCE)
  
	set(EXECUTABLE_OUTPUT_PATH ${binaryRoot})
	set(LIBRARY_OUTPUT_PATH ${binaryRoot})	

	# global preprocessor defines
	#
	# for dynamic linkage to es-core libraries
	set(dynamicLinkPreprocessorDefinesExe ES_USE_DLLS)
	# for dynamic linkage to es-core libraries and proper dll interface generation
	set(dynamicLinkPreprocessorDefines ${dynamicLinkPreprocessorDefinesExe} _USRDLL)
	# for each subproject, add project current source and build dirs to the include directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	# reset i18n aggregation variables
	I18N_RESET() 
	# localization integration variables
	set(i18nLanguages "" CACHE INTERNAL "" FORCE)
	set(i18nComponents "" CACHE INTERNAL "" FORCE)
	set(poPattern ".+[.]([a-z][a-z]_[A-Z][A-Z])[.]po" CACHE INTERNAL "" FORCE)	
	
	include_directories(	${PROJECT_SOURCE_DIR}
                        ${PROJECT_SOURCE_DIR}/../esfwx
												${PROJECT_SOURCE_DIR}/../esfwx/compatibility-headers						
												${PROJECT_SOURCE_DIR}/../esfwx/external
												)
	
	# frameworkGui components
	#
	add_subdirectory(escore-gui)
	add_subdirectory(escomm-gui)
	
 endif()