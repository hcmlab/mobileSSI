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

# this project only builds with android-ndk!
project(openssi-androidjavasensors)

#add_dependencies(ssiandroidjavasensors_test ssi ssiandroidjavasensors)




include_directories(include
					../../core/include/ioput/socket
					../../core/include/ioput
					../../core/include
					../

                                        /usr/lib/jvm/java-8-openjdk/include
                                        /usr/lib/jvm/java-8-openjdk/include/linux
                                        /usr/lib/jvm/java-8-openjdk-amd64/include
                                        /usr/lib/jvm/java-8-openjdk-amd64/include/linux

                     ${ANDROID_SYSTEM_INCLUDE_DIRS}
                    ${ANDROID_NDK}/sources/android/native_app_glue/
)

message(STATUS ${ANDROID_SYSTEM_INCLUDE_DIRS})
#message(STATUS "${ANDROID_SYSROOT}/usr/include")

set(COMMON_SRC_FILES

source/ExportMain.cpp
source/AndroidJavaSensors.cpp

)


 set(SRC_FILES ${COMMON_SRC_FILES} )




#find_library(OPENSSI_CORE_LIBRARY NAMES openssi-core HINTS ../../cmake-core/corelib/Debug/ ../../cmake-core/corelib/Release/)

IF(UNIX)

    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSI_CFLAG}" )
IF(ANDROID)
  set( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SSI_LD_FLAG}" )
  
#hackishly linking native sensor api
  set(ANDROID_LIB "-landroid -llog")
ENDIF(ANDROID)

ENDIF(UNIX)

add_library(ssiandroidjavasensors SHARED ${SRC_FILES} )

target_link_libraries(ssiandroidjavasensors ssi ${ANDROID_LIB} -ldl)


#add_executable(myapp main.c)

#rename targets if debug

  if(ANDROID)

  else(ANDROID)
set_target_properties(ssiandroidjavasensors PROPERTIES DEBUG_POSTFIX "d" PREFIX "")
  endif(ANDROID)

#install target to ssi install path set in base directory
install(TARGETS ssiandroidjavasensors DESTINATION ${SSI_INSTALL}/${SSI_PLATFORM}/${SSI_COMPILER})
