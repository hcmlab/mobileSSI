#-----------------------------------------------------------------------------
# Initialize project.

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	if(NOT DEFINED WITH_IN_SOURCE_BUILD)
		message(FATAL_ERROR
			"CMake generation for openssi is not allowed within the source directory!"
			"\n Remove the CMakeCache.txt file and try again from another folder, e.g.:"
			"\n "
			"\n rm CMakeCache.txt"
			"\n cd .."
			"\n mkdir cmake-make"
			"\n cd cmake-make"
			"\n cmake ../core"
			"\n "
			"\n Alternately define WITH_IN_SOURCE_BUILD to force this option (not recommended!)"
		)		
	endif()
endif()

#http://stackoverflow.com/questions/25609692/how-to-add-source-files-in-another-folder
#file(GLOB Dir1_Sources RELATIVE "Dir1" "*.cpp")
#http://www.opentissue.org/mediawiki/index.php/Using_CMake
#http://stackoverflow.com/questions/15175318/cmake-how-to-build-external-projects-and-include-their-targets
#http://www.cmake.org/Wiki/CMake:Install_Commands
cmake_minimum_required(VERSION 2.8)

project(openssi-eventTimeWarp)


if(ANDROID)
else(ANDROID)

endif(ANDROID)



include_directories(include ../../core/include/ioput/socket ../../core/include/ioput ../../core/include ../ ${CAIRO_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

set(COMMON_SRC_FILES


source/EventTimeWarp.cpp

source/ExportMain.cpp


)



 

 
 set(SRC_FILES ${COMMON_SRC_FILES} ${P_SRC_FILES} ${GUI_SRC_FILES})




#find_library(OPENSSI_CORE_LIBRARY NAMES openssi-core HINTS ../../cmake-core/corelib/Debug/ ../../cmake-core/corelib/Release/)

IF(MINGW)
  find_library (MINGW_WSOCKET "wsock32" HINTS ${MINGWLIB} )
  find_library (MINGW_WMM "winmm" HINTS ${MINGWLIB})
  find_library (MINGW_WSOCKET2 "ws2_32" HINTS ${MINGWLIB} )
  find_library (MINGW_PTHREAD "pthread" HINTS ${MINGWLIB} )
  	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb" )
  ELSEIF(UNIX)
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")

  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSI_CFLAG}" )
  set( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SSI_LD_FLAG}" )
   if (ANDROID)
    SET(MINGW_PTHREAD "")
  else (ANDROID)
  find_library (MINGW_PTHREAD "pthread" HINTS "/usr/lib" )
  endif (ANDROID)
  
ELSE(MINGW)
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
   SET(MINGW_PTHREAD "")
ENDIF(MINGW)

add_library(ssieventTimeWarp SHARED ${SRC_FILES} )
target_link_libraries(ssieventTimeWarp ssi ${MINGW_WSOCKET}  ${MINGW_WSOCKET2} ${MINGW_WMM} ${MINGW_PTHREAD} )
#add_executable(myapp main.c)

#rename targets if debug
  if(ANDROID)

  else(ANDROID)
set_target_properties(ssieventTimeWarp PROPERTIES DEBUG_POSTFIX "d" PREFIX "")
  endif(ANDROID)

#install target to ssi install path set in base directory
install(TARGETS ssieventTimeWarp DESTINATION ${SSI_INSTALL}/${SSI_PLATFORM}/${SSI_COMPILER})
