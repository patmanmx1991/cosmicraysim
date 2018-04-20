# $Id: CMakeLists.txt 68058 2013-03-13 14:47:43Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(COSMO)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()




#----------------------------------------------------------------------------
# Find ROOT variables if the variable G4ANALYSIS_USE_ROOT is set
#
find_package(ROOT QUIET)



#---Set Link and include directories--------------------------------------------------------------
#include_directories(${ROOT_INCLUDE_DIRS})
#link_directories(${ROOT_LIBRARY_DIR})

#---Set Flags-------------------------------------------------------------------------------------
#add_definitions(${ROOT_DEFINITIONS})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ROOT_C_FLAGS}")
#set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${ROOT_fortran_FLAGS}")

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
if(ROOT_FOUND)
add_definitions(-DG4ANALYSIS_USE_ROOT)
include(${Geant4_USE_FILE})
include_directories(${ROOT_INCLUDE_DIR} ${Geant4_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src/RAT/ ${PROJECT_SOURCE_DIR}/src/ ${PROJECT_SOURCE_DIR}/src/geo/)
else()
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/src/ ${PROJECT_SOURCE_DIR}/src/ ${PROJECT_SOURCE_DIR}/src/geo/)
endif()


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*/*.cc ${PROJECT_SOURCE_DIR}/src/geo/*/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*/*.hh ${PROJECT_SOURCE_DIR}/src/geo/*/*.cc)

#----------------------------------------------------------------------------
# Create the DB Library


#ROOT_GENERATE_DICTIONARY(DBlib ${PROJECT_SOURCE_DIR}/src/db/DBTable.h LINKDEF ${PROJECT_SOURCE_DIR}/src/db/DBTable_LinkDef.h)
#add_library(myDBlib SHARED ${PROJECT_SOURCE_DIR}/src/db/DBTable.cc DBlib.cxx)
#target_link_libraries(myDBlib ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(cosmicraysim ${PROJECT_SOURCE_DIR}/app/cosmicraysim.cc ${sources} ${headers})
if(ROOT_FOUND)
target_link_libraries(cosmicraysim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
else()
target_link_libraries(cosmicraysim ${Geant4_LIBRARIES})
endif()


#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(COSMO DEPENDS cosmicraysim)


#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B3. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(COSMIC_SCRIPTS
  debug.mac
  init.mac
  init_vis.mac
  vis.mac
  )

foreach(_script ${COSMIC_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/macros/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()


#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS cosmicraysim DESTINATION bin )
