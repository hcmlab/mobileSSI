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
cmake_minimum_required(VERSION 2.8)

project(ssimqtt)



IF(SSI_GUI STREQUAL "SDL")
	IF(UNIX)
		INCLUDE(FindPkgConfig)
		pkg_check_modules(CAIRO REQUIRED cairo)
		pkg_check_modules(SDL2 REQUIRED sdl2)
	endif(UNIX)
endif(SSI_GUI STREQUAL "SDL")

include_directories(
			include
			../../core/include/ioput/socket
			../../core/include/ioput
			../../core/include
			../../core/include/frame
			../
			${CAIRO_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS}
			)

set(COMMON_SRC_FILES

source/ExportMain.cpp
source/mongoose.c
source/mqtt.cpp
source/mqttclient.cpp
source/mqttserver.cpp


)
 IF( SSI_GUI STREQUAL "SDL")
  set(GUI_SRC_FILES

	)
ELSE( SSI_GUI STREQUAL "SDL")
  set(GUI_SRC_FILES )
ENDIF( SSI_GUI STREQUAL "SDL")

 IF(WIN32)
 set(P_SRC_FILES

	)
 ELSE(WIN32)
 set(P_SRC_FILES

	)
 ENDIF(WIN32)





 set(SRC_FILES ${COMMON_SRC_FILES} ${P_SRC_FILES} ${GUI_SRC_FILES})

#find_library(OPENSSI_CORE_LIBRARY NAMES openssi-core HINTS ../../cmake-core/corelib/Debug/ ../../cmake-core/corelib/Release/)
IF(MINGW)
  find_library (MINGW_WSOCKET "wsock32" HINTS ${MINGWLIB} )
  find_library (MINGW_WMM "winmm" HINTS ${MINGWLIB})
  find_library (MINGW_WSOCKET2 "ws2_32" HINTS ${MINGWLIB} )
  find_library (MINGW_PTHREAD "pthread" HINTS ${MINGWLIB} )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb -D NS_ENABLE_THREADS -D MG_ENABLE_MQTT_BROKER" )
ELSEIF(UNIX)

  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSI_CFLAG} -D NS_ENABLE_THREADS -D MG_ENABLE_MQTT_BROKER" )

IF(ANDROID)
  set( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  set( MINGW_PTHREAD "")
ELSE(ANDROID)
  find_library (MINGW_PTHREAD "pthread" HINTS "/usr/lib" )

ENDIF(ANDROID)
ELSE(MINGW)
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
   SET(MINGW_PTHREAD "")
ENDIF(MINGW)




add_library(ssimqtt SHARED ${SRC_FILES} )
IF(UNIX)
target_link_libraries(ssimqtt ssi ${MINGW_WSOCKET} ${MINGW_WMM} ${MINGW_WSOCKET2} ${MINGW_PTHREAD} -ldl)
ELSE(UNIX)
target_link_libraries(ssimqtt ssi ${MINGW_WSOCKET} ${MINGW_WSOCKET2}  ${MINGW_WMM} ${MINGW_PTHREAD})
ENDIF(UNIX)
#add_executable(myapp main.c)

#rename targets if debug
  
  if(ANDROID)
  else(ANDROID)
set_target_properties(ssimqtt PROPERTIES DEBUG_POSTFIX "d" PREFIX "")
  endif(ANDROID)

#install target to ssi install path set in base directory
install(TARGETS ssimqtt DESTINATION ${SSI_INSTALL}/${SSI_PLATFORM}/${SSI_COMPILER})
