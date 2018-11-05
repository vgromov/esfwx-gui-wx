# static framework components linkage pseudo-project
cmake_minimum_required(VERSION 3.4)
project(esfwx-gui-static-linkage)

# reset certain cache values
# remember BUILD_SHARED_LIBS setting to restore it later
set(oldBUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(../escore-gui ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEscoreGui)
add_subdirectory(../escomm-gui ${CMAKE_CURRENT_SOURCE_DIR}/tempStaticEscommGui)

# export common static-linkage targets for aux projects
export(TARGETS 
	${escoreguiLib_s} 
	${escommguiLib_s}	
  FILE ${PROJECT_BINARY_DIR}/esframeworkGui-static.cmake
	)
	   
# restore shared libs option					
set(BUILD_SHARED_LIBS ${oldBUILD_SHARED_LIBS} CACHE BOOL "" FORCE)	   